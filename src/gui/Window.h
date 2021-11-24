#ifndef WINDOW_H
#define WINDOW_H

#include "Container.h"
#include "Panel.h"
#include "bitmap/SimpleBitmap.h"
#include "drm/DRMBitmap.h"
#include <cstdint>
#include <string>
#include <memory>

namespace aerend {

class Window : public Container {
public:
    Window(int32_t x = 0, int32_t y = 0, int32_t w = 100, int32_t h = 100, std::string title = "");
    void set_title(std::string title) noexcept;
    SimpleBitmap& get_bmp() noexcept;
    std::shared_ptr<Panel> get_frame() noexcept;
    void repaint();
    void paint(Bitmap& bmp);
private:
    SimpleBitmap bmp;
    std::shared_ptr<Panel> title_bar, frame;
    std::string title;
};

}

#endif // WINDOW_H

