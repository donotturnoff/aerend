#ifndef KEYBOARD_EVENT_H
#define KEYBOARD_EVENT_H

#include "Event.h"
#include <cstdint>

namespace aerend {

class KeyboardEvent: public Event {
public:
    KeyboardEvent(EventType type, char c, bool shift, bool ctrl, bool alt, bool meta, bool fn, bool repeated);
    char get_char() const noexcept;
    bool get_shift() const noexcept;
    bool get_ctrl() const noexcept;
    bool get_alt() const noexcept;
    bool get_meta() const noexcept;
    bool get_fn() const noexcept;
    bool is_repeated() const noexcept;
private:
    char c;
    bool shift, ctrl, alt, meta, fn, repeated;
};

}

#endif // KEYBOARD_EVENT_H

