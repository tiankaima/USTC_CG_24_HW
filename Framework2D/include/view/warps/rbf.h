#pragma once
#include <view/warps/warp.h>

namespace USTC_CG
{
class RBF : public Warp
{
   public:
    using Warp::Warp;

    void warmup() override;

    Eigen::Vector2f warp(const Eigen::Vector2f& point) override;

   private:
    std::vector<float> min_radius_;
    std::vector<float> alpha_x_list_;
    std::vector<float> alpha_y_list_;
};
}  // namespace USTC_CG