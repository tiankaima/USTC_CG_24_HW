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
        kRect = 1,
        kPolygon = 2
    };

    constexpr static std::vector<RegionType> RegionTypeList()
    {
        return { RegionType::kDefault, RegionType::kRect, RegionType::kPolygon };
    }

    constexpr static std::string RegionTypeName(const RegionType& type)
    {
        switch (type)
        {
            case RegionType::kDefault: return "Default";
            case RegionType::kRect: return "Rect";
            case RegionType::kPolygon: return "Polygon";
            default: return "Unknown";
        }
    }

    explicit CompSourceImage(const std::string& label, const std::string& filename);
    ~CompSourceImage() noexcept override = default;

    void draw() override;

    void enable_selecting(bool flag);
    void set_region_type(RegionType type)
    {
        region_type_ = type;
    }
    RegionType get_region_type() const
    {
        return region_type_;
    }
    [[nodiscard]] std::string region_type_name() const
    {
        return RegionTypeName(region_type_);
    }
    void select_region();
    std::shared_ptr<Image> get_region();
    std::shared_ptr<Image> get_data();
    [[nodiscard]] ImVec2 get_position() const;
    [[nodiscard]] std::shared_ptr<Poisson> get_poisson() const;

   private:
    ImVec2 start_, end_;
    std::vector<ImVec2> poly_points_;
    bool flag_enable_selecting_region_ = false;
    bool draw_status_ = false;
    RegionType region_type_ = RegionType::kRect;
    std::shared_ptr<Image> selected_region_;
    std::shared_ptr<Poisson> poisson_;
};

}  // namespace USTC_CG