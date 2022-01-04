#ifndef MOUSE_PRESS_EVENT_H
#define MOUSE_PRESS_EVENT_H

#include "Event.h"
#include "MouseEvent.h"
#include <cstdint>

namespace aerend {

struct MousePressEvent: public MouseEvent {
    MousePressEvent(int32_t x, int32_t y, bool left, bool middle, bool right);
    virtual EventType get_type();
    static const EventType type = EventType::MOUSE_PRESS;
};

}

#endif // MOUSE_PRESS_EVENT_H

