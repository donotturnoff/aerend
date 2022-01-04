#include "MousePressEvent.h"

namespace aerend {

MousePressEvent::MousePressEvent(int32_t x, int32_t y, bool left, bool middle, bool right) : MouseEvent(x, y, 0, 0, 0, 0, left, middle, right) {}

EventType MousePressEvent::get_type() {
    return type;
}

}
