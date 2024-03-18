#include "view/shapes/polygon.h"

#include <imgui.h>

namespace USTC_CG
{
void Polygon::draw(const Config& config) const
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    for (size_t i = 0; i < points_.size() - 1; i++)
    {
        draw_list->AddLine(points_[i] + config.bias, points_[i + 1] + config.bias, config.line_color, config.line_thickness);
    }

    if (points_.size() > 2)
    {
        draw_list->AddLine(points_.back() + config.bias, points_.front() + config.bias, config.line_color, config.line_thickness);
    }
}

void Polygon::addPoint(ImVec2 point)
{
    points_.push_back(point);
}

void Polygon::update(ImVec2 point)
{
    if (points_.empty() || points_.size() == 1)
    {
        points_.push_back(point);
        return;
    }
    points_.pop_back();
    points_.push_back(point);
}
}  // namespace USTC_CG