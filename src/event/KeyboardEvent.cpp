#include "KeyboardEvent.h"

namespace aerend {

KeyboardEvent::KeyboardEvent(EventType type, char c, bool shift, bool ctrl, bool alt, bool meta, bool fn) : Event(type), c(c), shift(shift), ctrl(ctrl), alt(alt), meta(meta), fn(fn) {}

char KeyboardEvent::get_char() const noexcept {
    return c;
}

bool KeyboardEvent::get_shift() const noexcept {
    return shift;
}

bool KeyboardEvent::get_ctrl() const noexcept {
    return ctrl;
}

bool KeyboardEvent::get_alt() const noexcept {
    return alt;
}

bool KeyboardEvent::get_meta() const noexcept {
    return meta;
}

bool KeyboardEvent::get_fn() const noexcept {
    return fn;
}

}
