#include "WindowLayout.h"
#include <iostream>

namespace aerend {

WindowLayout::WindowLayout() : component(0) {}

void WindowLayout::place(Container& parent, Widget& child) {
    int32_t padding = parent.get_padding().t;
    if (component == 0) { // Title bar
        child.set_pos(padding, padding);
        // TODO: make title bar height customiseable
        child.set_size(parent.get_w()-2*padding, 14);
    } else if (component == 1) { // Frame
        child.set_pos(padding, 14+padding);
        child.set_size(parent.get_w()-2*padding, parent.get_h() - 14 - 2*padding);
    }
    component++;
}

void WindowLayout::reset() {
    component = 0;
}

}
