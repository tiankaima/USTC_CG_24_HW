#include "view/warps/fisheye.h"

#include <cmath>

namespace USTC_CG
{
void FishEye::warmup()
{
}

Eigen::Vector2f FishEye::warp(const Eigen::Vector2f& point)
{
    Eigen::Vector2f center = { size_.x / 2, size_.y / 2 };
    float distance = (point - center).norm();
    if (distance < 1e-6)
        return center;
    float ratio = std::sqrt(distance) * 10 / distance;
    return center + (point - center) * ratio;
}
}  // namespace USTC_CG
