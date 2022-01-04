#ifndef MOUSE_SCROLL_EVENT_H
#define MOUSE_SCROLL_EVENT_H

#include "Event.h"
#include "MouseEvent.h"
#include <cstdint>

namespace aerend {

struct MouseScrollEvent: public MouseEvent {
    MouseScrollEvent(int32_t scroll_x, int32_t scroll_y, bool left, bool middle, bool right);
    virtual EventType get_type();
    static const EventType type = EventType::MOUSE_SCROLL;
};

}

#endif // MOUSE_SCROLL_EVENT_H

