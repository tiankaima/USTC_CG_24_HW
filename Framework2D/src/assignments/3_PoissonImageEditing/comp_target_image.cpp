#include "comp_target_image.h"

#include <cmath>
#include <iostream>
#include <utility>

namespace USTC_CG
{

CompTargetImage::CompTargetImage(const std::string& label, const std::string& filename) : ImageEditor(label, filename)
{
    if (data_)
        back_up_ = std::make_shared<Image>(*data_);
}

void CompTargetImage::draw()
{
    // Draw the image
    ImageEditor::draw();
    // Invisible button for interactions
    ImGui::SetCursorScreenPos(position_);
    ImGui::InvisibleButton(
        label_.c_str(), ImVec2(static_cast<float>(image_width_), static_cast<float>(image_height_)), ImGuiButtonFlags_MouseButtonLeft);
    bool is_hovered_ = ImGui::IsItemHovered();
    // When the mouse is clicked or moving, we would adapt clone function to
    // copy the selected region to the target.
    ImGuiIO& io = ImGui::GetIO();
    if (is_hovered_ && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        edit_status_ = true;
        mouse_position_ = ImVec2(io.MousePos.x - position_.x, io.MousePos.y - position_.y);
        clone();
    }
    if (edit_status_)
    {
        mouse_position_ = ImVec2(io.MousePos.x - position_.x, io.MousePos.y - position_.y);
        if (flag_realtime_updating)
            clone();
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            edit_status_ = false;
        }
    }
}

void CompTargetImage::set_source(std::shared_ptr<CompSourceImage> source)
{
    source_image_ = std::move(source);
}

void CompTargetImage::set_realtime(bool flag)
{
    flag_realtime_updating = flag;
}

void CompTargetImage::restore()
{
    *data_ = *back_up_;
    update();
}

void CompTargetImage::set_paste()
{
    clone_type_ = CloneType::kPaste;
}

void CompTargetImage::set_seamless()
{
    clone_type_ = CloneType::kSeamless;
}

void CompTargetImage::clone()
{
    // The implementation of different types of cloning
    // HW3_TODO: In this function, you should at least implement the "seamless"
    // cloning labeled by `clone_type_ ==kSeamless`.
    //
    // The realtime updating (update when the mouse is moving) is only available
    // when the checkbox is selected. It is required to improve the efficiency
    // of your seamless cloning to achieve realtime editing. (Use decomposition
    // of sparse matrix before solve the linear system)
    if (data_ == nullptr || source_image_ == nullptr || source_image_->get_region() == nullptr)
        return;
    std::shared_ptr<Image> mask = source_image_->get_region();

    switch (clone_type_)
    {
        case CompTargetImage::CloneType::kDefault: break;
        case CompTargetImage::CloneType::kPaste:
        {
            restore();
            source_image_->get_poisson()->CopyPaste(
                { (int)(mouse_position_.x - source_image_->get_position().x), (int)(mouse_position_.y - source_image_->get_position().y) },
                { 0, 0 },
                *data_,
                *source_image_->get_data());
            break;
        }
        case CompTargetImage::CloneType::kSeamless:
        {
            restore();
            source_image_->get_poisson()->GetPoisson(
                { (int)(mouse_position_.x - source_image_->get_position().x), (int)(mouse_position_.y - source_image_->get_position().y) },
                { 0, 0 },
                *data_,
                *source_image_->get_data());
            break;
        }
        default:
        {
            break;
        }
    }

    update();
}
}  // namespace USTC_CG