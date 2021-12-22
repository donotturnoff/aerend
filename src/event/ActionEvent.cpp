#include "ActionEvent.h"

namespace aerend {

ActionEvent::ActionEvent(Widget* widget, bool left, bool middle, bool right) : widget(widget), left(left), middle(middle), right(right) {
    this->type = EventType::ACTION;
}

// TODO: make this a property of all events
Widget* ActionEvent::get_widget() const noexcept {
    return widget;
}

bool ActionEvent::get_left() const noexcept {
    return left;
}

bool ActionEvent::get_middle() const noexcept {
    return middle;
}

bool ActionEvent::get_right() const noexcept {
    return right;
}

}
