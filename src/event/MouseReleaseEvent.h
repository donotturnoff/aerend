#ifndef MOUSE_RELEASE_EVENT_H
#define MOUSE_RELEASE_EVENT_H

#include "Event.h"
#include "MouseEvent.h"
#include <cstdint>

namespace aerend {

struct MouseReleaseEvent: public MouseEvent {
    MouseReleaseEvent(int32_t x, int32_t y, bool left, bool middle, bool right);
    virtual EventType get_type();
    static const EventType type = EventType::MOUSE_RELEASE;
};

}

#endif // MOUSE_RELEASE_EVENT_H

