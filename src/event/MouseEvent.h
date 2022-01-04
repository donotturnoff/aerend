#ifndef MOUSE_EVENT_H
#define MOUSE_EVENT_H

#include "Event.h"
#include <cstdint>

namespace aerend {

struct MouseEvent: public Event {
    MouseEvent(int32_t x, int32_t y, int32_t dx, int32_t dy, int32_t scroll_x, int32_t scroll_y, bool left, bool middle, bool right);
    virtual EventType get_type();
    const int32_t x, y, dx, dy, scroll_x, scroll_y;
    const bool left, middle, right;
};

}

#endif // MOUSE_EVENT_H

