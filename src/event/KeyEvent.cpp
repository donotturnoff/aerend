#include "KeyEvent.h"

namespace aerend {

KeyEvent::KeyEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn, bool repeated) : c(c), shift(shift), ctrl(ctrl), alt(alt), meta(meta), fn(fn), repeated(repeated) {}

EventType KeyEvent::get_type() {
    return type;
}

}
