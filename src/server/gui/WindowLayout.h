#ifndef WINDOW_LAYOUT_H
#define WINDOW_LAYOUT_H

#include "Container.h"
#include "Widget.h"
#include <cstdint>

namespace aerend {

class WindowLayout : public LayoutManager {
public:
    WindowLayout();
    void place(Container& parent, Widget& child);
    void reset();
    int32_t index_from_position(Container& parent, int32_t x, int32_t y);
private:
    int32_t component;
};

}

#endif // WINDOW_LAYOUT_H

