#pragma once
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Sparse>

#include "image.h"
#include "imgui.h"

namespace USTC_CG
{
class Poisson
{
   public:
    enum class CloneType
    {
        kDefault = 0,
        kPaste = 1,
        kSeamless = 2,
        kMixed = 3
    };

    constexpr static std::vector<CloneType> CloneTypeList()
    {
        return { CloneType::kDefault, CloneType::kPaste, CloneType::kSeamless, CloneType::kMixed };
    }

    constexpr static std::string CloneTypeName(const CloneType& type)
    {
        switch (type)
        {
            case CloneType::kDefault: return "Default";
            case CloneType::kPaste: return "Paste";
            case CloneType::kSeamless: return "Seamless";
            case CloneType::kMixed: return "Mixed";
            default: return "Unknown";
        }
    }

    Poisson()
    {
        width_ = 0;
        height_ = 0;
        num_ = 0;
    }
    ~Poisson() = default;
    void PoissonInit();
    void set_mask(Eigen::MatrixXi mask);
    void apply_paste(const Eigen::Vector2i& to_offset, const Eigen::Vector2i& from_offset, const std::shared_ptr<Image>& to, const Image& from);
    void apply_seamless(const Eigen::Vector2i& to_offset, const Eigen::Vector2i& from_offset, const std::shared_ptr<Image>& to, const Image& from);
    void apply_mixed(const Eigen::Vector2i& to_offset, const Eigen::Vector2i& from_offset, const std::shared_ptr<Image>& to, const Image& from);

    void apply_type(
        const Eigen::Vector2i& to_offset,
        const Eigen::Vector2i& from_offset,
        const std::shared_ptr<Image>& to,
        const Image& from,
        CloneType type)
    {
        switch (type)
        {
            case CloneType::kDefault: break;
            case CloneType::kPaste: apply_paste(to_offset, from_offset, to, from); break;
            case CloneType::kSeamless: apply_seamless(to_offset, from_offset, to, from); break;
            case CloneType::kMixed: apply_mixed(to_offset, from_offset, to, from); break;
            default: break;
        }
    }

   private:
    Eigen::MatrixXi index_;
    int width_, height_, num_;
    Eigen::SparseMatrix<double> sparse_;
    Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> solver_;
    Eigen::MatrixXi mask_;
    Eigen::VectorXd b_r_;
    Eigen::VectorXd b_g_;
    Eigen::VectorXd b_b_;
};
}  // namespace USTC_CG