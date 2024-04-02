#include "Eigen/Dense"
#include "Eigen/Sparse"
#include "GCore/Components/MeshOperand.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "utils/util_openmesh_bind.h"

namespace USTC_CG::node_arap {
static void node_arap_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Geometry>("Input");
    b.add_input<decl::Float2Buffer>("InitialTexCoords");
    b.add_input<decl::String>("lambda");
    b.add_output<decl::Float2Buffer>("TexCoords");
}

static void node_arap_exec(ExeParams params)
{
    auto input = params.get_input<GOperandBase>("Input");
    if (!input.get_component<MeshComponent>()) {
        throw std::runtime_error("Need Geometry Input.");
    }

    auto mesh = operand_to_openmesh(&input);
    auto n = (long long)mesh->n_faces();
    if (n == 0) {
        throw std::runtime_error("No faces in the mesh.");
    }

    auto lambda_string = params.get_input<std::string>("lambda");
    double lambda;
    try {
        lambda = std::stod(lambda_string);
    }
    catch (...) {
        throw std::runtime_error("Invalid lambda value.");
    }

    auto texture_coords = params.get_input<pxr::VtArray<pxr::GfVec2f>>("InitialTexCoords");

    std::vector<std::array<int, 3>> x_id(n);              // n u 3     x_id (face -> vertex)
    std::vector<std::array<Eigen::Vector2d, 3>> x(n);     // n u 3 u 2 x
    std::vector<std::array<PolyMesh::Point, 3>> x_3d(n);  // n u 3 u 2 x_3d
    std::vector<std::array<double, 3>> x_cot_angles(n);   // n u 3     x_cot_angles

    std::vector<std::array<Eigen::Vector2d, 3>> u(n);  // n u 3 u 2 u
    std::vector<Eigen::Matrix2d> Matrix_list(n);       // n u 2 u 2 Lt

    // Local Phase:
    for (const auto& f : mesh->faces()) {
        /*
         * In the following code, the label 0,1,2 is used in different places,
         * here's what each of them would represent to avoid confusion:
         *
         *
         * Vertex:
         * the structure is given by f.vertices(), which provides the vertices in a consistent order
         * we label the vertices as 0, 1, 2 in the order they are provided by f.vertices()
         *
         * Angles:
         * the cotangent angle of the edge opposite to the vertex labeled i is x_cot_angles[i]
         * => angle( edge(i,i-1) , edge(i,i+1) )
         * (i-1 = i+2 MOD 3)
         *
         * Edge should be understood as the edge opposite to the vertex labeled i
         * (so the only edge that doesn't have vertex i as an endpoint)
         */

        auto t = f.idx();
        auto S = Eigen::Matrix2d();
        S.setZero();

        {      // x: a local 2D coordinate system)
            {  // Get 3D coordinates of the vertices of the face
                int i = 0;
                for (auto vertex : f.vertices()) {
                    x_id[t][i] = vertex.idx();
                    x_3d[t][i] = mesh->point(vertex);
                    i++;
                }
            }

            x[t][0] = { 0, 0 };
            x[t][1] = { (x_3d[t][1] - x_3d[t][0]).norm(), 0 };
            {  // x[t][2]
                auto edge1 = x_3d[t][1] - x_3d[t][0];
                auto edge2 = x_3d[t][2] - x_3d[t][0];
                auto cos_angle = edge1.dot(edge2) / (edge1.norm() * edge2.norm());
                auto sin_angle = std::sqrt(1 - cos_angle * cos_angle);
                x[t][2] = { edge2.norm() * cos_angle, edge2.norm() * sin_angle };
            }

            {  // cot angles:
                for (int i = 0; i < 3; i++) {
                    auto edge1 = x_3d[t][(i + 1) % 3] - x_3d[t][i];
                    auto edge2 = x_3d[t][(i + 2) % 3] - x_3d[t][i];
                    x_cot_angles[t][i] = edge1.dot(edge2) / (edge1.cross(edge2).norm());
                }
            }
        }

        // u: (the 2D texture coordinates)
        for (int i = 0; i < 3; i++) {
            auto tmp = texture_coords[x_id[t][i]];
            u[t][i] = { tmp[0], tmp[1] };
        }

        // S:
        for (int i = 0; i < 3; i++) {
            auto j = (i + 1) % 3;
            auto k = (i + 2) % 3;
            S += x_cot_angles[t][k] * ((u[t][i] - u[t][j]) * (x[t][i] - x[t][j]).transpose());
        }

        // SVD:
        Eigen::JacobiSVD<Eigen::Matrix2d> svd(S, Eigen::ComputeFullU | Eigen::ComputeFullV);
        const auto& U = svd.matrixU();
        const auto& V = svd.matrixV();

        // Lt:
        auto Lt = U * V.transpose();
        Matrix_list[f.idx()] = Lt;
    }

    auto vertex_id_in_face = [](const PolyMesh::VertexHandle& vertex,
                                const OpenMesh::SmartFaceHandle& face) {
        int i = 0;
        for (const auto& v : face.vertices()) {
            if (v.idx() == vertex.idx())
                return i;
            i++;
        }
        throw std::runtime_error("Vertex not found in face.");
    };

    auto m = (long long)mesh->n_vertices();
    auto A = Eigen::SparseMatrix<double>(m, m);
    A.setZero();
    auto A_ = std::vector<Eigen::Triplet<double>>();

    // Global Phase 1: Build the matrix A
    for (const auto& vertex_i : mesh->vertices()) {
        auto id_i = vertex_i.idx();
        double temp_i_eff = 0.0;

        if (id_i == 0) {
            A_.emplace_back(id_i, id_i, 1.0);
            continue;
        }

        auto process_half_edge = [&](const OpenMesh::SmartHalfedgeHandle& half_edge) {
            double temp_j_eff = 0.0;

            auto vertex_i = half_edge.opp().to();  // intended shadowing.
            auto vertex_j = half_edge.to();
            auto id_j = vertex_j.idx();
            auto face = half_edge.face();

            if (face.is_valid()) {
                auto t = face.idx();
                auto i = vertex_id_in_face(vertex_i, face);
                auto j = vertex_id_in_face(vertex_j, face);
                auto k = (0 + 1 + 2) - i - j;
                temp_j_eff += x_cot_angles[t][k];
            }

            face = half_edge.opp().face();

            if (face.is_valid()) {
                auto t = face.idx();
                auto i = vertex_id_in_face(vertex_i, face);
                auto j = vertex_id_in_face(vertex_j, face);
                auto k = (0 + 1 + 2) - i - j;
                temp_j_eff += x_cot_angles[t][k];
            }

            A_.emplace_back(id_i, id_j, -temp_j_eff);
            temp_i_eff += temp_j_eff;
        };

        for (const auto& half_edge : vertex_i.outgoing_halfedges()) {
            process_half_edge(half_edge);
        }

        A_.emplace_back(id_i, id_i, temp_i_eff);
    }
    A.setFromTriplets(A_.begin(), A_.end());

    auto solver = Eigen::SimplicialCholesky<Eigen::SparseMatrix<double>>();
    solver.compute(A);

    // Global Phase 2: Build vector b:
    auto bx = Eigen::VectorXd(m);
    auto by = Eigen::VectorXd(m);
    bx.setZero();
    by.setZero();

    for (const auto& vertex_i : mesh->vertices()) {
        auto id_i = vertex_i.idx();

        if (id_i == 0) {
            bx(id_i) = 0;
            by(id_i) = 0;
            continue;
        }

        Eigen::Vector2d tmp_vector;
        tmp_vector.setZero();

        auto process_half_edge = [&](const OpenMesh::SmartHalfedgeHandle& half_edge) {
            auto vertex_i = half_edge.opp().to();  // intended shadowing.
            auto vertex_j = half_edge.to();

            auto face = half_edge.face();
            if (face.is_valid()) {
                auto t = face.idx();
                auto i = vertex_id_in_face(vertex_i, face);
                auto j = vertex_id_in_face(vertex_j, face);
                auto k = (0 + 1 + 2) - i - j;

                tmp_vector += x_cot_angles[t][k] * (Matrix_list[t] * (x[t][i] - x[t][j]));
            }

            face = half_edge.opp().face();
            if (face.is_valid()) {
                auto t = face.idx();
                auto i = vertex_id_in_face(vertex_i, face);
                auto j = vertex_id_in_face(vertex_j, face);
                auto k = (0 + 1 + 2) - i - j;

                tmp_vector += x_cot_angles[t][k] * (Matrix_list[t] * (x[t][i] - x[t][j]));
            }
        };
        for (const auto& half_edge : vertex_i.outgoing_halfedges()) {
            process_half_edge(half_edge);
        }

        bx(id_i) = tmp_vector(0);
        by(id_i) = tmp_vector(1);
    }

    Eigen::VectorXd new_x = solver.solve(bx);
    Eigen::VectorXd new_y = solver.solve(by);

    pxr::VtArray<pxr::GfVec2f> uv_result;
    for (int i = 0; i < m; i++) {
        uv_result.emplace_back(pxr::GfVec2f{ (float)(new_x[i]), (float)(new_y[i]) });
    }
    params.set_output("TexCoords", uv_result);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy_s(ntype.ui_name, "ARAP Parameterization");
    strcpy_s(ntype.id_name, "geom_arap");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_arap_exec;
    ntype.declare = node_arap_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_arap