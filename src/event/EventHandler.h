#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <memory>

namespace aerend {

class Widget;
class Window;
class Event;
enum class EventType;

class EventHandler {
public:
    EventHandler(EventType type, Widget* widget);
    void handle(std::shared_ptr<Event> event);
    EventType get_type();
    Widget* get_widget();
private:
    EventType type;
    Widget* widget;
};

}

#endif // EVENT_HANDLER_H

