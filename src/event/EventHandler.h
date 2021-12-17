#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "Event.h"
#include "gui/Widget.h"
#include "gui/Window.h"
#include <memory>

namespace aerend {

class EventHandler {
public:
    EventHandler(EventType type, Widget* widget);
    EventHandler(EventType type, Window* window);
    void handle(std::shared_ptr<Event> event);
    EventType get_type();
    Widget* get_widget();
    Window* get_window();
private:
    EventType type;
    Widget* widget;
    Window* window;
};

}

#endif // EVENT_HANDLER_H

