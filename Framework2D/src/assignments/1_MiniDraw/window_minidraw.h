#pragma once

#include <memory>

#include "view/comp_canvas.h"
#include "view/window.h"

namespace USTC_CG
{
class MiniDraw : public Window
{
   public:
    explicit MiniDraw(const std::string& window_name);
    ~MiniDraw() override = default;

    void draw() override;

   private:
    void draw_canvas();

    std::shared_ptr<Canvas> p_canvas_ = nullptr;

    bool flag_show_canvas_view_ = true;
};
}  // namespace USTC_CG