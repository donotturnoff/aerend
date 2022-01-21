#include "WindowLayout.h"
#include "Window.h"
#include <iostream>

namespace aerend {

WindowLayout::WindowLayout() : component(0) {}

void WindowLayout::place(Container& parent, Widget& child) {
    int32_t h = Window::def_title_bar_height;
    int32_t padding = parent.get_padding().t;
    if (component == 0) { // Title bar
        child.set_pos(padding, padding);
        child.set_size(parent.get_w()-2*padding, h);
    } else if (component == 1) { // Frame
        child.set_pos(padding, h+padding);
        child.set_size(parent.get_w()-2*padding, parent.get_h() - h - 2*padding);
    }
    component++;
}

void WindowLayout::reset() {
    component = 0;
}

}
