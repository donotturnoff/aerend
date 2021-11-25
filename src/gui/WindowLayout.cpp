#include "WindowLayout.h"
#include <iostream>

namespace aerend {

WindowLayout::WindowLayout() : component(0) {}

void WindowLayout::place(Container& parent, Widget& child) {
    if (component == 0) { // Title bar
        child.set_pos(0, 0);
        // TODO: make title bar height customiseable
        child.set_size(parent.get_w(), 14);
    } else if (component == 1) { // Frame
        child.set_pos(0, 14);
        child.set_size(parent.get_w(), parent.get_h() - 14);
    }
    component++;
}

void WindowLayout::reset() {

}

}
