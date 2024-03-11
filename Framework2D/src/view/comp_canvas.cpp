#include "view/comp_canvas.h"

#include <cmath>
#include <iostream>

#include "imgui.h"

namespace USTC_CG
{
void Canvas::draw()
{
    draw_background();

    if (is_hovered_ && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        mouse_click_event();
    }
    mouse_move_event();
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        mouse_release_event();
    }

    // ctrl z:
    if (ImGui::GetIO().KeyCtrl &&
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
    {
        undo();
    }
    // ctrl y:
    if (ImGui::GetIO().KeyCtrl &&
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Y)))
    {
        redo();
    }

    // enter:
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
    {
        if (draw_status_)
        {
            finish_drawing();
        }
    }

    draw_shapes();
}

void Canvas::set_type(ShapeType type)
{
    draw_status_ = false;
    shape_type_ = type;
}

void Canvas::undo()
{
    if (history_index == -1)
    {
        return;
    }
    if (history_actions[history_index].action == Action::kDraw)
    {
        shape_list_.pop_back();
    }
    else if (history_actions[history_index].action == Action::kDelete)
    {
        shape_list_.push_back(history_actions[history_index].shape);
    }
    history_index--;
}

void Canvas::redo()
{
    if (history_index == history_actions.size() - 1)
    {
        return;
    }
    history_index++;
    if (history_actions[history_index].action == Action::kDraw)
    {
        shape_list_.push_back(history_actions[history_index].shape);
    }
    else if (history_actions[history_index].action == Action::kDelete)
    {
        // shape_list_.pop_back();
        shape_list_.erase(
            shape_list_.begin() +
            static_cast<int>(history_actions[history_index].delete_index));
    }
}

void Canvas::clear()
{
    if (history_index != -1 && history_index != history_actions.size() - 1)
    {
        history_actions.erase(
            history_actions.begin() + static_cast<int>(history_index) + 1,
            history_actions.end());
    }
    for (auto index = shape_list_.size(); index > 0; index--)
    {
        history_actions.push_back(
            { Action::kDelete, shape_list_[index - 1], index - 1 });
        history_index++;
    }

    shape_list_.clear();
}

void Canvas::set_attributes(const ImVec2& min, const ImVec2& size)
{
    canvas_min_ = min;
    canvas_size_ = size;
    canvas_minimal_size_ = size;
    canvas_max_ =
        ImVec2(canvas_min_.x + canvas_size_.x, canvas_min_.y + canvas_size_.y);
}

void Canvas::show_background(bool flag)
{
    show_background_ = flag;
}

void Canvas::draw_background()
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    if (show_background_)
    {
        // Draw background recrangle
        draw_list->AddRectFilled(canvas_min_, canvas_max_, background_color_);
        // Draw background border
        draw_list->AddRect(canvas_min_, canvas_max_, border_color_);
    }
    /// Invisible button over the canvas to capture mouse interactions.
    ImGui::SetCursorScreenPos(canvas_min_);
    ImGui::InvisibleButton(
        label_.c_str(), canvas_size_, ImGuiButtonFlags_MouseButtonLeft);
    // Record the current status of the invisible button
    is_hovered_ = ImGui::IsItemHovered();
    is_active_ = ImGui::IsItemActive();
}

void Canvas::draw_shapes() const
{
    Shape::Config s = { .bias = { canvas_min_.x, canvas_min_.y } };
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // ClipRect can hide the drawing content outside of the rectangular area
    draw_list->PushClipRect(canvas_min_, canvas_max_, true);
    for (const auto& shape : shape_list_)
    {
        shape->draw(s);
    }
    if (draw_status_ && current_shape_)
    {
        s.is_current_drawing = true;
        current_shape_->draw(s);
    }
    draw_list->PopClipRect();
}

void Canvas::finish_drawing()
{
    draw_status_ = false;
    if (current_shape_)
    {
        shape_list_.push_back(current_shape_);
        if (history_index != -1 && history_index != history_actions.size() - 1)
        {
            history_actions.erase(
                history_actions.begin() + static_cast<int>(history_index) + 1,
                history_actions.end());
        }
        history_actions.push_back({ Action::kDraw, current_shape_ });
        history_index++;
        current_shape_.reset();
    }
}

void Canvas::mouse_click_event()
{
    if (!draw_status_)
    {
        draw_status_ = true;
        start_point_ = end_point_ = mouse_pos_in_canvas();
        current_shape_ = Canvas::create_shape(shape_type_, start_point_);
    }
    else
    {
        if (shape_type_ == ShapeType::kPolygon)
        {
            std::dynamic_pointer_cast<Polygon>(current_shape_)
                ->addPoint(mouse_pos_in_canvas());
            return;
        }

        finish_drawing();
    }
}

void Canvas::mouse_move_event()
{
    if (draw_status_)
    {
        end_point_ = mouse_pos_in_canvas();
        if (current_shape_)
        {
            current_shape_->update(end_point_);
        }
    }
}

void Canvas::mouse_release_event()
{
}

ImVec2 Canvas::mouse_pos_in_canvas() const
{
    ImGuiIO& io = ImGui::GetIO();
    const ImVec2 mouse_pos_in_canvas(
        io.MousePos.x - canvas_min_.x, io.MousePos.y - canvas_min_.y);
    return mouse_pos_in_canvas;
}
}  // namespace USTC_CG