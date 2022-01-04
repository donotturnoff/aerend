#include "MouseEvent.h"

namespace aerend {

MouseEvent::MouseEvent(int32_t x, int32_t y, int32_t dx, int32_t dy, int32_t scroll_x, int32_t scroll_y, bool left, bool middle, bool right) : x(x), y(y), dx(dx), dy(dy), scroll_x(scroll_x), scroll_y(scroll_y), left(left), middle(middle), right(right) {}

EventType MouseEvent::get_type() {
    return type;
}

}
