#pragma once

#include <imgui.h>

#include <memory>
#include <string>
#include <vector>

#include "view/component.h"
#include "view/shapes/ellipse.h"
#include "view/shapes/freehand.h"
#include "view/shapes/line.h"
#include "view/shapes/polygon.h"
#include "view/shapes/rect.h"
#include "view/shapes/shape.h"

namespace USTC_CG
{

// Canvas class for drawing shapes.
class Canvas : public Component
{
   public:
    // Inherits constructor from Component.
    using Component::Component;

    // Override the draw method from the parent Component class.
    void draw() override;

    // Enumeration for supported shape types.
    enum class ShapeType
    {
        kDefault = 0,
        kLine = 1,
        kRect = 2,
        kEllipse = 3,
        kPolygon = 4,
        kFreehand = 5
    };

    [[nodiscard]] static constexpr std::vector<ShapeType> all_types()
    {
        return {
            ShapeType::kDefault, ShapeType::kLine,    ShapeType::kRect,
            ShapeType::kEllipse, ShapeType::kPolygon, ShapeType::kFreehand
        };
    }

    [[nodiscard]] static constexpr std::string name(const ShapeType& type)
    {
        switch (type)
        {
            case ShapeType::kDefault: return "Default";
            case ShapeType::kLine: return "Line";
            case ShapeType::kRect: return "Rect";
            case ShapeType::kEllipse: return "Ellipse";
            case ShapeType::kPolygon: return "Polygon";
            case ShapeType::kFreehand: return "Freehand";
            default: return "Unknown";
        }
    }

    [[nodiscard]] static std::shared_ptr<Shape> create_shape(
        const ShapeType& type,
        ImVec2 point)
    {
        switch (type)
        {
            case ShapeType::kDefault:  //
                return nullptr;
            case ShapeType::kLine:  //
                return std::make_shared<Line>(point, point);
            case ShapeType::kRect:  //
                return std::make_shared<Rect>(point, point);
            case ShapeType::kEllipse:
                return std::make_shared<Ellipse>(point, point);
            case ShapeType::kPolygon:  //
                return std::make_shared<Polygon>(point);
            case ShapeType::kFreehand:  //
                return std::make_shared<Freehand>(point);
            default:  //
                return nullptr;
        }
    }

    [[nodiscard]] std::string shape_type_name() const
    {
        return name(shape_type_);
    }

    void set_type(ShapeType type);

    void undo();
    void redo();
    void clear();

    // Set canvas attributes (position and size).
    void set_attributes(const ImVec2& min, const ImVec2& size);

    // Controls the visibility of the canvas background.
    void show_background(bool flag);

   private:
    // Drawing functions.
    void draw_background();
    void draw_shapes() const;

    void finish_drawing();

    // Event handlers for mouse interactions.
    void mouse_click_event();
    void mouse_move_event();
    void mouse_release_event();

    // Calculates mouse's relative position in the canvas.
    [[nodiscard]] ImVec2 mouse_pos_in_canvas() const;

    // Canvas attributes.
    ImVec2 canvas_min_;         // Top-left corner of the canvas.
    ImVec2 canvas_max_;         // Bottom-right corner of the canvas.
    ImVec2 canvas_size_;        // Size of the canvas.
    bool draw_status_ = false;  // Is the canvas currently being drawn on.

    ImVec2 canvas_minimal_size_ = ImVec2(50.f, 50.f);
    ImU32 background_color_ = IM_COL32(50, 50, 50, 255);
    ImU32 border_color_ = IM_COL32(255, 255, 255, 255);
    bool show_background_ = true;  // Controls background visibility.

    // Mouse interaction status.
    bool is_hovered_, is_active_;

    // Current shape being drawn.
    ShapeType shape_type_ = ShapeType::kDefault;
    ImVec2 start_point_, end_point_;
    std::shared_ptr<Shape> current_shape_;

    // List of shapes drawn on the canvas.
    std::vector<std::shared_ptr<Shape>> shape_list_;

    enum class Action
    {
        kDraw = 0,
        kDelete = 1,
    };

    struct History
    {
        Action action;
        std::shared_ptr<Shape> shape;
        size_t delete_index = -1;
    };

    std::vector<History> history_actions;
    size_t history_index = -1;
};
}  // namespace USTC_CG
