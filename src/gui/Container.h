#ifndef CONTAINER_H
#define CONTAINER_H

#include "Widget.h"
#include "LayoutManager.h"
#include "bitmap/Bitmap.h"
#include <memory>
#include <vector>

namespace aerend {

class Window;

class Container : public Widget {
public:
    void set_lm(std::shared_ptr<LayoutManager> lm) noexcept;
    void set_root(Window* root) noexcept;
    void add(std::shared_ptr<Widget> child);
    void rm(std::shared_ptr<Widget> child);
    std::shared_ptr<Widget> get_child(const int32_t i) const noexcept;
    virtual void repaint();
    virtual void repaint(bool direct);
    virtual void layout();
    virtual void paint(Bitmap& bmp);
protected:
    std::shared_ptr<LayoutManager> lm;
    std::vector<std::shared_ptr<Widget>> children;
};

}

#endif // CONTAINER_H

