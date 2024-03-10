#pragma once
#include <view/warps/warp.h>

namespace USTC_CG
{
class FishEye : public Warp
{
   public:
    using Warp::Warp;

    void warmup() override;

    Eigen::Vector2f warp(const Eigen::Vector2f& point) override;
};
}  // namespace USTC_CG
