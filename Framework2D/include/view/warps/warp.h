#pragma once
#include <imgui.h>
#include <view/image.h>

#include <vector>

#include "Eigen/Core"
#include "Eigen/Dense"
#include "imgui.h"

namespace USTC_CG
{
class Warp
{
   public:
    Warp(const ImVec2& size, const std::vector<ImVec2>& control_points, const std::vector<ImVec2>& target_points) : size_(size)
    {
        control_points_.resize(control_points.size());
        target_points_.resize(target_points.size());
        for (auto i = 0; i < control_points.size(); i++)
        {
            control_points_[i] = { control_points[i].x, control_points[i].y };
            target_points_[i] = { target_points[i].x, target_points[i].y };
        }
    }
    ~Warp() = default;

    virtual void warmup() = 0;

    virtual Eigen::Vector2f warp(const Eigen::Vector2f& point) = 0;

   protected:
    ImVec2 size_;
    std::vector<Eigen::Vector2f> control_points_;
    std::vector<Eigen::Vector2f> target_points_;
};
}  // namespace USTC_CG

inline Eigen::Vector2f from_theta(float theta, float r)
{
    return { r * std::cos(theta), r * std::sin(theta) };
}
