#include "view/poisson.h"

#include <iostream>
#include <utility>

namespace USTC_CG
{
Poisson::Poisson()
{
    pixels_num_ = 0;
    index_matrix_.resize(0, 0);
    sparse_matrix_.resize(0, 0);
    width_ = 0;
    height_ = 0;
}

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

void Poisson::set_inside_mask(Eigen::MatrixXi inside_mask)
{
    inside_mask_ = std::move(inside_mask);
}

void Poisson::PoissonInit(const Image& source_img)
{
    width_ = (int)inside_mask_.rows();
    height_ = (int)inside_mask_.cols();
    paste_point_[0] = 0;
    paste_point_[1] = 1;
    index_matrix_.resize(width_, height_);
    index_matrix_.setZero();
    for (int i = 0; i < width_; i++)
        for (int j = 0; j < height_; j++)
            if (inside_mask_(i, j) == 1)
                index_matrix_(i, j) = pixels_num_++;

    sparse_matrix_.resize(pixels_num_, pixels_num_);
    sparse_matrix_.setZero();

    std::vector<Eigen::Triplet<float>> coef;
    for (int i = 0; i < width_; i++)
    {
        for (int j = 0; j < height_; j++)
        {
            if (inside_mask_(i, j) == 1)
            {
                int index = index_matrix_(i, j);
                coef.push_back(Eigen::Triplet<float>(index, index, 4));
                if (i > 0 && inside_mask_(i - 1, j) == 1)
                    coef.push_back(Eigen::Triplet<float>(index, index_matrix_(i - 1, j), -1));

                if (j > 0 && inside_mask_(i, j - 1) == 1)
                    coef.push_back(Eigen::Triplet<float>(index, index_matrix_(i, j - 1), -1));

                if (i < width_ - 1 && inside_mask_(i + 1, j) == 1)
                    coef.push_back(Eigen::Triplet<float>(index, index_matrix_(i + 1, j), -1));

                if (j < height_ - 1 && inside_mask_(i, j + 1) == 1)
                    coef.push_back(Eigen::Triplet<float>(index, index_matrix_(i, j + 1), -1));
            }
        }
    }
    sparse_matrix_.setFromTriplets(coef.begin(), coef.end());
    sparse_matrix_.makeCompressed();

    Predecomposition();
}

void Poisson::Predecomposition()
{
    solver.compute(sparse_matrix_);
    if (solver.info() != Eigen::Success)
    {
        std::cerr << "Compute Matrix is error" << std::endl;
        return;
    }
}

void Poisson::GetPoisson(Eigen::Vector<int, 2> paste_point, Eigen::Vector<int, 2> source_point, Image& paste_img_, const Image& source_img_)
{
    paste_point_ = std::move(paste_point);
    source_point_ = std::move(source_point);

    div_red_.resize(pixels_num_);
    div_green_.resize(pixels_num_);
    div_blue_.resize(pixels_num_);
    div_red_.setZero();
    div_green_.setZero();
    div_blue_.setZero();

    for (int i = 0; i < width_; i++)
    {
        for (int j = 0; j < height_; j++)
        {
            if (inside_mask_(i, j) == 1)
            {
                int index = index_matrix_(i, j);
                int x = source_point_.x() + i;
                int y = source_point_.y() + j;

                auto temp_vec = convert_v(source_img_.get_pixel(x, y));
                temp_vec *= 4;
                temp_vec -= convert_v(source_img_.get_pixel(x + 1, y));
                temp_vec -= convert_v(source_img_.get_pixel(x - 1, y));
                temp_vec -= convert_v(source_img_.get_pixel(x, y - 1));
                temp_vec -= convert_v(source_img_.get_pixel(x, y + 1));

                div_red_(index_matrix_(i, j)) += temp_vec[0];
                div_green_(index_matrix_(i, j)) += temp_vec[1];
                div_blue_(index_matrix_(i, j)) += temp_vec[2];

                if (i == 0 || (i > 0 && inside_mask_(i - 1, j) == 0))
                {
                    div_red_[index] += paste_img_.get_pixel(i + paste_point_.x() - 1, j + paste_point_.y())[0];
                    div_green_[index] += paste_img_.get_pixel(i + paste_point_.x() - 1, j + paste_point_.y())[1];
                    div_blue_[index] += paste_img_.get_pixel(i + paste_point_.x() - 1, j + paste_point_.y())[2];
                }
                if (i == width_ - 1 || (i < width_ - 1 && inside_mask_(i + 1, j) == 0))
                {
                    div_red_[index] += paste_img_.get_pixel(i + paste_point_.x() + 1, j + paste_point_.y())[0];
                    div_green_[index] += paste_img_.get_pixel(i + paste_point_.x() + 1, j + paste_point_.y())[1];
                    div_blue_[index] += paste_img_.get_pixel(i + paste_point_.x() + 1, j + paste_point_.y())[2];
                }
                if (j == 0 || (j > 0 && inside_mask_(i, j - 1) == 0))
                {
                    div_red_[index] += paste_img_.get_pixel(i + paste_point_.x(), j + paste_point_.y() - 1)[0];
                    div_green_[index] += paste_img_.get_pixel(i + paste_point_.x(), j + paste_point_.y() - 1)[1];
                    div_blue_[index] += paste_img_.get_pixel(i + paste_point_.x(), j + paste_point_.y() - 1)[2];
                }
                if (j == height_ - 1 || (j < height_ - 1 && inside_mask_(i, j + 1) == 0))
                {
                    div_red_[index] += paste_img_.get_pixel(i + paste_point_.x(), j + paste_point_.y() + 1)[0];
                    div_green_[index] += paste_img_.get_pixel(i + paste_point_.x(), j + paste_point_.y() + 1)[1];
                    div_blue_[index] += paste_img_.get_pixel(i + paste_point_.x(), j + paste_point_.y() + 1)[2];
                }
            }
        }
    }

    Eigen::VectorXd vec_red(pixels_num_);
    Eigen::VectorXd vec_green(pixels_num_);
    Eigen::VectorXd vec_blue(pixels_num_);

    vec_red = solver.solve(div_red_);
    vec_green = solver.solve(div_green_);
    vec_blue = solver.solve(div_blue_);

    for (int i = 0; i < width_; i++)
    {
        for (int j = 0; j < height_; j++)
        {
            if (inside_mask_(i, j) == 1)
            {
                int index = index_matrix_(i, j);
                int red = (int)vec_red(index);
                int green = (int)vec_green(index);
                int blue = (int)vec_blue(index);

                unsigned char red_ = red > 255 ? 255 : (red < 0 ? 0 : red);
                unsigned char green_ = green > 255 ? 255 : (green < 0 ? 0 : green);
                unsigned char blue_ = blue > 255 ? 255 : (blue < 0 ? 0 : blue);

                if (i + paste_point_.x() < paste_img_.width() && j + paste_point_.y() < paste_img_.height())
                    paste_img_.set_pixel(i + paste_point_.x(), j + paste_point_.y(), { red_, green_, blue_ });
            }
        }
    }
}

double Poisson::VecLength(const Eigen::Vector3d& vec)
{
    return sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
}

void Poisson::MixingPoisson(Eigen::Vector<int, 2> paste_point, Eigen::Vector<int, 2> source_point, Image& paste_img_, const Image& source_img_)
{
    paste_point_ = std::move(paste_point);
    source_point_ = std::move(source_point);

    div_red_.resize(pixels_num_);
    div_green_.resize(pixels_num_);
    div_blue_.resize(pixels_num_);
    div_red_.setZero();
    div_green_.setZero();
    div_blue_.setZero();

    for (int i = 0; i < width_; i++)
    {
        for (int j = 0; j < height_; j++)
        {
            if (inside_mask_(i, j) == 1)
            {
                int index = index_matrix_(i, j);
                int x = source_point_.x() + i;
                int y = source_point_.y() + j;
                int x_ = paste_point_.x() + i;
                int y_ = paste_point_.y() + j;

                Eigen::Vector3d vec, temp_vec, temp_vec_paste;

                temp_vec = convert_v(source_img_.get_pixel(x, y));
                temp_vec -= convert_v(source_img_.get_pixel(x + 1, y));
                temp_vec_paste = convert_v(paste_img_.get_pixel(x_, y_));
                temp_vec_paste -= convert_v(paste_img_.get_pixel(x_ + 1, y_));
                vec = VecLength(temp_vec) > VecLength(temp_vec_paste) ? temp_vec : temp_vec_paste;

                temp_vec = convert_v(source_img_.get_pixel(x, y));
                temp_vec -= convert_v(source_img_.get_pixel(x - 1, y));
                temp_vec_paste = convert_v(paste_img_.get_pixel(x_, y_));
                temp_vec_paste -= convert_v(paste_img_.get_pixel(x_ - 1, y_));
                vec += VecLength(temp_vec) > VecLength(temp_vec_paste) ? temp_vec : temp_vec_paste;

                temp_vec = convert_v(source_img_.get_pixel(x, y));
                temp_vec -= convert_v(source_img_.get_pixel(x, y + 1));
                temp_vec_paste = convert_v(paste_img_.get_pixel(x_, y_));
                temp_vec_paste -= convert_v(paste_img_.get_pixel(x_, y_ + 1));
                vec += VecLength(temp_vec) > VecLength(temp_vec_paste) ? temp_vec : temp_vec_paste;

                temp_vec = convert_v(source_img_.get_pixel(x, y));
                temp_vec -= convert_v(source_img_.get_pixel(x, y - 1));
                temp_vec_paste = convert_v(paste_img_.get_pixel(x_, y_));
                temp_vec_paste -= convert_v(paste_img_.get_pixel(x_, y_ - 1));
                vec += VecLength(temp_vec) > VecLength(temp_vec_paste) ? temp_vec : temp_vec_paste;

                div_red_(index_matrix_(i, j)) = vec[0];
                div_green_(index_matrix_(i, j)) = vec[1];
                div_blue_(index_matrix_(i, j)) = vec[2];

                if (i == 0 || (i > 0 && inside_mask_(i - 1, j) == 0))
                {
                    div_red_[index] += paste_img_.get_pixel(i + paste_point_.x() - 1, j + paste_point_.y())[0];
                    div_green_[index] += paste_img_.get_pixel(i + paste_point_.x() - 1, j + paste_point_.y())[1];
                    div_blue_[index] += paste_img_.get_pixel(i + paste_point_.x() - 1, j + paste_point_.y())[2];
                }
                if (i == width_ - 1 || (i < width_ - 1 && inside_mask_(i + 1, j) == 0))
                {
                    div_red_[index] += paste_img_.get_pixel(i + paste_point_.x() + 1, j + paste_point_.y())[0];
                    div_green_[index] += paste_img_.get_pixel(i + paste_point_.x() + 1, j + paste_point_.y())[1];
                    div_blue_[index] += paste_img_.get_pixel(i + paste_point_.x() + 1, j + paste_point_.y())[2];
                }
                if (j == 0 || (j > 0 && inside_mask_(i, j - 1) == 0))
                {
                    div_red_[index] += paste_img_.get_pixel(i + paste_point_.x(), j + paste_point_.y() - 1)[0];
                    div_green_[index] += paste_img_.get_pixel(i + paste_point_.x(), j + paste_point_.y() - 1)[1];
                    div_blue_[index] += paste_img_.get_pixel(i + paste_point_.x(), j + paste_point_.y() - 1)[2];
                }
                if (j == height_ - 1 || (j < height_ - 1 && inside_mask_(i, j + 1) == 0))
                {
                    div_red_[index] += paste_img_.get_pixel(i + paste_point_.x(), j + paste_point_.y() + 1)[0];
                    div_green_[index] += paste_img_.get_pixel(i + paste_point_.x(), j + paste_point_.y() + 1)[1];
                    div_blue_[index] += paste_img_.get_pixel(i + paste_point_.x(), j + paste_point_.y() + 1)[2];
                }
            }
        }
    }

    Eigen::VectorXd vec_red(pixels_num_);
    Eigen::VectorXd vec_green(pixels_num_);
    Eigen::VectorXd vec_blue(pixels_num_);

    vec_red = solver.solve(div_red_);
    vec_green = solver.solve(div_green_);
    vec_blue = solver.solve(div_blue_);

    for (int i = 0; i < width_; i++)
        for (int j = 0; j < height_; j++)
            if (inside_mask_(i, j) == 1)
            {
                int index = index_matrix_(i, j);
                int red = (int)vec_red(index);
                int green = (int)vec_green(index);
                int blue = (int)vec_blue(index);

                unsigned char red_ = red > 255 ? 255 : (red < 0 ? 0 : red);
                unsigned char green_ = green > 255 ? 255 : (green < 0 ? 0 : green);
                unsigned char blue_ = blue > 255 ? 255 : (blue < 0 ? 0 : blue);

                if (i + paste_point_.x() < paste_img_.width() && j + paste_point_.y() < paste_img_.height())
                    paste_img_.set_pixel(i + paste_point_.x(), j + paste_point_.y(), { red_, green_, blue_ });
            }
}

void Poisson::CopyPaste(Eigen::Vector<int, 2> paste_point, Eigen::Vector<int, 2> source_point, Image& paste_img_, const Image& source_img_)
{
    paste_point_ = std::move(paste_point);
    source_point_ = std::move(source_point);
    for (int i = 0; i < width_; i++)
        for (int j = 0; j < height_; j++)
            if (inside_mask_(i, j) == 1 && i + paste_point_.x() < paste_img_.width() && j + paste_point_.y() < paste_img_.height() &&
                i + source_point_.x() < source_img_.width() && j + source_point_.y() < source_img_.height())
                paste_img_.set_pixel(i + paste_point_.x(), j + paste_point_.y(), source_img_.get_pixel(i + source_point_.x(), j + source_point_.y()));
}
}  // namespace USTC_CG