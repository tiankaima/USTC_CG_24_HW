#pragma once

#include "shape.h"

namespace USTC_CG
{
class Line : public Shape
{
   public:
    Line() = default;

    Line(ImVec2 start_point, ImVec2 end_point)
        : start_point_(start_point),
          end_point_(end_point)
    {
    }

    ~Line() override = default;

    // Overrides draw function to implement line-specific drawing logic
    void draw(const Config& config) const override;

    // Overrides Shape's update function to adjust the end point during
    // interaction
    void update(ImVec2 point) override;

   private:
    ImVec2 start_point_, end_point_;
};
}  // namespace USTC_CG
