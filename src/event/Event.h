#ifndef EVENT_H
#define EVENT_H

#include <cstdint>

namespace aerend {

class Widget;

enum class EventType {
    HALT, KEY_PRESS, KEY_RELEASE, KEY_TYPE, MOUSE_PRESS, MOUSE_RELEASE, MOUSE_CLICK, MOUSE_MOVE, MOUSE_SCROLL, ACTION, MOUSE_ENTER, MOUSE_EXIT, MAX_NUM=MOUSE_EXIT
};

struct Event {
    const EventType type;
    virtual ~Event() = default;
protected:
    Event(EventType type);
};



struct HaltEvent : public Event {
    HaltEvent();
};

struct KeyEvent: public Event {
    const char c;
    const bool shift, ctrl, alt, meta, fn, repeated;
protected:
    KeyEvent(EventType type, char c, bool shift, bool ctrl, bool alt, bool meta, bool fn, bool repeated);
};

struct MouseEvent: public Event {
    const int32_t x, y, dx, dy, scroll_x, scroll_y;
    const bool left, middle, right;
protected:
    MouseEvent(EventType type, int32_t x, int32_t y, int32_t dx, int32_t dy, int32_t scroll_x, int32_t scroll_y, bool left, bool middle, bool right);
};

struct WidgetEvent: public Event {
    const Widget* widget;
protected:
    WidgetEvent(EventType type, Widget* widget);
};



struct KeyPressEvent : public KeyEvent {
    KeyPressEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn, bool repeated);
};

struct KeyReleaseEvent : public KeyEvent {
    KeyReleaseEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn);
};
struct KeyTypeEvent : public KeyEvent {
    KeyTypeEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn);
};

struct MouseClickEvent: public MouseEvent {
    MouseClickEvent(int32_t x, int32_t y, bool left, bool middle, bool right);
};

struct MouseMoveEvent: public MouseEvent {
    MouseMoveEvent(int32_t x, int32_t y, int32_t dx, int32_t dy, bool left, bool middle, bool right);
};

struct MousePressEvent: public MouseEvent {
    MousePressEvent(int32_t x, int32_t y, bool left, bool middle, bool right);
};

struct MouseReleaseEvent: public MouseEvent {
    MouseReleaseEvent(int32_t x, int32_t y, bool left, bool middle, bool right);
};

struct MouseScrollEvent: public MouseEvent {
    MouseScrollEvent(int32_t scroll_x, int32_t scroll_y, bool left, bool middle, bool right);
};

struct ActionEvent: public WidgetEvent {
    ActionEvent(Widget* widget);
};

struct MouseEnterEvent: public WidgetEvent {
    MouseEnterEvent(Widget* widget);
};

struct MouseExitEvent: public WidgetEvent {
    MouseExitEvent(Widget* widget);
};

}

#endif // EVENT_H

