#include "view/shapes/freehand.h"

#include <imgui.h>

namespace USTC_CG
{
// Draw the line using ImGui
void Freehand::draw(const Config& config) const
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    for (size_t i = 0; i < points_.size() - 1; i++)
    {
        draw_list->AddLine(
            points_[i] + config.bias,
            points_[i + 1] + config.bias,
            config.line_color,
            config.line_thickness);
    }
}

void Freehand::update(ImVec2 point)
{
    points_.push_back(point);
}
}  // namespace USTC_CG