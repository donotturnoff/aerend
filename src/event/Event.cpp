#include "Event.h"
#include <iostream>

namespace aerend {

Event::Event(EventType type) : type(type) {}

EventType Event::get_type() const noexcept {
    return type;
}




HaltEvent::HaltEvent() : Event(EventType::HALT) {}





KeyEvent::KeyEvent(EventType type, char c, bool shift, bool ctrl, bool alt, bool meta, bool fn, bool repeated) : Event(type), c(c), keys((shift<<5) | (ctrl<<4) | (alt<<3) | (meta<<2) | (fn<<1) | (repeated)) {}

char KeyEvent::get_char() const noexcept {
    return c;
}

bool KeyEvent::is_shift_down() const noexcept {
    return keys & 0x20;
}

bool KeyEvent::is_ctrl_down() const noexcept {
    return keys & 0x10;
}

bool KeyEvent::is_alt_down() const noexcept {
    return keys & 0x08;
}

bool KeyEvent::is_meta_down() const noexcept {
    return keys & 0x04;
}

bool KeyEvent::is_fn_down() const noexcept {
    return keys & 0x02;
}

bool KeyEvent::is_repeated() const noexcept {
    return keys & 0x01;
}

uint8_t KeyEvent::get_flags() const noexcept {
    return keys;
}

KeyPressEvent::KeyPressEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn, bool repeated) : KeyEvent(EventType::KEY_PRESS, c, shift, ctrl, alt, meta, fn, repeated) {}

KeyReleaseEvent::KeyReleaseEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn) : KeyEvent(EventType::KEY_RELEASE, c, shift, ctrl, alt, meta, fn, false) {}

KeyTypeEvent::KeyTypeEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn) : KeyEvent(EventType::KEY_TYPE, c, shift, ctrl, alt, meta, fn, false) {}

KeyTypeEvent::KeyTypeEvent(Event* event) : KeyTypeEvent(event->get_char(), event->is_shift_down(), event->is_ctrl_down(), event->is_alt_down(), event->is_meta_down(), event->is_fn_down()) {}






MouseEvent::MouseEvent(EventType type, int16_t dx, int16_t dy, bool left, bool middle, bool right) : Event(type), dx(dx), dy(dy), buttons((left << 2) | (middle << 1) | (right)) {}

bool MouseEvent::is_left_down() const noexcept {
    return buttons & 0x4;
}

bool MouseEvent::is_middle_down() const noexcept {
    return buttons & 0x2;
}

bool MouseEvent::is_right_down() const noexcept {
    return buttons & 0x1;
}

uint8_t MouseEvent::get_flags() const noexcept {
    return buttons;
}

MouseClickEvent::MouseClickEvent(bool left, bool middle, bool right) : MouseEvent(EventType::MOUSE_CLICK, 0, 0, left, middle, right) {}

MouseClickEvent::MouseClickEvent(Event* event) : MouseClickEvent(event->is_left_down(), event->is_middle_down(), event->is_right_down()) {}

MouseMoveEvent::MouseMoveEvent(int16_t dx, int16_t dy, bool left, bool middle, bool right) : MouseEvent(EventType::MOUSE_MOVE, dx, dy, left, middle, right) {}

int16_t MouseMoveEvent::get_dx() const noexcept {
    return dx;
}

int16_t MouseMoveEvent::get_dy() const noexcept {
    return dy;
}

MousePressEvent::MousePressEvent(bool left, bool middle, bool right) : MouseEvent(EventType::MOUSE_PRESS, 0, 0, left, middle, right) {}

MouseReleaseEvent::MouseReleaseEvent(bool left, bool middle, bool right) : MouseEvent(EventType::MOUSE_RELEASE, 0, 0, left, middle, right) {}

MouseScrollEvent::MouseScrollEvent(int16_t dx, int16_t dy, bool left, bool middle, bool right) : MouseEvent(EventType::MOUSE_SCROLL, dx, dy, left, middle, right) {}

int16_t MouseScrollEvent::get_dx() const noexcept {
    return dx;
}

int16_t MouseScrollEvent::get_dy() const noexcept {
    return dy;
}





WidgetEvent::WidgetEvent(EventType type, Widget* widget) : Event(type), widget(widget) {}

Widget* WidgetEvent::get_widget() const noexcept {
    return widget;
}

ActionEvent::ActionEvent(Widget* widget) : WidgetEvent(EventType::ACTION, widget) {}

MouseEnterEvent::MouseEnterEvent(Widget* widget) : WidgetEvent(EventType::MOUSE_ENTER, widget) {}

MouseExitEvent::MouseExitEvent(Widget* widget) : WidgetEvent(EventType::MOUSE_EXIT, widget) {}

}
