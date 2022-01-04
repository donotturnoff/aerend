#include "MouseScrollEvent.h"

namespace aerend {

MouseScrollEvent::MouseScrollEvent(int32_t scroll_x, int32_t scroll_y, bool left, bool middle, bool right) : MouseEvent(0, 0, 0, 0, scroll_x, scroll_y, left, middle, right) {}

EventType MouseScrollEvent::get_type() {
    return type;
}

}
