#ifndef EVENT_H
#define EVENT_H

namespace aerend {

enum class EventType {
    KEY_PRESS, KEY_RELEASE, KEY_TYPE, MOUSE_PRESS, MOUSE_RELEASE, MOUSE_CLICK, MOUSE_MOVE, MOUSE_SCROLL, ACTION, MAX_NUM=MOUSE_SCROLL
};

class Event {
public:
    EventType get_type() const noexcept;
protected:
    EventType type;
};

}

#endif // EVENT_H

