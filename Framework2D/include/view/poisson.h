#pragma once
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Sparse>

// #include "ScanLine.h"
#include "image.h"
#include "imgui.h"

namespace USTC_CG
{
class Poisson
{
   public:
    Poisson();
    ~Poisson() = default;
    void PoissonInit(const Image& source_img);
    void set_inside_mask(Eigen::MatrixXi inside_mask);
    void GetPoisson(Eigen::Vector<int, 2> paste_point, Eigen::Vector<int, 2> source_point, Image& paste_img_, const Image& source_img_);
    void MixingPoisson(Eigen::Vector<int, 2> paste_point, Eigen::Vector<int, 2> source_point, Image& paste_img_, const Image& source_img_);
    void CopyPaste(Eigen::Vector<int, 2> paste_point, Eigen::Vector<int, 2> source_point, Image& paste_img_, const Image& source_img_);

   private:
    void Predecomposition();
    double VecLength(const Eigen::Vector3d& vec);

   private:
    // Image source_img_;
    Eigen::Vector<int, 2> source_point_, paste_point_;
    int pixels_num_;
    Eigen::MatrixXi index_matrix_;
    int width_, height_;
    Eigen::SparseMatrix<double> sparse_matrix_;
    Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> solver;
    Eigen::MatrixXi inside_mask_;
    Eigen::VectorXd div_red_;
    Eigen::VectorXd div_green_;
    Eigen::VectorXd div_blue_;
};
}  // namespace USTC_CG