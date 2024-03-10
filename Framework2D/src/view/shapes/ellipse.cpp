#include "view/shapes/ellipse.h"

#include <imgui.h>

#include <cmath>
#include "iostream"

namespace USTC_CG
{
void Ellipse::draw(const Config& config) const
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddEllipse(
        config.bias + (start_point_ + end_point_) / 2,  // center
        std::abs(end_point_.x - start_point_.x) / 2,
        std::abs(end_point_.y - start_point_.y) / 2,
        config.line_color,
        0,
        0,
        config.line_thickness);

    if (config.is_current_drawing)
    {
        draw_list->AddRect(
            config.bias + start_point_,
            config.bias + end_point_,
            opacity(config.line_color, 0.5),
            0.f,
            ImDrawFlags_None,
            config.line_thickness);
    }
}

void Ellipse::update(ImVec2 point)
{
    // if user is holding shift:
    if (ImGui::GetIO().KeyShift)
    {
        auto radius = std::min(
            std::abs(point.x - start_point_.x),
            std::abs(point.y - start_point_.y));
        this->end_point_ = ImVec2(
            (point.x > start_point_.x) ? start_point_.x + radius
                                      : start_point_.x - radius,
            (point.y > start_point_.y) ? start_point_.y + radius
                                      : start_point_.y - radius);

        return;
    }

    this->end_point_ = point;
}
}  // namespace USTC_CG