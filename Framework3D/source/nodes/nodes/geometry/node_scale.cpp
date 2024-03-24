#include "GCore/Components/MeshOperand.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "utils/util_openmesh_bind.h"

namespace USTC_CG::node_scale {
static void node_scale_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Geometry>("Input");
    b.add_input<decl::String>("x");
    b.add_output<decl::Geometry>("Output");
}

static void node_scale_exec(ExeParams params)
{
    auto input = params.get_input<GOperandBase>("Input");

    if (!input.get_component<MeshComponent>()) {
        throw std::runtime_error("Scale: need geometry input.");
    }

    auto scale = params.get_input<std::string>("x");
    double scale_value;
    try {
        scale_value = std::stof(scale);
    }
    catch (...) {
        throw std::runtime_error("Scale: invalid scale value");
    }

    auto mesh = operand_to_openmesh(&input);
    auto mesh_new = std::make_unique<PolyMesh>(*mesh);

    for (auto v : mesh_new->vertices()) {
        mesh_new->point(v) = mesh_new->point(v) * scale_value;
    }

    auto operand_base = openmesh_to_operand(mesh_new.get());
    params.set_output("Output", std::move(*operand_base));
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy_s(ntype.ui_name, "Scale");
    strcpy_s(ntype.id_name, "geom_scale");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_scale_exec;
    ntype.declare = node_scale_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_scale
