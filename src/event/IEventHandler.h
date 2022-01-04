#ifndef I_EVENT_HANDLER_H
#define I_EVENT_HANDLER_H

#include "Event.h"
#include <memory>

namespace aerend {

enum class EventType;

struct IEventHandler {
    IEventHandler(EventType type);
    virtual void handle(std::shared_ptr<Event> event);
    const EventType type;
};

}

#endif // I_EVENT_HANDLER_H

