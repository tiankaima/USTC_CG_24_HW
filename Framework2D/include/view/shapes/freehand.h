#pragma once

#include <vector>

#include "shape.h"

namespace USTC_CG
{
class Freehand : public Shape
{
   public:
    Freehand() = default;

    Freehand(ImVec2 start_point) : points_({ start_point })
    {
    }

    ~Freehand() override = default;

    // Overrides draw function to implement Freehand-specific drawing logic
    void draw(const Config& config) const override;

    // Overrides Shape's update function to adjust the end point during
    // interaction
    void update(ImVec2 point) override;

   private:
    std::vector<ImVec2> points_;
};
}  // namespace USTC_CG
