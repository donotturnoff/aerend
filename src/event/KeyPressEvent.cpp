#include "KeyPressEvent.h"

namespace aerend {

KeyPressEvent::KeyPressEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn, bool repeated) : KeyEvent(c, shift, ctrl, alt, meta, fn, repeated) {}

EventType KeyPressEvent::get_type() {
    return type;
}

}
