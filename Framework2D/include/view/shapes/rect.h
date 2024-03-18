#pragma once

#include <algorithm>
#include <cmath>

#include "shape.h"

namespace USTC_CG
{
class Rect : public Shape
{
   public:
    Rect() = default;

    Rect(ImVec2 start_point, ImVec2 end_point) : start_point_(start_point), end_point_(end_point)
    {
    }

    ~Rect() override = default;

    // Draws the rectangle on the screen
    // Overrides draw function to implement rectangle-specific drawing logic
    void draw(const Config& config) const override;

    // Overrides Shape's update function to adjust the rectangle size during
    // interaction
    void update(ImVec2 point) override;

   private:
    ImVec2 start_point_, end_point_;
};
}  // namespace USTC_CG
