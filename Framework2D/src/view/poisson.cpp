#include "view/poisson.h"

#include <iostream>
#include <utility>

namespace USTC_CG
{
std::vector<int> convert(const std::vector<unsigned char>& vec)
{
    return std::vector<int>{ vec[0], vec[1], vec[2] };
}

Eigen::Vector3d convert_v(const std::vector<unsigned char>& vec)
{
    Eigen::Vector3d result;
    for (int i = 0; i < 3; i++)
        result[i] = vec[i];
    return result;
}

double VecLength(const Eigen::Vector3d& vec)
{
    return sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
}

void Poisson::set_mask(Eigen::MatrixXi mask)
{
    mask_ = std::move(mask);
}

void Poisson::PoissonInit()
{
    width_ = (int)mask_.rows();
    height_ = (int)mask_.cols();
    index_.resize(width_, height_);
    index_.setZero();
    for (int i = 0; i < width_; i++)
        for (int j = 0; j < height_; j++)
            if (mask_(i, j) == 1)
                index_(i, j) = num_++;

    sparse_.resize(num_, num_);
    sparse_.setZero();

    std::vector<Eigen::Triplet<float>> coefficients;
    for (int i = 0; i < width_; i++)
    {
        for (int j = 0; j < height_; j++)
        {
            if (mask_(i, j) == 1)
            {
                int index = index_(i, j);
                coefficients.push_back(Eigen::Triplet<float>(index, index, 4));
                if (i > 0 && mask_(i - 1, j) == 1)
                    coefficients.push_back(Eigen::Triplet<float>(index, index_(i - 1, j), -1));

                if (j > 0 && mask_(i, j - 1) == 1)
                    coefficients.push_back(Eigen::Triplet<float>(index, index_(i, j - 1), -1));

                if (i < width_ - 1 && mask_(i + 1, j) == 1)
                    coefficients.push_back(Eigen::Triplet<float>(index, index_(i + 1, j), -1));

                if (j < height_ - 1 && mask_(i, j + 1) == 1)
                    coefficients.push_back(Eigen::Triplet<float>(index, index_(i, j + 1), -1));
            }
        }
    }
    sparse_.setFromTriplets(coefficients.begin(), coefficients.end());
    sparse_.makeCompressed();

    solver_.compute(sparse_);
    if (solver_.info() != Eigen::Success)
    {
        std::cerr << "Compute failed" << std::endl;
        return;
    }
}

void Poisson::apply_seamless(
    const Eigen::Vector2i& to_offset,
    const Eigen::Vector2i& from_offset,
    const std::shared_ptr<Image>& to,
    const Image& from)
{
    b_r_.resize(num_);
    b_g_.resize(num_);
    b_b_.resize(num_);
    b_r_.setZero();
    b_g_.setZero();
    b_b_.setZero();

    for (int i = 0; i < width_; i++)
    {
        for (int j = 0; j < height_; j++)
        {
            if (mask_(i, j) == 1)
            {
                int index = index_(i, j);
                int x = from_offset.x() + i;
                int y = from_offset.y() + j;

                auto temp_vec = convert_v(from.get_pixel(x, y));
                temp_vec *= 4;
                temp_vec -= convert_v(from.get_pixel(x + 1, y));
                temp_vec -= convert_v(from.get_pixel(x - 1, y));
                temp_vec -= convert_v(from.get_pixel(x, y - 1));
                temp_vec -= convert_v(from.get_pixel(x, y + 1));

                b_r_(index_(i, j)) += temp_vec[0];
                b_g_(index_(i, j)) += temp_vec[1];
                b_b_(index_(i, j)) += temp_vec[2];

                if (i == 0 || (i > 0 && mask_(i - 1, j) == 0))
                {
                    b_r_[index] += to->get_pixel(i + to_offset.x() - 1, j + to_offset.y())[0];
                    b_g_[index] += to->get_pixel(i + to_offset.x() - 1, j + to_offset.y())[1];
                    b_b_[index] += to->get_pixel(i + to_offset.x() - 1, j + to_offset.y())[2];
                }
                if (i == width_ - 1 || (i < width_ - 1 && mask_(i + 1, j) == 0))
                {
                    b_r_[index] += to->get_pixel(i + to_offset.x() + 1, j + to_offset.y())[0];
                    b_g_[index] += to->get_pixel(i + to_offset.x() + 1, j + to_offset.y())[1];
                    b_b_[index] += to->get_pixel(i + to_offset.x() + 1, j + to_offset.y())[2];
                }
                if (j == 0 || (j > 0 && mask_(i, j - 1) == 0))
                {
                    b_r_[index] += to->get_pixel(i + to_offset.x(), j + to_offset.y() - 1)[0];
                    b_g_[index] += to->get_pixel(i + to_offset.x(), j + to_offset.y() - 1)[1];
                    b_b_[index] += to->get_pixel(i + to_offset.x(), j + to_offset.y() - 1)[2];
                }
                if (j == height_ - 1 || (j < height_ - 1 && mask_(i, j + 1) == 0))
                {
                    b_r_[index] += to->get_pixel(i + to_offset.x(), j + to_offset.y() + 1)[0];
                    b_g_[index] += to->get_pixel(i + to_offset.x(), j + to_offset.y() + 1)[1];
                    b_b_[index] += to->get_pixel(i + to_offset.x(), j + to_offset.y() + 1)[2];
                }
            }
        }
    }

    Eigen::VectorXd vec_r(num_);
    Eigen::VectorXd vec_g(num_);
    Eigen::VectorXd vec_b(num_);

    vec_r = solver_.solve(b_r_);
    vec_g = solver_.solve(b_g_);
    vec_b = solver_.solve(b_b_);

    for (int i = 0; i < width_; i++)
    {
        for (int j = 0; j < height_; j++)
        {
            if (mask_(i, j) == 1)
            {
                int index = index_(i, j);
                int red = (int)vec_r(index);
                int green = (int)vec_g(index);
                int blue = (int)vec_b(index);

                unsigned char red_ = red > 255 ? 255 : (red < 0 ? 0 : red);
                unsigned char green_ = green > 255 ? 255 : (green < 0 ? 0 : green);
                unsigned char blue_ = blue > 255 ? 255 : (blue < 0 ? 0 : blue);

                if (i + to_offset.x() < to->width() && j + to_offset.y() < to->height())
                    to->set_pixel(i + to_offset.x(), j + to_offset.y(), { red_, green_, blue_ });
            }
        }
    }
}

void Poisson::apply_mixed(const Eigen::Vector2i& to_offset, const Eigen::Vector2i& from_offset, const std::shared_ptr<Image>& to, const Image& from)
{
    b_r_.resize(num_);
    b_g_.resize(num_);
    b_b_.resize(num_);
    b_r_.setZero();
    b_g_.setZero();
    b_b_.setZero();

    for (int i = 0; i < width_; i++)
    {
        for (int j = 0; j < height_; j++)
        {
            if (mask_(i, j) == 1)
            {
                int index = index_(i, j);
                int x = from_offset.x() + i;
                int y = from_offset.y() + j;
                int x_ = to_offset.x() + i;
                int y_ = to_offset.y() + j;

                Eigen::Vector3d vec, temp_vec, temp_vec_paste;

                temp_vec = convert_v(from.get_pixel(x, y));
                temp_vec -= convert_v(from.get_pixel(x + 1, y));
                temp_vec_paste = convert_v(to->get_pixel(x_, y_));
                temp_vec_paste -= convert_v(to->get_pixel(x_ + 1, y_));
                vec = VecLength(temp_vec) > VecLength(temp_vec_paste) ? temp_vec : temp_vec_paste;

                temp_vec = convert_v(from.get_pixel(x, y));
                temp_vec -= convert_v(from.get_pixel(x - 1, y));
                temp_vec_paste = convert_v(to->get_pixel(x_, y_));
                temp_vec_paste -= convert_v(to->get_pixel(x_ - 1, y_));
                vec += VecLength(temp_vec) > VecLength(temp_vec_paste) ? temp_vec : temp_vec_paste;

                temp_vec = convert_v(from.get_pixel(x, y));
                temp_vec -= convert_v(from.get_pixel(x, y + 1));
                temp_vec_paste = convert_v(to->get_pixel(x_, y_));
                temp_vec_paste -= convert_v(to->get_pixel(x_, y_ + 1));
                vec += VecLength(temp_vec) > VecLength(temp_vec_paste) ? temp_vec : temp_vec_paste;

                temp_vec = convert_v(from.get_pixel(x, y));
                temp_vec -= convert_v(from.get_pixel(x, y - 1));
                temp_vec_paste = convert_v(to->get_pixel(x_, y_));
                temp_vec_paste -= convert_v(to->get_pixel(x_, y_ - 1));
                vec += VecLength(temp_vec) > VecLength(temp_vec_paste) ? temp_vec : temp_vec_paste;

                b_r_(index_(i, j)) = vec[0];
                b_g_(index_(i, j)) = vec[1];
                b_b_(index_(i, j)) = vec[2];

                if (i == 0 || (i > 0 && mask_(i - 1, j) == 0))
                {
                    b_r_[index] += to->get_pixel(i + to_offset.x() - 1, j + to_offset.y())[0];
                    b_g_[index] += to->get_pixel(i + to_offset.x() - 1, j + to_offset.y())[1];
                    b_b_[index] += to->get_pixel(i + to_offset.x() - 1, j + to_offset.y())[2];
                }
                if (i == width_ - 1 || (i < width_ - 1 && mask_(i + 1, j) == 0))
                {
                    b_r_[index] += to->get_pixel(i + to_offset.x() + 1, j + to_offset.y())[0];
                    b_g_[index] += to->get_pixel(i + to_offset.x() + 1, j + to_offset.y())[1];
                    b_b_[index] += to->get_pixel(i + to_offset.x() + 1, j + to_offset.y())[2];
                }
                if (j == 0 || (j > 0 && mask_(i, j - 1) == 0))
                {
                    b_r_[index] += to->get_pixel(i + to_offset.x(), j + to_offset.y() - 1)[0];
                    b_g_[index] += to->get_pixel(i + to_offset.x(), j + to_offset.y() - 1)[1];
                    b_b_[index] += to->get_pixel(i + to_offset.x(), j + to_offset.y() - 1)[2];
                }
                if (j == height_ - 1 || (j < height_ - 1 && mask_(i, j + 1) == 0))
                {
                    b_r_[index] += to->get_pixel(i + to_offset.x(), j + to_offset.y() + 1)[0];
                    b_g_[index] += to->get_pixel(i + to_offset.x(), j + to_offset.y() + 1)[1];
                    b_b_[index] += to->get_pixel(i + to_offset.x(), j + to_offset.y() + 1)[2];
                }
            }
        }
    }

    Eigen::VectorXd vec_red(num_);
    Eigen::VectorXd vec_green(num_);
    Eigen::VectorXd vec_blue(num_);

    vec_red = solver_.solve(b_r_);
    vec_green = solver_.solve(b_g_);
    vec_blue = solver_.solve(b_b_);

    for (int i = 0; i < width_; i++)
        for (int j = 0; j < height_; j++)
            if (mask_(i, j) == 1)
            {
                int index = index_(i, j);
                int red = (int)vec_red(index);
                int green = (int)vec_green(index);
                int blue = (int)vec_blue(index);

                unsigned char red_ = red > 255 ? 255 : (red < 0 ? 0 : red);
                unsigned char green_ = green > 255 ? 255 : (green < 0 ? 0 : green);
                unsigned char blue_ = blue > 255 ? 255 : (blue < 0 ? 0 : blue);

                if (i + to_offset.x() < to->width() && j + to_offset.y() < to->height())
                    to->set_pixel(i + to_offset.x(), j + to_offset.y(), { red_, green_, blue_ });
            }
}

void Poisson::apply_paste(const Eigen::Vector2i& to_offset, const Eigen::Vector2i& from_offset, const std::shared_ptr<Image>& to, const Image& from)
{
    for (int i = 0; i < width_; i++)
        for (int j = 0; j < height_; j++)
            if (mask_(i, j) == 1 && i + to_offset.x() < to->width() && j + to_offset.y() < to->height() && i + from_offset.x() < from.width() &&
                j + from_offset.y() < from.height())
                to->set_pixel(i + to_offset.x(), j + to_offset.y(), from.get_pixel(i + from_offset.x(), j + from_offset.y()));
}
}  // namespace USTC_CG