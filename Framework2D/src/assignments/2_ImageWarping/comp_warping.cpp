#include "comp_warping.h"

#include <annoylib.h>
#include <kissrandom.h>

#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "view/warps/warp.h"

namespace USTC_CG
{

CompWarping::CompWarping(const std::string& label, const std::string& filename) : ImageEditor(label, filename)
{
    if (data_)
        back_up_ = std::make_shared<Image>(*data_);
}

void CompWarping::draw()
{
    // Draw the image
    ImageEditor::draw();
    // Draw the canvas
    if (flag_enable_selecting_points_)
        select_points();
}

void CompWarping::invert()
{
    for (int i = 0; i < data_->width(); ++i)
    {
        for (int j = 0; j < data_->height(); ++j)
        {
            const auto color = data_->get_pixel(i, j);
            data_->set_pixel(
                i,
                j,
                { static_cast<unsigned char>(255 - color[0]),
                  static_cast<unsigned char>(255 - color[1]),
                  static_cast<unsigned char>(255 - color[2]) });
        }
    }
    // After change the image, we should reload the image data to the renderer
    update();
}
void CompWarping::mirror(bool is_horizontal, bool is_vertical)
{
    Image image_tmp(*data_);
    int width = data_->width();
    int height = data_->height();

    if (is_horizontal && is_vertical)
        for (int i = 0; i < width; ++i)
            for (int j = 0; j < height; ++j)
                data_->set_pixel(i, j, image_tmp.get_pixel(width - 1 - i, height - 1 - j));

    if (is_horizontal && !is_vertical)
        for (int i = 0; i < width; ++i)
            for (int j = 0; j < height; ++j)
                data_->set_pixel(i, j, image_tmp.get_pixel(width - 1 - i, j));

    if (!is_horizontal && is_vertical)
        for (int i = 0; i < width; ++i)
            for (int j = 0; j < height; ++j)
                data_->set_pixel(i, j, image_tmp.get_pixel(i, height - 1 - j));

    // After change the image, we should reload the image data to the renderer
    update();
}
void CompWarping::gray_scale()
{
    for (int i = 0; i < data_->width(); ++i)
    {
        for (int j = 0; j < data_->height(); ++j)
        {
            const auto color = data_->get_pixel(i, j);
            unsigned char gray_value = (color[0] + color[1] + color[2]) / 3;
            data_->set_pixel(i, j, { gray_value, gray_value, gray_value });
        }
    }
    // After change the image, we should reload the image data to the renderer
    update();
}
void CompWarping::warping(WarpType type)
{
    if (start_points_.empty())
        return;

    std::shared_ptr<Warp> warp = create_warp(type, ImVec2((float)data_->width(), (float)data_->height()), start_points_, end_points_);
    warp->warmup();

    // Create a new image, init as black
    Image warped_image(*data_);
    for (int y = 0; y < data_->height(); ++y)
        for (int x = 0; x < data_->width(); ++x)
            warped_image.set_pixel(x, y, { 0, 0, 0 });

    // Mask to store which pixel has been filled
    std::vector<std::vector<unsigned char>> mask(
        data_->width(),
        std::vector<unsigned char>(
            data_->height(),
            0));  // using unsigned char since vector<bool> is bullshit

    // Ann index
    Annoy::AnnoyIndex<int, float, Annoy::Euclidean, Annoy::Kiss64Random, Annoy::AnnoyIndexSingleThreadedBuildPolicy> ann(2);
    int ann_counts = 0;

    for (int y = 0; y < data_->height(); ++y)
        for (int x = 0; x < data_->width(); ++x)
        {
            auto p = warp->warp(Eigen::Vector2f((float)x, (float)y));
            auto new_x = static_cast<int>(p(0));
            auto new_y = static_cast<int>(p(1));

            // Copy the color from the original image to the result image
            if (new_x >= 0 && new_x < data_->width() && new_y >= 0 && new_y < data_->height())
            {
                std::vector<unsigned char> pixel = data_->get_pixel(x, y);
                warped_image.set_pixel(new_x, new_y, pixel);

                mask[new_x][new_y] = 1;

                auto vec = new float[2];
                vec[0] = (float)new_x;
                vec[1] = (float)new_y;
                ann.add_item(ann_counts++, vec);
            }
        }

    ann.build(2);
    for (int y = 0; y < data_->height(); ++y)
        for (int x = 0; x < data_->width(); ++x)
            if (!mask[x][y])
            {
                auto vec = new float[2];
                vec[0] = (float)x;
                vec[1] = (float)y;
                std::vector<int> indices;
                ann.get_nns_by_vector(vec, 1, 1, &indices, nullptr);

                auto pos = new float[2];
                ann.get_item(indices[0], pos);
                std::vector<unsigned char> pixel = warped_image.get_pixel((int)pos[0], (int)pos[1]);
                warped_image.set_pixel(x, y, pixel);
            }

    *data_ = std::move(warped_image);
    update();
}
void CompWarping::restore()
{
    *data_ = *back_up_;
    update();
}
void CompWarping::enable_selecting(bool flag)
{
    flag_enable_selecting_points_ = flag;
}
void CompWarping::select_points()
{
    /// Invisible button over the canvas to capture mouse interactions.
    ImGui::SetCursorScreenPos(position_);
    ImGui::InvisibleButton(
        label_.c_str(), ImVec2(static_cast<float>(image_width_), static_cast<float>(image_height_)), ImGuiButtonFlags_MouseButtonLeft);
    // Record the current status of the invisible button
    bool is_hovered_ = ImGui::IsItemHovered();
    // Selections
    ImGuiIO& io = ImGui::GetIO();
    if (is_hovered_ && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        draw_status_ = true;
        start_ = end_ = ImVec2(io.MousePos.x - position_.x, io.MousePos.y - position_.y);
    }
    if (draw_status_)
    {
        end_ = ImVec2(io.MousePos.x - position_.x, io.MousePos.y - position_.y);
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            start_points_.push_back(start_);
            end_points_.push_back(end_);
            draw_status_ = false;
        }
    }
    // Visualization
    auto draw_list = ImGui::GetWindowDrawList();
    for (size_t i = 0; i < start_points_.size(); ++i)
    {
        ImVec2 s(start_points_[i].x + position_.x, start_points_[i].y + position_.y);
        ImVec2 e(end_points_[i].x + position_.x, end_points_[i].y + position_.y);
        draw_list->AddLine(s, e, IM_COL32(255, 0, 0, 255), 2.0f);
        draw_list->AddCircleFilled(s, 4.0f, IM_COL32(0, 0, 255, 255));
        draw_list->AddCircleFilled(e, 4.0f, IM_COL32(0, 255, 0, 255));
    }
    if (draw_status_)
    {
        ImVec2 s(start_.x + position_.x, start_.y + position_.y);
        ImVec2 e(end_.x + position_.x, end_.y + position_.y);
        draw_list->AddLine(s, e, IM_COL32(255, 0, 0, 255), 2.0f);
        draw_list->AddCircleFilled(s, 4.0f, IM_COL32(0, 0, 255, 255));
    }
}
void CompWarping::init_selections()
{
    start_points_.clear();
    end_points_.clear();
}
}  // namespace USTC_CG