#ifndef EVENT_H
#define EVENT_H

#include "gui/Window.h"
#include "gui/Widget.h"

namespace aerend {

enum class EventType {
    KEY_PRESS, KEY_RELEASE, KEY_TYPE, MOUSE_PRESS, MOUSE_RELEASE, MOUSE_CLICK, MOUSE_MOVE, MOUSE_SCROLL, MAX_NUM=MOUSE_SCROLL
};

class Event {
public:
    EventType get_type() const noexcept;
    void set_window(Window* window) noexcept;
    void set_widget(Widget* widget) noexcept;
    Window* get_window() const noexcept;
    Widget* get_widget() const noexcept;

protected:
    EventType type;
    Window* window;
    Widget* widget;
};

}

#endif // EVENT_H

