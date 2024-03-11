#include "view/shapes/line.h"

#include <imgui.h>

namespace USTC_CG
{
// Draw the line using ImGui
void Line::draw(const Config& config) const
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddLine(
        config.bias + start_point_,
        config.bias + end_point_,
        config.line_color,
        config.line_thickness);
}

void Line::update(ImVec2 point)
{
    end_point_ = point;
}
}  // namespace USTC_CG