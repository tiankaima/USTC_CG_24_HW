#pragma once

#include <memory>

#include "comp_source_image.h"
#include "comp_target_image.h"
#include "view/window.h"

namespace USTC_CG
{
class WindowPoisson : public Window
{
   public:
    explicit WindowPoisson(const std::string& window_name);
    ~WindowPoisson() override = default;

    void draw() override;

   private:
    void draw_toolbar();
    void draw_target();
    void draw_source();

    void draw_open_target_image_file_dialog();
    void draw_open_source_image_file_dialog();
    void draw_save_image_file_dialog();

    static void add_tooltips(const std::string& desc);

    std::shared_ptr<CompTargetImage> p_target_ = nullptr;
    std::shared_ptr<CompSourceImage> p_source_ = nullptr;

    bool flag_show_target_view_ = true;
    bool flag_show_source_view_ = true;
    bool flag_open_target_file_dialog_ = false;
    bool flag_open_source_file_dialog_ = false;
    bool flag_save_file_dialog_ = false;
};
}  // namespace USTC_CG