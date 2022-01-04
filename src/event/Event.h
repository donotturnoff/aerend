#ifndef EVENT_H
#define EVENT_H

namespace aerend {

enum class EventType {
    NONE, KEY_PRESS, KEY_RELEASE, KEY_TYPE, MOUSE_PRESS, MOUSE_RELEASE, MOUSE_CLICK, MOUSE_MOVE, MOUSE_SCROLL, MAX_NUM=MOUSE_SCROLL
};

struct Event {
    virtual EventType get_type(); // Used to get EventType value from run-time type
    static const EventType type = EventType::NONE; // Used to get EventType value from compile-time type
};

}

#endif // EVENT_H

