#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "IEventHandler.h"
#include "Action.h"
#include <memory>
#include <functional>

namespace aerend {

class Event;
enum class EventType;

template <typename E>
struct EventHandler : public IEventHandler {
    EventHandler(std::shared_ptr<Action<E>> action, std::function<bool(std::shared_ptr<E>)> cond);
    void handle(std::shared_ptr<E> event);
    std::function<bool(std::shared_ptr<E>)> cond;
    std::shared_ptr<Action<E>> action;
};

}

#include "EventHandler.tpp"

#endif // EVENT_HANDLER_H

