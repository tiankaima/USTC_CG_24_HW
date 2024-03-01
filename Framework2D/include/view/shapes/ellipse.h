#pragma once

#include "shape.h"

namespace USTC_CG
{
class Ellipse : public Shape
{
   public:
    Ellipse() = default;

    Ellipse(ImVec2 start_point, ImVec2 end_point)
        : start_point_(start_point),
          end_point_(end_point)
    {
    }

    ~Ellipse() override = default;

    void draw(const Config& config) const override;

    void update(ImVec2 point) override;

   private:
    ImVec2 start_point_, end_point_;
};
}  // namespace USTC_CG