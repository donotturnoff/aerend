#include "KeyReleaseEvent.h"

namespace aerend {

KeyReleaseEvent::KeyReleaseEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn) : KeyEvent(c, shift, ctrl, alt, meta, fn, false) {}

EventType KeyReleaseEvent::get_type() {
    return type;
}

}
