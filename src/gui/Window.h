#ifndef WINDOW_H
#define WINDOW_H

#include "Container.h"
#include "Panel.h"
#include "Label.h"
#include "WidgetMap.h"
#include "bitmap/SimpleBitmap.h"
#include "bitmap/DRMBitmap.h"
#include "text/Font.h"
#include <cstdint>
#include <string>
#include <memory>

namespace aerend {

class Window : public Container {
public:
    Window(Client& client, int32_t x = 0, int32_t y = 0, int32_t w = 100, int32_t h = 100, std::string title = "");
    void set_title(std::string title) noexcept;
    void set_pos(const int32_t x, const int32_t y) noexcept;
    void set_size(const int32_t w, const int32_t h);
    std::string get_title() const noexcept;
    SimpleBitmap& get_bmp() noexcept;
    WidgetMap& get_wmp() noexcept;
    Widget* get_widget_at(int32_t x, int32_t y);
    void open();
    void close();
    void bump();
    void add(Widget* child);
    void rm(Widget* child);
    void repaint();
    void repaint(bool direct);
    void paint(Bitmap& bmp);
    void map_widget(Widget* widget);
private:
    static const char* WIN_TITLE_FONT_PATH;
    static const int32_t WIN_TITLE_FONT_SIZE;
    Panel* title_bar;
    Panel* frame;
    Label* title_label;
    SimpleBitmap bmp;
    WidgetMap wmp;
    std::string title;
    bool draggable;
};

}

#endif // WINDOW_H

