#pragma once

#include "comp_source_image.h"
#include "view/comp_image.h"

namespace USTC_CG
{
class CompTargetImage : public ImageEditor
{
   public:
    explicit CompTargetImage(const std::string& label, const std::string& filename);
    ~CompTargetImage() noexcept override = default;

    void draw() override;
    void set_source(std::shared_ptr<CompSourceImage> source);
    void set_realtime(bool flag);
    void restore();
    void set_clone_type(Poisson::CloneType type)
    {
        clone_type_ = type;
    }
    void clone();

   private:
    std::shared_ptr<Image> back_up_;
    std::shared_ptr<CompSourceImage> source_image_;
    Poisson::CloneType clone_type_ = Poisson::CloneType::kDefault;
    ImVec2 mouse_position_;
    bool edit_status_ = false;
    bool flag_realtime_updating = false;
};

}  // namespace USTC_CG