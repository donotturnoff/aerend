#ifndef KEY_PRESS_EVENT_H
#define KEY_PRESS_EVENT_H

#include "KeyEvent.h"

namespace aerend {

struct KeyPressEvent : public KeyEvent {
    KeyPressEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn, bool repeated);
    virtual EventType get_type();
    static const EventType type = EventType::KEY_PRESS;
};

}

#endif // KEY_PRESS_EVENT_H

