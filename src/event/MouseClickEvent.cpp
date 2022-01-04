#include "MouseClickEvent.h"

namespace aerend {

MouseClickEvent::MouseClickEvent(Widget* widget, int32_t x, int32_t y, bool left, bool middle, bool right) : MouseEvent(x, y, 0, 0, 0, 0, left, middle, right), widget(widget) {}

EventType MouseClickEvent::get_type() {
    return type;
}

}
