#pragma once

#include <vector>

#include "shape.h"

namespace USTC_CG
{
class Polygon : public Shape
{
   public:
    Polygon() = default;

    Polygon(ImVec2 start_point) : points_({ start_point })
    {
    }

    ~Polygon() override = default;

    // Overrides draw function to implement Polygon-specific drawing logic
    void draw(const Config& config) const override;

    // Overrides Shape's update function to adjust the end point during
    // interaction
    void update(ImVec2 point) override;

    void addPoint(ImVec2 point);

   private:
    std::vector<ImVec2> points_;
};
}  // namespace USTC_CG