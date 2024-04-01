#include "GCore/Components/MeshOperand.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "iostream"
#include "utils/util_openmesh_bind.h"

namespace USTC_CG::node_mesh_weight_cotangent {
static void node_mesh_weight_cotangent_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Geometry>("Input");
    b.add_output<decl::Float1Buffer>("Output");
}

static void node_mesh_weight_cotangent_exec(ExeParams params)
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

        auto sum = 0.0f;
        for (const auto& half_edge : vertex.outgoing_halfedges()) {
            auto vertex2 = half_edge.to();
            auto vertex3 = half_edge.prev().opp().to();
            auto vertex4 = half_edge.next().to();

            auto edge1 = mesh->point(vertex2) - mesh->point(vertex3);
            auto edge2 = mesh->point(vertex) - mesh->point(vertex3);
            auto edge3 = mesh->point(vertex2) - mesh->point(vertex4);
            auto edge4 = mesh->point(vertex) - mesh->point(vertex4);

            // cot = cos() / sin() => dot product / cross product
            auto cot1 = edge1.dot(edge2) / (edge1.cross(edge2).norm());
            auto cot2 = edge3.dot(edge4) / (edge3.cross(edge4).norm());

            cot1 = abs(cot1);
            cot2 = abs(cot2);

            sum += cot1 + cot2;
            result[vertex.idx() * n + vertex2.idx()] = -cot1 - cot2;
        }
        result[vertex.idx() * n + vertex.idx()] = sum;
    }

    params.set_output("Output", result);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy_s(ntype.ui_name, "Mesh Weight Cotangent");
    strcpy_s(ntype.id_name, "geom_mesh_weight_cotangent");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_mesh_weight_cotangent_exec;
    ntype.declare = node_mesh_weight_cotangent_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_mesh_weight_cotangent