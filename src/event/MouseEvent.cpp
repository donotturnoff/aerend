#include "MouseEvent.h"

namespace aerend {

MouseEvent::MouseEvent(EventType type, int32_t x, int32_t y, int32_t dx, int32_t dy, int32_t scroll_x, int32_t scroll_y, bool left, bool middle, bool right) : x(x), y(y), dx(dx), dy(dy), scroll_x(scroll_x), scroll_y(scroll_y), left(left), middle(middle), right(right) {
    this->type = type;
}

int32_t MouseEvent::get_x() const noexcept {
    return x;
}

int32_t MouseEvent::get_y() const noexcept {
    return y;
}

int32_t MouseEvent::get_dx() const noexcept {
    return dx;
}

int32_t MouseEvent::get_dy() const noexcept {
    return dy;
}

int32_t MouseEvent::get_scroll_x() const noexcept {
    return scroll_x;
}

int32_t MouseEvent::get_scroll_y() const noexcept {
    return scroll_y;
}

bool MouseEvent::get_left() const noexcept {
    return left;
}

bool MouseEvent::get_middle() const noexcept {
    return middle;
}

bool MouseEvent::get_right() const noexcept {
    return right;
}

}
