#include "comp_source_image.h"

#include <algorithm>
#include <list>

namespace USTC_CG
{

CompSourceImage::CompSourceImage(const std::string& label, const std::string& filename) : ImageEditor(label, filename)
{
    if (data_)
        selected_region_ = std::make_shared<Image>(data_->width(), data_->height(), 1);
}

void CompSourceImage::draw()
{
    // Draw the image
    ImageEditor::draw();
    // Draw selected region
    if (flag_enable_selecting_region_)
        select_region();
}

void CompSourceImage::enable_selecting(bool flag)
{
    flag_enable_selecting_region_ = flag;
}

void CompSourceImage::select_region()
{
    /// Invisible button over the canvas to capture mouse interactions.
    ImGui::SetCursorScreenPos(position_);
    ImGui::InvisibleButton(
        label_.c_str(), ImVec2(static_cast<float>(image_width_), static_cast<float>(image_height_)), ImGuiButtonFlags_MouseButtonLeft);
    // Record the current status of the invisible button
    bool is_hovered_ = ImGui::IsItemHovered();
    ImGuiIO& io = ImGui::GetIO();
    if (is_hovered_ && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !draw_status_)
    {
        draw_status_ = true;
        if (region_type_ == RegionType::kPolygon)
        {
            poly_points_ = {};
            start_ = ImVec2(
                std::clamp<float>(io.MousePos.x - position_.x, 0, (float)image_width_),
                std::clamp<float>(io.MousePos.y - position_.y, 0, (float)image_height_));
            poly_points_.push_back(start_);
        }
        else
        {
            start_ = end_ = ImVec2(
                std::clamp<float>(io.MousePos.x - position_.x, 0, (float)image_width_),
                std::clamp<float>(io.MousePos.y - position_.y, 0, (float)image_height_));
        }
    }
    if (draw_status_)
    {
        if (region_type_ == RegionType::kPolygon)
        {
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                poly_points_.pop_back();
            }
            poly_points_.push_back(ImVec2(
                std::clamp<float>(io.MousePos.x - position_.x, 0, (float)image_width_),
                std::clamp<float>(io.MousePos.y - position_.y, 0, (float)image_height_)));
        }
        else
        {
            end_ = ImVec2(
                std::clamp<float>(io.MousePos.x - position_.x, 0, (float)image_width_),
                std::clamp<float>(io.MousePos.y - position_.y, 0, (float)image_height_));
        }
        if ((!ImGui::IsMouseDown(ImGuiMouseButton_Left) && region_type_ != RegionType::kPolygon) ||
            (ImGui::IsMouseDown(ImGuiMouseButton_Right) && region_type_ == RegionType::kPolygon))
        {
            draw_status_ = false;
            for (int i = 0; i < selected_region_->width(); ++i)
                for (int j = 0; j < selected_region_->height(); ++j)
                    selected_region_->set_pixel(i, j, { 0 });
            switch (region_type_)
            {
                case CompSourceImage::RegionType::kDefault:
                {
                    break;
                }
                case CompSourceImage::RegionType::kRect:
                {
                    for (int i = static_cast<int>(start_.x); i < static_cast<int>(end_.x); ++i)
                    {
                        for (int j = static_cast<int>(start_.y); j < static_cast<int>(end_.y); ++j)
                        {
                            selected_region_->set_pixel(i, j, { 255 });
                        }
                    }
                    break;
                }
                case CompSourceImage::RegionType::kPolygon:
                {
                    for (int i = 0; i < selected_region_->width(); ++i)
                    {
                        std::vector<int> intersections;
                        for (int j = 0; j < poly_points_.size() - 1; j++)
                        {
                            ImVec2 p1 = poly_points_[j];
                            ImVec2 p2 = poly_points_[j + 1];
                            if ((p1.y > i && p2.y < i) || (p1.y < i && p2.y > i))
                            {
                                float x = p1.x + (i - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
                                intersections.push_back(static_cast<int>(x));
                            }
                        }
                        // last -> 0:
                        ImVec2 p1 = poly_points_[poly_points_.size() - 1];
                        ImVec2 p2 = poly_points_[0];
                        if ((p1.y > i && p2.y < i) || (p1.y < i && p2.y > i))
                        {
                            float x = p1.x + (i - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
                            intersections.push_back(static_cast<int>(x));
                        }

                        std::sort(intersections.begin(), intersections.end());
                        for (int j = 0; j < intersections.size(); j += 2)
                        {
                            for (int k = intersections[j]; k < intersections[j + 1]; k++)
                            {
                                selected_region_->set_pixel(k, i, { 255 });
                            }
                        }
                    }
                }
                default: break;
            }

            Eigen::MatrixXi mask_eigen = selected_region_->as_eigen_mask();
            poisson_ = std::make_shared<Poisson>();
            poisson_->set_mask(mask_eigen);
            poisson_->PoissonInit();
        }
    }

    // Visualization
    auto draw_list = ImGui::GetWindowDrawList();
    ImVec2 s(start_.x + position_.x, start_.y + position_.y);
    ImVec2 e(end_.x + position_.x, end_.y + position_.y);

    switch (region_type_)
    {
        case CompSourceImage::RegionType::kDefault:
        {
            break;
        }
        case CompSourceImage::RegionType::kRect:
        {
            if (e.x > s.x && e.y > s.y)
                draw_list->AddRect(s, e, IM_COL32(255, 0, 0, 255), 2.0f);
            break;
        }
        case CompSourceImage::RegionType::kPolygon:
        {
            if (poly_points_.size() > 1)
            {
                for (int i = 0; i < poly_points_.size() - 1; i++)
                {
                    draw_list->AddLine(
                        ImVec2(poly_points_[i].x + position_.x, poly_points_[i].y + position_.y),
                        ImVec2(poly_points_[i + 1].x + position_.x, poly_points_[i + 1].y + position_.y),
                        IM_COL32(255, 0, 0, 255),
                        2.0f);
                }
                draw_list->AddLine(
                    ImVec2(poly_points_[poly_points_.size() - 1].x + position_.x, poly_points_[poly_points_.size() - 1].y + position_.y),
                    ImVec2(poly_points_[0].x + position_.x, poly_points_[0].y + position_.y),
                    IM_COL32(255, 0, 0, 255),
                    2.0f);
            }
        }
        default: break;
    }
}
std::shared_ptr<Image> CompSourceImage::get_region()
{
    return selected_region_;
}
std::shared_ptr<Image> CompSourceImage::get_data()
{
    return data_;
}
ImVec2 CompSourceImage::get_position() const
{
    return start_;
}
std::shared_ptr<Poisson> CompSourceImage::get_poisson() const
{
    return poisson_;
}
}  // namespace USTC_CG