#ifndef WINDOW_H
#define WINDOW_H

#include "Container.h"
#include "Panel.h"
#include "Label.h"
#include "Button.h"
#include "bitmap/SimpleBitmap.h"
#include "bitmap/DRMBitmap.h"
#include "text/Font.h"
#include <cstdint>
#include <string>
#include <memory>

namespace aerend {

class Window : public Container {
public:
    Window(Client& client, int32_t x = Window::def_x, int32_t y = Window::def_y, int32_t w = Window::def_w, int32_t h = Window::def_h, std::string title = Window::def_title);
    ~Window();
    void set_title(std::string title) noexcept;
    void set_pos(const int32_t x, const int32_t y) noexcept;
    void set_size(const int32_t w, const int32_t h);
    std::string get_title() const noexcept;
    SimpleBitmap& get_bmp() noexcept;

    void open();
    void close();
    void bump();
    void add(Widget* child);
    void rm(Widget* child);
    void repaint();
    void repaint(bool direct);
    void paint(Bitmap& bmp);

    void get_widgets_at(std::vector<Widget*>& widgets, int32_t x, int32_t y) noexcept;

    static const int32_t def_x, def_y, def_w, def_h, def_title_bar_height, def_title_font_size;
    static const Colour def_bg_colour;
    static const Border def_border;
    static const Padding def_padding;
    static const std::string def_title, def_title_font_path;
private:
    static const char* WIN_TITLE_FONT_PATH;
    static const int32_t WIN_TITLE_FONT_SIZE;
    SimpleBitmap bmp;
    std::unique_ptr<Panel> title_bar;
    std::unique_ptr<Panel> frame;
    std::unique_ptr<Label> title_label;
    std::string title;
    bool draggable;
};

}

#endif // WINDOW_H

