#include "view/warps/rbf.h"

#include <cmath>

namespace USTC_CG
{
void RBF::warmup()
{
}

Eigen::Vector2f RBF::warp(const Eigen::Vector2f& point)
{
    Eigen::Vector2f result = { 0, 0 };
    for (int i = 0; i < control_points_.size(); ++i)
    {
        const auto& cp = control_points_[i];
        const auto& tp = target_points_[i];
        const float distance = (cp - point).norm();
        result += tp * distance;
    }
    return result;
}
}  // namespace USTC_CG
