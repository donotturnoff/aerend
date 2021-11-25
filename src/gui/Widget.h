#ifndef WIDGET_H
#define WIDGET_H

#include "GUIException.h"
#include "bitmap/SimpleBitmap.h"
#include "bitmap/Bitmap.h"
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
    virtual void set_root(Window* root) noexcept;
    void set_parent(Container* parent) noexcept;
    void set_autorepaint(bool should_autorepaint) noexcept;
    void set_autolayout(bool should_autolayout) noexcept;
    int32_t get_x() const noexcept;
    int32_t get_y() const noexcept;
    int32_t get_w() const noexcept;
    int32_t get_h() const noexcept;
    Window* get_root() const noexcept;
    Container* get_parent() const noexcept;
    void autorepaint();
    void autolayout();
    virtual void repaint();
    virtual void layout();
    virtual void paint(Bitmap& bmp) = 0;
protected:
    bool should_autorepaint, should_autolayout;
    Window* root;
    Container* parent;
    int32_t x, y, w, h;
};

}

#endif // WIDGET_H

