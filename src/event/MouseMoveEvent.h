#ifndef MOUSE_MOVE_EVENT_H
#define MOUSE_MOVE_EVENT_H

#include "Event.h"
#include "MouseEvent.h"
#include "gui/Widget.h"
#include <cstdint>

namespace aerend {

struct MouseMoveEvent: public MouseEvent {
    MouseMoveEvent(int32_t x, int32_t y, int32_t dx, int32_t dy, bool left, bool middle, bool right);
    virtual EventType get_type();
    static const EventType type = EventType::MOUSE_MOVE;
};

}

#endif // MOUSE_MOVE_EVENT_H

