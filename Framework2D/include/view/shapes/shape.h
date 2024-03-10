#pragma once
#include <cmath>

#include "imgui.h"

using IM_COL32_D = decltype(IM_COL32(0, 0, 0, 0));

namespace USTC_CG
{
class Shape
{
   public:
    // Draw Settings
    struct Config
    {
        // Bias to adjust the shape's position on the screen
        ImVec2 bias = ImVec2(0.0f, 0.0f);
        // Line color in RGBA format
        IM_COL32_D line_color = IM_COL32(255, 0, 0, 255);
        float line_thickness = 2.0f;
        bool is_current_drawing = false;
    };

   public:
    virtual ~Shape() = default;

    /**
     * Draws the shape on the screen.
     * This is a pure virtual function that must be implemented by all
     * derived classes.
     *
     * @param config The configuration settings for drawing, including line
     * color, thickness, and bias.
     *               - line_color defines the color of the shape's outline.
     *               - line_thickness determines how thick the outline will
     * be.
     *               - bias is used to adjust the shape's position on the
     * screen.
     */
    virtual void draw(const Config& config) const = 0;
    /**
     * Updates the state of the shape.
     * This function allows for dynamic modification of the shape, in
     * response to user interactions like dragging.
     *
     * @param point Dragging point. e.g. end point of a line.
     */
    virtual void update(ImVec2 point) = 0;
};
}  // namespace USTC_CG

inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
    return { lhs.x + rhs.x, lhs.y + rhs.y };
}

inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
    return { lhs.x - rhs.x, lhs.y - rhs.y };
}

inline ImVec2 operator*(const ImVec2& lhs, double rhs)
{
    return { lhs.x * (float)rhs, lhs.y * (float)rhs };
}

inline ImVec2 operator/(const ImVec2& lhs, double rhs)
{
    return { lhs.x / (float)rhs, lhs.y / (float)rhs };
}

inline ImVec2 from_theta(double theta, double r = 1)
{
    return { static_cast<float>(r * cos(theta)),
             static_cast<float>(r * sin(theta)) };
}

inline auto distance(const ImVec2& lhs, const ImVec2& rhs)
{
    return std::sqrt(
        (lhs.x - rhs.x) * (lhs.x - rhs.x) + (lhs.y - rhs.y) * (lhs.y - rhs.y));
}

/**
 * Adjusts the alpha value of a color.
 * @param color The color to adjust.
 * @param alpha The alpha value to multiply the color's alpha by.
 */
inline IM_COL32_D opacity(IM_COL32_D color, float alpha)
{
    return IM_COL32(
        (color >> IM_COL32_A_SHIFT) * alpha,
        (color >> IM_COL32_B_SHIFT) & 0xFF,
        (color >> IM_COL32_G_SHIFT) & 0xFF,
        (color >> IM_COL32_R_SHIFT) & 0xFF);
}