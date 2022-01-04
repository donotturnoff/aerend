#include "MouseMoveEvent.h"

namespace aerend {

MouseMoveEvent::MouseMoveEvent(int32_t x, int32_t y, int32_t dx, int32_t dy, bool left, bool middle, bool right) : MouseEvent(x, y, dx, dy, 0, 0, left, middle, right) {}

EventType MouseMoveEvent::get_type() {
    return type;
}

}
