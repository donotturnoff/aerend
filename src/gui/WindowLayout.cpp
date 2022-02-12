#include "WindowLayout.h"
#include "Window.h"
#include <iostream>

namespace aerend {

WindowLayout::WindowLayout() : component(0) {}

void WindowLayout::place(Container& parent, Widget& child) {
    int32_t h = Window::def_title_bar_height;
    int32_t extra = parent.get_padding().t + parent.get_border().t;
    if (component == 0) { // Title bar
        child.set_pos(extra, extra);
        child.set_size(parent.get_w()-2*extra, h);
    } else if (component == 1) { // Frame
        child.set_pos(extra, h+extra);
        child.set_size(parent.get_w()-2*extra, parent.get_h() - h - 2*extra);
    }
    component++;
}

void WindowLayout::reset() {
    component = 0;
}

}
