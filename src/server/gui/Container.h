#ifndef CONTAINER_H
#define CONTAINER_H

#include "Widget.h"
#include "LayoutManager.h"
#include "GridLayout.h"
#include "bitmap/Bitmap.h"
#include <memory>
#include <vector>

namespace aerend {

class Window;

class Container : public Widget {
public:
    Container(Client& client, std::unique_ptr<LayoutManager> lm, Colour bg_colour, Border border, Margin margin, Padding padding);
    ~Container();
    void set_lm(std::unique_ptr<LayoutManager> lm) noexcept;
    void set_root(Window* root) noexcept;
    virtual void add(Widget* child);
    virtual void rm(Widget* child);
    virtual void rm(uint32_t index);
    virtual void repaint();
    virtual void repaint(bool direct);
    virtual void layout();
    virtual void paint(Bitmap& bmp); // TODO: make this abstract
    void get_widgets_at(std::vector<Widget*>& widgets, int32_t x, int32_t y) noexcept;
protected:
    std::unique_ptr<LayoutManager> lm;
    std::vector<Widget*> children;
};

}

#endif // CONTAINER_H

