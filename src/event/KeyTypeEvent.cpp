#include "KeyTypeEvent.h"

namespace aerend {

KeyTypeEvent::KeyTypeEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn) : KeyEvent(c, shift, ctrl, alt, meta, fn, false) {}

EventType KeyTypeEvent::get_type() {
    return type;
}

}
