#include "MouseClickEvent.h"

namespace aerend {

MouseClickEvent::MouseClickEvent(Widget* widget, bool left, bool middle, bool right) : Event(EventType::MOUSE_CLICK), widget(widget), left(left), middle(middle), right(right) {}

// TODO: make this a property of all events
Widget* MouseClickEvent::get_widget() const noexcept {
    return widget;
}

bool MouseClickEvent::get_left() const noexcept {
    return left;
}

bool MouseClickEvent::get_middle() const noexcept {
    return middle;
}

bool MouseClickEvent::get_right() const noexcept {
    return right;
}

}
