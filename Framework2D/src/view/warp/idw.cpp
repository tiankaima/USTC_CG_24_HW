#include "view/warps/idw.h"

#include <Eigen/Dense>
#include <cmath>

#define mu 2.5

namespace USTC_CG
{
/**
 * @brief warmup the idw algorithm, calculate the transform matrix Ds_
 */
void IDW::warmup()
{
    Ds_.clear();
    Ds_.resize(control_points_.size());
    for (auto i = 0; i < control_points_.size(); i++)
    {
        Ds_[i] = Eigen::Matrix2f::Identity();
    }

    if (control_points_.size() == 1)
        return;

    for (auto i = 0; i < control_points_.size(); i++)
    {
        Eigen::MatrixXf co(2, 2);
        Eigen::VectorXf b_1(2);
        Eigen::VectorXf b_2(2);
        co.setZero();
        b_1.setZero();
        b_2.setZero();

        for (auto j = 0; j < control_points_.size(); j++)
        {
            if (i == j)
                continue;

            auto sigma =
                std::pow((control_points_[i] - control_points_[j]).norm(), -2);
            for (const auto m : { 0, 1 })
                for (const auto n : { 0, 1 })
                    co(m, n) +=
                        (float)sigma *
                        (control_points_[j](m) - control_points_[i](m)) *
                        (control_points_[j](n) - control_points_[i](n));

            for (const auto m : { 0, 1 })
            {
                b_1(m) += (float)sigma *
                          (control_points_[j](m) - control_points_[i](m)) *
                          (target_points_[j](0) - target_points_[i](0));
                b_2(m) += (float)sigma *
                          (control_points_[j](m) - control_points_[i](m)) *
                          (target_points_[j](1) - target_points_[i](1));
            }
        }

        b_1 = co.colPivHouseholderQr().solve(b_1);
        b_2 = co.colPivHouseholderQr().solve(b_2);
        for (auto m : { 0, 1 })
        {
            Ds_[i](0, m) = b_1(m);
            Ds_[i](1, m) = b_2(m);
        }
    }
}

/**
 * @brief use idw algorithm to warp the point
 * @param point (x, y), the point to be warped
 * @return the warped point (x, y)
 */
Eigen::Vector2f IDW::warp(const Eigen::Vector2f& point)
{
    Eigen::Vector2f result = { 0, 0 };
    auto weight_sum = 0.0;
    for (int i = 0; i < control_points_.size(); ++i)
    {
        const auto& cp = control_points_[i];
        const auto& tp = target_points_[i];

        auto weight = 1.0 / (cp - point).norm();
        weight_sum += weight;
        result += weight * (Ds_[i] * (point - cp) + tp);
    }
    return result / weight_sum;
}
}  // namespace USTC_CG