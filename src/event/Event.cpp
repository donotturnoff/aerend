#include "Event.h"

namespace aerend {

EventType Event::get_type() const noexcept {
    return type;
}

void Event::set_window(Window* window) noexcept {
    this->window = window;
    this->widget = nullptr;
}

void Event::set_widget(Widget* widget) noexcept {
    this->window = widget->get_root();
    this->widget = widget;
}

Window* Event::get_window() const noexcept {
    return window;
}

Widget* Event::get_widget() const noexcept {
    return widget;
}

}
