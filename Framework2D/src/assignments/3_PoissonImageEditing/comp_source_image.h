#pragma once

#include "view/comp_image.h"
#include "view/poisson.h"

namespace USTC_CG
{
class CompSourceImage : public ImageEditor
{
   public:
    enum class RegionType
    {
        kDefault = 0,
        kRect = 1
    };

    explicit CompSourceImage(const std::string& label, const std::string& filename);
    ~CompSourceImage() noexcept override = default;

    void draw() override;

    // Point selecting interaction
    void enable_selecting(bool flag);
    void select_region();
    // Get the selected region in the source image, this would be a binary mask
    std::shared_ptr<Image> get_region();
    // Get the image data
    std::shared_ptr<Image> get_data();
    // Get the position to locate the region in the target image
    [[nodiscard]] ImVec2 get_position() const;
    std::shared_ptr<Poisson> get_poisson() const;

   private:
    RegionType region_type_ = RegionType::kRect;
    std::shared_ptr<Image> selected_region_;
    ImVec2 start_, end_;
    bool flag_enable_selecting_region_ = false;
    bool draw_status_ = false;
    std::shared_ptr<Poisson> poisson_;
};

}  // namespace USTC_CG