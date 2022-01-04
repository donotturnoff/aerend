#ifndef MOUSE_CLICK_EVENT_H
#define MOUSE_CLICK_EVENT_H

#include "Event.h"
#include "MouseEvent.h"
#include "gui/Widget.h"
#include <cstdint>

namespace aerend {

// TODO: create WidgetEvent class for MouseClickEvent and KeyTypeEvent to extend?
struct MouseClickEvent: public MouseEvent {
    MouseClickEvent(Widget* widget, int32_t x, int32_t y, bool left, bool middle, bool right);
    virtual EventType get_type();
    static const EventType type = EventType::MOUSE_CLICK;
    const Widget* widget;
};

}

#endif // MOUSE_CLICK_EVENT_H

