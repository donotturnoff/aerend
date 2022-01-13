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
    Container(Client& client, std::unique_ptr<LayoutManager> lm = std::move(std::make_unique<GridLayout>()), Colour bg_colour=Colour::white(), Border border=Border{}, Margin margin=Margin{}, Padding padding=Padding{});
    void set_lm(std::unique_ptr<LayoutManager> lm) noexcept;
    void set_root(Window* root) noexcept;
    virtual void add(Widget* child);
    virtual void rm(Widget* child);
    virtual void repaint();
    virtual void repaint(bool direct);
    virtual void layout();
    virtual void paint(Bitmap& bmp);
protected:
    std::unique_ptr<LayoutManager> lm;
    std::vector<Widget*> children;
};

}

#endif // CONTAINER_H

