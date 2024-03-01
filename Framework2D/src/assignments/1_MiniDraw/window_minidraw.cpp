#include "window_minidraw.h"

#include <iostream>

namespace USTC_CG
{
MiniDraw::MiniDraw(const std::string& window_name) : Window(window_name)
{
    p_canvas_ = std::make_shared<Canvas>("Cmpt.Canvas");
}

void MiniDraw::draw()
{
    draw_canvas();
}

void MiniDraw::draw_canvas()
{
    // Set a full screen canvas view
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    if (ImGui::Begin(
            "Canvas",
            &flag_show_canvas_view_,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground))
    {
        for (const auto type : Canvas::all_types())
        {
            if (ImGui::Button(Canvas::name(type).c_str()))
            {
                p_canvas_->set_type(type);
            }
            ImGui::SameLine();
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear"))
        {
            std::cout << "Clear canvas" << std::endl;
            p_canvas_->clear();
        }
        ImGui::SameLine();
        if (ImGui::Button("Undo"))
        {
            std::cout << "Undo" << std::endl;
            p_canvas_->undo();
        }
        ImGui::SameLine();
        if (ImGui::Button("Redo"))
        {
            std::cout << "Redo" << std::endl;
            p_canvas_->redo();
        }

        ImGui::Text("Press left mouse to add shapes.");

        ImGui::Text("Current shape: %s", p_canvas_->shape_type_name().c_str());

        // Set the canvas to fill the rest of the window
        const auto& canvas_min = ImGui::GetCursorScreenPos();
        const auto& canvas_size = ImGui::GetContentRegionAvail();
        p_canvas_->set_attributes(canvas_min, canvas_size);
        p_canvas_->draw();
    }
    ImGui::End();
}
}  // namespace USTC_CG