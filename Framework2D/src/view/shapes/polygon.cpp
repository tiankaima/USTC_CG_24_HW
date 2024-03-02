#include "view/shapes/polygon.h"

#include <imgui.h>

namespace USTC_CG
{
void Polygon::draw(const Config& config) const
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    for (size_t i = 0; i < this->points_.size() - 1; i++)
    {
        draw_list->AddLine(
            this->points_[i] + config.bias,
            this->points_[i + 1] + config.bias,
            config.line_color,
            config.line_thickness);
    }

    if (this->points_.size() > 2)
    {
        draw_list->AddLine(
            this->points_.back() + config.bias,
            this->points_.front() + config.bias,
            config.line_color,
            config.line_thickness);
    }
}

void Polygon::addPoint(ImVec2 point)
{
    this->points_.push_back(point);
}

void Polygon::update(ImVec2 point)
{
    if (this->points_.empty() || this->points_.size() == 1)
    {
        this->points_.push_back(point);
        return;
    }
    this->points_.pop_back();
    this->points_.push_back(point);
}
}  // namespace USTC_CG