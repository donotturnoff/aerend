#ifndef KEY_EVENT_H
#define KEY_EVENT_H

#include "Event.h"
#include <cstdint>

namespace aerend {

struct KeyEvent: public Event {
    KeyEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn, bool repeated);
    virtual EventType get_type();
    char c;
    bool shift, ctrl, alt, meta, fn, repeated;
};

}

#endif // KEY_EVENT_H

