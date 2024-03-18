#pragma once

#include "view/comp_image.h"
#include "view/warps/fisheye.h"
#include "view/warps/idw.h"
#include "view/warps/rbf.h"
#include "view/warps/warp.h"

namespace USTC_CG
{
// Image component for warping and other functions
class CompWarping : public ImageEditor
{
   public:
    explicit CompWarping(const std::string& label, const std::string& filename);
    ~CompWarping() noexcept override = default;

    void draw() override;

    enum class WarpType
    {
        kDefault,
        kIDW,
        kRBF,
        kFishEye,
    };

    static constexpr std::vector<WarpType> WarpTypeList()
    {
        return { WarpType::kDefault, WarpType::kIDW, WarpType::kRBF, WarpType::kFishEye };
    }

    static constexpr std::string WarpTypeName(const WarpType& type)
    {
        switch (type)
        {
            case WarpType::kDefault: return "Default";
            case WarpType::kIDW: return "IDW";
            case WarpType::kRBF: return "RBF";
            case WarpType::kFishEye: return "FishEye";
            default: return "Unknown";
        }
    }

    static std::shared_ptr<Warp>
    create_warp(WarpType type, const ImVec2& size, const std::vector<ImVec2>& start_points, const std::vector<ImVec2>& end_points)
    {
        switch (type)
        {
            case WarpType::kDefault:  //
                return nullptr;
            case WarpType::kIDW:  //
                return std::make_shared<IDW>(size, start_points, end_points);
            case WarpType::kRBF:  //
                return std::make_shared<RBF>(size, start_points, end_points);
            case WarpType::kFishEye:  //
                return std::make_shared<FishEye>(size, start_points, end_points);
            default:  //
                return nullptr;
        }
    }

    // Simple edit functions
    void invert();
    void mirror(bool is_horizontal, bool is_vertical);
    void gray_scale();
    void warping(WarpType type);
    void restore();

    // Point selecting interaction
    void enable_selecting(bool flag);
    void select_points();
    void init_selections();

   private:
    // Store the original image data
    std::shared_ptr<Image> back_up_;
    // The selected point couples for image warping
    std::vector<ImVec2> start_points_, end_points_;

    ImVec2 start_, end_;
    bool flag_enable_selecting_points_ = false;
    bool draw_status_ = false;
};

}  // namespace USTC_CG