#include "Eigen/Core"
#include "GCore/Components/MeshOperand.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "utils/util_openmesh_bind.h"

namespace USTC_CG::node_mesh_weight_wls {
static void node_mesh_weight_wls_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Geometry>("Input");
    b.add_output<decl::Float1Buffer>("Output");
}

static void node_mesh_weight_wls_exec(ExeParams params)
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
        for (const auto& vertex2 : vertex.vertices()) {
            auto edge = mesh->point(vertex2) - mesh->point(vertex);
            sum += edge.norm();
            result[vertex.idx() * n + vertex2.idx()] = -edge.norm();
        }
        result[vertex.idx() * n + vertex.idx()] = sum;
    }

    params.set_output("Output", result);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy_s(ntype.ui_name, "Mesh Weight Least Squares");
    strcpy_s(ntype.id_name, "geom_mesh_weight_wls");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_mesh_weight_wls_exec;
    ntype.declare = node_mesh_weight_wls_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_mesh_weight_wls