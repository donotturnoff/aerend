#ifndef WIDGET_H
#define WIDGET_H

#include "GUIException.h"
#include "bitmap/SimpleBitmap.h"
#include "bitmap/Bitmap.h"
#include "style/Colour.h"
#include "style/Border.h"
#include "style/Margin.h"
#include "style/Padding.h"
#include <cstdint>
#include <memory>

namespace aerend {

class Window;
class Container;

class Widget : std::enable_shared_from_this<Widget> {
public:
    Widget();

    void set_x(const int32_t x) noexcept;
    void set_y(const int32_t y) noexcept;
    virtual void set_pos(const int32_t x, const int32_t y) noexcept;
    void set_w(const int32_t w);
    void set_h(const int32_t h);
    virtual void set_size(const int32_t w, const int32_t h);
    void set_preferred_w(const int32_t preferred_w);
    void set_preferred_h(const int32_t preferred_h);
    virtual void set_preferred_size(const int32_t preferred_w, const int32_t preferred_h);
    void set_full_w(const int32_t full_w);
    void set_full_h(const int32_t full_h);
    virtual void set_full_size(const int32_t full_w, const int32_t full_h);
    virtual void set_root(Window* root) noexcept;
    void set_parent(Container* parent) noexcept;
    void set_autorepaint(bool should_autorepaint) noexcept;
    void set_autolayout(bool should_autolayout) noexcept;
    virtual void set_bg_colour(Colour colour) noexcept;
    virtual void set_border(Border border) noexcept;
    virtual void set_margin(Margin margin) noexcept;
    virtual void set_padding(Padding padding) noexcept;

    int32_t get_x() const noexcept;
    int32_t get_y() const noexcept;
    int32_t get_w() const noexcept;
    int32_t get_h() const noexcept;
    int32_t get_preferred_w() const noexcept;
    int32_t get_preferred_h() const noexcept;
    int32_t get_full_w() const noexcept;
    int32_t get_full_h() const noexcept;
    int32_t get_preferred_full_w() const noexcept;
    int32_t get_preferred_full_h() const noexcept;
    Window* get_root() const noexcept;
    Container* get_parent() const noexcept;
    Border get_border() const noexcept;
    Margin get_margin() const noexcept;
    Padding get_padding() const noexcept;

    void autorepaint();
    void autolayout();
    virtual void repaint();
    virtual void repaint(bool direct);
    virtual void layout();
    virtual void paint(Bitmap& bmp) = 0;
protected:
    bool should_autorepaint, should_autolayout;
    Window* root;
    Container* parent;
    Padding padding;
    Border border;
    Margin margin;
    Colour bg_colour;
    int32_t x, y, w, h, preferred_w, preferred_h, full_w, full_h;
};

}

#endif // WIDGET_H

