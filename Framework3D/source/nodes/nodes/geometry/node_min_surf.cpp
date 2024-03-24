
#include "Eigen/Core"
#include "Eigen/SparseCholesky"
#include "Eigen/SparseLU"
#include "GCore/Components/MeshOperand.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "iostream"
#include "utils/util_openmesh_bind.h"

namespace USTC_CG::node_min_surf {
static void node_min_surf_declare(NodeDeclarationBuilder& b)
{
    // Input-1: Original 3D mesh with boundary
    b.add_input<decl::Geometry>("Input");
    b.add_input<decl::Float1Buffer>("Weight");
    b.add_output<decl::Geometry>("Output");
}

static void node_min_surf_exec(ExeParams params)
{
    auto input = params.get_input<GOperandBase>("Input");
    if (!input.get_component<MeshComponent>()) {
        throw std::runtime_error("Minimal Surface: Need Geometry Input.");
    }

    auto mesh = operand_to_openmesh(&input);
    auto n = (long long)mesh->n_vertices();
    auto weight = params.get_input<pxr::VtArray<float>>("Weight");

    if (n == 0) {
        throw std::runtime_error("No vertices in the mesh.");
    }
    if (weight.size() != n * n) {
        throw std::runtime_error("size(weight) != n * n");
    }

    std::vector<Eigen::Triplet<double>> coefficients;
    Eigen::VectorXd b_x, b_y, b_z;

    b_x.resize(n);
    b_y.resize(n);
    b_z.resize(n);
    b_x.setZero();
    b_y.setZero();
    b_z.setZero();

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (weight[i * n + j] != 0)
                coefficients.emplace_back(i, j, weight[i * n + j]);

    for (const auto& vertex : mesh->vertices()) {
        auto i = vertex.idx();
        PolyMesh::Point p = mesh->point(vertex);
        if (vertex.is_boundary()) {
            b_x[i] = p[0];
            b_y[i] = p[1];
            b_z[i] = p[2];
            continue;
        }
        else {
            b_x[i] = 0;
            b_y[i] = 0;
            b_z[i] = 0;
        }
    }

    // Generate sparse matrix out of coefficients
    Eigen::SparseMatrix<double> A(n, n);
    A.setZero();
    A.setFromTriplets(coefficients.begin(), coefficients.end());

    // Solve the linear system
    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
    solver.compute(A);
    if (solver.info() != Eigen::Success) {
        throw std::runtime_error("Failed to decompose the matrix.");
    }

    // Solve the linear system
    Eigen::VectorXd x = solver.solve(b_x);
    Eigen::VectorXd y = solver.solve(b_y);
    Eigen::VectorXd z = solver.solve(b_z);

    auto mesh_new = std::make_unique<PolyMesh>(*mesh);
    for (auto vertex : mesh_new->vertices()) {
        auto i = vertex.idx();
        mesh_new->set_point(vertex, PolyMesh::Point(x[i], y[i], z[i]));
    }
    auto operand_base = openmesh_to_operand(mesh_new.get());
    params.set_output("Output", std::move(*operand_base));
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy_s(ntype.ui_name, "Minimal Surface");
    strcpy_s(ntype.id_name, "geom_min_surf");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_min_surf_exec;
    ntype.declare = node_min_surf_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_min_surf
