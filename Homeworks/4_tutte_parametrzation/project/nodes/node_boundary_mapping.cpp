#include <Eigen/Core>

#include "GCore/Components/MeshOperand.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "utils/util_openmesh_bind.h"

namespace USTC_CG::node_boundary_mapping {

auto find_boundary_loop(PolyMesh* mesh)
{
    OpenMesh::SmartVertexHandle start_vertex;
    for (auto vertex : mesh->vertices()) {
        if (vertex.is_boundary()) {
            start_vertex = vertex;
            break;
        }
    }

    std::vector<PolyMesh::VertexHandle> boundary_loop;
    auto current_vertex = start_vertex;
    do {
        boundary_loop.push_back(current_vertex);
        for (auto vertex : current_vertex.vertices()) {
            if (vertex.is_boundary()) {
                current_vertex = vertex;
                break;
            }
        }
    } while (current_vertex != start_vertex);

    return boundary_loop;
}

std::vector<double> calculate_boundary_lengths(
    PolyMesh* mesh,
    const std::vector<PolyMesh::VertexHandle>& boundary_loop)
{
    std::vector<double> lengths;
    for (size_t i = 0; i < boundary_loop.size(); i++) {
        auto next_vertex = boundary_loop[(i + 1) % boundary_loop.size()];
        lengths.push_back((mesh->point(next_vertex) - mesh->point(boundary_loop[i])).norm());
    }
    return lengths;
}

double calculate_boundary_length(
    PolyMesh* mesh,
    const std::vector<PolyMesh::VertexHandle>& boundary_loop)
{
    double length = 0;
    for (size_t i = 0; i < boundary_loop.size(); i++) {
        auto next_vertex = boundary_loop[(i + 1) % boundary_loop.size()];
        length += (mesh->point(next_vertex) - mesh->point(boundary_loop[i])).norm();
    }
    return length;
}

std::vector<double> calculate_boundary_cumulative_lengths_normalized(
    PolyMesh* mesh,
    const std::vector<PolyMesh::VertexHandle>& boundary_loop)
{
    auto lengths = calculate_boundary_lengths(mesh, boundary_loop);
    auto total_length = calculate_boundary_length(mesh, boundary_loop);
    std::vector<double> cumulative_lengths;
    cumulative_lengths.push_back(0);
    for (size_t i = 0; i < boundary_loop.size(); i++) {
        cumulative_lengths.push_back(cumulative_lengths.back() + lengths[i] / total_length);
    }
    return cumulative_lengths;
}

std::vector<PolyMesh::Point> map_to_circle(const std::vector<double>& cumulative_lengths)
{
    // map to a unit circle on x-y plane
    std::vector<PolyMesh::Point> positions;
    for (double cumulative_length : cumulative_lengths) {
        auto angle = 2 * M_PI * cumulative_length;
        positions.emplace_back(std::cos(angle), std::sin(angle), 0);
    }
    return positions;
}

std::vector<PolyMesh::Point> map_to_square(const std::vector<double>& cumulative_lengths)
{
    // map to a unit square on x-y plane
    std::vector<PolyMesh::Point> positions;
    for (double cumulative_length : cumulative_lengths) {
        double x, y;
        if (cumulative_length < 0.25) {
            x = 1.0;
            y = 4.0 * cumulative_length;
        }
        else if (cumulative_length < 0.5) {
            x = 1.0 - 4.0 * (cumulative_length - 0.25);
            y = 1.0;
        }
        else if (cumulative_length < 0.75) {
            x = 0.0;
            y = 1.0 - 4.0 * (cumulative_length - 0.5);
        }
        else {
            x = 4.0 * (cumulative_length - 0.75);
            y = 0.0;
        }
        positions.emplace_back(x, y, 0);
    }
    return positions;
}

static void node_map_boundary_to_circle_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Geometry>("Input");
    b.add_output<decl::Geometry>("Output");
}

static void node_map_boundary_to_circle_exec(ExeParams params)
{
    auto input = params.get_input<GOperandBase>("Input");
    if (!input.get_component<MeshComponent>()) {
        throw std::runtime_error("Boundary Mapping: Need Geometry Input.");
    }

    auto mesh = operand_to_openmesh(&input);
    auto n = (long long)mesh->n_vertices();

    if (n == 0) {
        throw std::runtime_error("No vertices in the mesh.");
    }

    auto b = find_boundary_loop(mesh.get());
    auto cumulative_lengths_n = calculate_boundary_cumulative_lengths_normalized(mesh.get(), b);
    auto positions = map_to_circle(cumulative_lengths_n);

    auto mesh_new = std::make_unique<PolyMesh>(*mesh);

    for (size_t i = 0; i < b.size(); i++) {
        mesh_new->set_point(b[i], positions[i]);
    }

    auto operand_base = openmesh_to_operand(mesh_new.get());
    params.set_output("Output", std::move(*operand_base));
}

static void node_map_boundary_to_square_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Geometry>("Input");
    b.add_output<decl::Geometry>("Output");
}

static void node_map_boundary_to_square_exec(ExeParams params)
{
    auto input = params.get_input<GOperandBase>("Input");
    if (!input.get_component<MeshComponent>()) {
        throw std::runtime_error("Input does not contain a mesh");
    }

    auto mesh = operand_to_openmesh(&input);
    auto n = (long long)mesh->n_vertices();

    if (n == 0) {
        throw std::runtime_error("No vertices in the mesh.");
    }

    auto b = find_boundary_loop(mesh.get());
    auto cumulative_lengths_n = calculate_boundary_cumulative_lengths_normalized(mesh.get(), b);
    auto positions = map_to_square(cumulative_lengths_n);

    auto mesh_new = std::make_unique<PolyMesh>(*mesh);

    for (size_t i = 0; i < b.size(); i++) {
        mesh_new->set_point(b[i], positions[i]);
    }

    auto operand_base = openmesh_to_operand(mesh_new.get());
    params.set_output("Output", std::move(*operand_base));
}

static void node_register()
{
    static NodeTypeInfo ntype_square, ntype_circle;

    strcpy_s(ntype_square.ui_name, "Map Boundary to Square");
    strcpy_s(ntype_square.id_name, "geom_map_boundary_to_square");

    geo_node_type_base(&ntype_square);
    ntype_square.node_execute = node_map_boundary_to_square_exec;
    ntype_square.declare = node_map_boundary_to_square_declare;
    nodeRegisterType(&ntype_square);

    strcpy_s(ntype_circle.ui_name, "Map Boundary to Circle");
    strcpy_s(ntype_circle.id_name, "geom_map_boundary_to_circle");

    geo_node_type_base(&ntype_circle);
    ntype_circle.node_execute = node_map_boundary_to_circle_exec;
    ntype_circle.declare = node_map_boundary_to_circle_declare;
    nodeRegisterType(&ntype_circle);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_boundary_mapping
