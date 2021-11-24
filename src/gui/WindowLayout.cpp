#include "WindowLayout.h"
#include <iostream>

namespace aerend {

WindowLayout::WindowLayout() : component(0) {}

void WindowLayout::place(Container& parent, Widget& child) {
    if (component == 0) { // Title bar
        child.set_pos(0, 0);
        child.set_size(parent.get_w(), 20);
    } else if (component == 1) { // Frame
        child.set_pos(0, 20);
        child.set_size(parent.get_w(), parent.get_h() - 20);
    }
    component++;
}

}
