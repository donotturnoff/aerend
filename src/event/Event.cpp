#include "Event.h"

namespace aerend {

Event::Event(EventType type) : type(type) {}



HaltEvent::HaltEvent() : Event(EventType::HALT) {}

KeyEvent::KeyEvent(EventType type, char c, bool shift, bool ctrl, bool alt, bool meta, bool fn, bool repeated) : Event(type), c(c), shift(shift), ctrl(ctrl), alt(alt), meta(meta), fn(fn), repeated(repeated) {}

MouseEvent::MouseEvent(EventType type, int32_t x, int32_t y, int32_t dx, int32_t dy, int32_t scroll_x, int32_t scroll_y, bool left, bool middle, bool right) : Event(type), x(x), y(y), dx(dx), dy(dy), scroll_x(scroll_x), scroll_y(scroll_y), left(left), middle(middle), right(right) {}



KeyPressEvent::KeyPressEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn, bool repeated) : KeyEvent(EventType::KEY_PRESS, c, shift, ctrl, alt, meta, fn, repeated) {}

KeyReleaseEvent::KeyReleaseEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn) : KeyEvent(EventType::KEY_RELEASE, c, shift, ctrl, alt, meta, fn, false) {}

KeyTypeEvent::KeyTypeEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn) : KeyEvent(EventType::KEY_TYPE, c, shift, ctrl, alt, meta, fn, false) {}

MouseClickEvent::MouseClickEvent(int32_t x, int32_t y, bool left, bool middle, bool right) : MouseEvent(EventType::MOUSE_CLICK, x, y, 0, 0, 0, 0, left, middle, right) {}

MouseMoveEvent::MouseMoveEvent(int32_t x, int32_t y, int32_t dx, int32_t dy, bool left, bool middle, bool right) : MouseEvent(EventType::MOUSE_MOVE, x, y, dx, dy, 0, 0, left, middle, right) {}

MousePressEvent::MousePressEvent(int32_t x, int32_t y, bool left, bool middle, bool right) : MouseEvent(EventType::MOUSE_PRESS, x, y, 0, 0, 0, 0, left, middle, right) {}

MouseReleaseEvent::MouseReleaseEvent(int32_t x, int32_t y, bool left, bool middle, bool right) : MouseEvent(EventType::MOUSE_RELEASE, x, y, 0, 0, 0, 0, left, middle, right) {}

MouseScrollEvent::MouseScrollEvent(int32_t scroll_x, int32_t scroll_y, bool left, bool middle, bool right) : MouseEvent(EventType::MOUSE_SCROLL, 0, 0, 0, 0, scroll_x, scroll_y, left, middle, right) {}

}
