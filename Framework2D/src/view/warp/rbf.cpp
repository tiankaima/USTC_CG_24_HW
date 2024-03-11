#include "view/warps/rbf.h"

#include <cmath>

namespace USTC_CG
{
void RBF::warmup()
{
    this->min_radius_.clear();
    this->min_radius_.resize(this->control_points_.size(), 0);
    if (control_points_.size() <= 1)
    {
        this->min_radius_[0] = 0;
    }
    else
    {
        for (auto i = 0; i < this->control_points_.size(); i++)
        {
            float min = std::numeric_limits<float>::max();
            for (auto j = 0; j < this->control_points_.size(); j++)
            {
                if (i == j)
                    continue;

                float distance =
                    (this->control_points_[i] - this->control_points_[j])
                        .norm();
                if (distance < min)
                    min = distance;
            }
            this->min_radius_[i] = min;
        }
    }

    Eigen::MatrixXf co;
    Eigen::VectorXf alpha_x;
    Eigen::VectorXf alpha_y;
    Eigen::VectorXf diff_x;
    Eigen::VectorXf diff_y;

    if (control_points_.empty())
        return;

    const int n = (int)control_points_.size();
    co.resize(n, n);
    alpha_x.resize(n);
    alpha_y.resize(n);
    diff_x.resize(n);
    diff_y.resize(n);

    for (auto i = 0; i < n; i++)
    {
        for (auto j = 0; j < n; j++)
        {
            float distance = (control_points_[i] - control_points_[j]).norm();
            co(i, j) = std::sqrt(
                distance * distance + min_radius_[i] * min_radius_[i]);
        }
        diff_x(i) = target_points_[i].x() - control_points_[i].x();
        diff_y(i) = target_points_[i].y() - control_points_[i].y();
    }
    alpha_x = co.colPivHouseholderQr().solve(diff_x);
    alpha_y = co.colPivHouseholderQr().solve(diff_y);
    if (n == 1)
    {
        const float coeff = co(0, 0) + 1e-5f;
        alpha_x(0) = diff_x(0) / coeff;
        alpha_y(0) = diff_y(0) / coeff;
    }
    alpha_x_list_.clear();
    alpha_y_list_.clear();
    for (int i = 0; i < n; ++i)
    {
        alpha_x_list_.push_back(alpha_x(i));
        alpha_y_list_.push_back(alpha_y(i));
    }
}

Eigen::Vector2f RBF::warp(const Eigen::Vector2f& point)
{
    Eigen::Vector2f result = { 0, 0 };
    for (int i = 0; i < control_points_.size(); ++i)
    {
        const auto& cp = control_points_[i];
        const float distance = (cp - point).norm();
        result.x() +=
            alpha_x_list_[i] *
            std::sqrt(distance * distance + min_radius_[i] * min_radius_[i]);
        result.y() +=
            alpha_y_list_[i] *
            std::sqrt(distance * distance + min_radius_[i] * min_radius_[i]);
    }
    result.x() += point.x();
    result.y() += point.y();
    return result;
}
}  // namespace USTC_CG
