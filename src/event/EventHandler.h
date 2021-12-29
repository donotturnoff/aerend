#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <memory>

namespace aerend {

class Event;
enum class EventType;

class EventHandler {
public:
    EventHandler(EventType type);
    void handle(std::shared_ptr<Event> event);
    EventType get_type();
private:
    EventType type;
};

}

#endif // EVENT_HANDLER_H

