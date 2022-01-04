#ifndef KEY_RELEASE_EVENT_H
#define KEY_RELEASE_EVENT_H

#include "KeyEvent.h"

namespace aerend {

struct KeyReleaseEvent : public KeyEvent {
    KeyReleaseEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn);
    virtual EventType get_type();
    static const EventType type = EventType::KEY_RELEASE;
};

}

#endif // KEY_RELEASE_EVENT_H

