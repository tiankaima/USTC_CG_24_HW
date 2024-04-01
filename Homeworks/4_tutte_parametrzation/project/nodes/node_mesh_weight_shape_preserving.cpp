#include "Eigen/Core"
#include "GCore/Components/MeshOperand.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "utils/util_openmesh_bind.h"

namespace USTC_CG::node_mesh_weight_shape_preserving {
static void node_mesh_weight_shape_preserving_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Geometry>("Input");
    b.add_output<decl::Float1Buffer>("Output");
}

float tri_area(auto a, auto b, auto c)
{
    // three Eigen::Vector2f points on plane. calc the area of the triangle
    auto ab = b - a;
    auto ac = c - a;
    return 0.5f * std::abs(ab.x() * ac.y() - ab.y() * ac.x());
}

static void node_mesh_weight_shape_preserving_exec(ExeParams params)
{
    auto input = params.get_input<GOperandBase>("Input");

    if (!input.get_component<MeshComponent>()) {
        throw std::runtime_error("Mesh Weight Uniform: need geometry input.");
    }

    auto mesh = operand_to_openmesh(&input);
    auto n = mesh->n_vertices();

    if (n == 0) {
        throw std::runtime_error("No vertices in the mesh.");
    }

    pxr::VtArray<float> result(n * n);

    for (const auto& vertex : mesh->vertices()) {
        if (vertex.is_boundary()) {
            result[vertex.idx() * n + vertex.idx()] = 1.0f;
            continue;
        }

        std::vector<float> angles, lengths, id_list;
        auto angles_sum = 0.0f;
        for (const auto& half_edge : vertex.outgoing_halfedges()) {
            auto next_edge = half_edge.next();
            auto edge_1 = mesh->point(next_edge.to()) - mesh->point(vertex);
            auto edge_2 = mesh->point(half_edge.to()) - mesh->point(vertex);

            auto cos_angle = edge_1.dot(edge_2) / (edge_1.norm() * edge_2.norm());
            auto angle = acos(cos_angle);
            angles.push_back(angle);
            angles_sum += angle;

            lengths.push_back(edge_2.norm());
            id_list.push_back(half_edge.to().idx());
        }

        std::vector<Eigen::Vector2f> neighbors;
        auto theta = 0.0f;
        for (auto i = 0; i < angles.size(); i++) {
            neighbors.push_back(Eigen::Vector2f(lengths[i] * cos(theta), lengths[i] * sin(theta)));
            theta += 2 * M_PI * angles[i] / angles_sum;
        }

        Eigen::Vector2f p(0.0f, 0.0f);

        for (auto i = 0; i < angles.size(); i++) {
            auto p1 = neighbors[i];
            for (auto j = 0; j < angles.size(); j++) {
                auto next = (j + 1) % angles.size();
                if (j != i && next != i) {
                    auto p2 = neighbors[j];
                    auto p3 = neighbors[next];

                    auto S = tri_area(p1, p2, p3);
                    auto sigma1 = tri_area(p, p2, p3) / S;
                    auto sigma2 = tri_area(p1, p, p3) / S;
                    auto sigma3 = tri_area(p1, p2, p) / S;

                    if (sigma1 + sigma2 + sigma3 < 1.001) {
                        result[vertex.idx() * n + vertex.idx()] += 1.0f;
                        result[vertex.idx() * n + id_list[i]] += -sigma1;
                        result[vertex.idx() * n + id_list[j]] += -sigma2;
                        result[vertex.idx() * n + id_list[next]] += -sigma3;
                    }
                }
            }
        }
    }

    params.set_output("Output", result);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy_s(ntype.ui_name, "Mesh Weight Shape Preserving");
    strcpy_s(ntype.id_name, "geom_mesh_weight_shape_preserving");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_mesh_weight_shape_preserving_exec;
    ntype.declare = node_mesh_weight_shape_preserving_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_mesh_weight_shape_preserving