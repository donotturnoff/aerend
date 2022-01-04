#ifndef KEY_TYPE_EVENT_H
#define KEY_TYPE_EVENT_H

#include "KeyEvent.h"

namespace aerend {

struct KeyTypeEvent : public KeyEvent {
    KeyTypeEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn);
    virtual EventType get_type();
    static const EventType type = EventType::KEY_TYPE;
};

}

#endif // KEY_TYPE_EVENT_H

