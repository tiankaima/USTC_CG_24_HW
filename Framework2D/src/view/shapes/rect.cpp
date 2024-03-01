#include "view/shapes/rect.h"

#include <imgui.h>

namespace USTC_CG
{
// Draw the rectangle using ImGui
void Rect::draw(const Config& config) const
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRect(
        config.bias + start_point_,
        config.bias + end_point_,
        config.line_color,
        0.f,  // No rounding of corners
        ImDrawFlags_None,
        config.line_thickness);
}

void Rect::update(ImVec2 point)
{
    if (ImGui::GetIO().KeyShift)
    {
        auto width = std::abs(point.x - start_point_.x);
        auto height = std::abs(point.y - start_point_.y);
        auto min = std::min(width, height);
        this->end_point_ = ImVec2(
            (point.x > start_point_.x) ? start_point_.x + min
                                      : start_point_.x - min,
            (point.y > start_point_.y) ? start_point_.y + min
                                      : start_point_.y - min);
        return;
    }

    this->end_point_ = point;
}

}  // namespace USTC_CG
