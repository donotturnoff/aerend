#ifndef WINDOW_H
#define WINDOW_H

#include "Container.h"
#include "Panel.h"
#include "bitmap/SimpleBitmap.h"
#include "drm/DRMBitmap.h"
#include "text/Font.h"
#include <cstdint>
#include <string>
#include <memory>

namespace aerend {

class Window : public Container {
public:
    Window(int32_t x = 0, int32_t y = 0, int32_t w = 100, int32_t h = 100, std::string title = "");
    void set_title(std::string title) noexcept;
    std::string get_title() const noexcept;
    SimpleBitmap& get_bmp() noexcept;
    std::shared_ptr<Panel> get_title_bar() noexcept;
    std::shared_ptr<Panel> get_frame() noexcept;
    void bump();
    void repaint();
    void paint(Bitmap& bmp);
private:
    static const char* WIN_TITLE_FONT_PATH;
    static const int32_t WIN_TITLE_FONT_SIZE;
    SimpleBitmap bmp;
    std::string title;
};

}

#endif // WINDOW_H

