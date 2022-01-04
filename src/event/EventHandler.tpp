#ifndef EVENT_HANDLER_TPP
#define EVENT_HANDLER_TPP

#include "EventHandler.h"
#include <iostream>

namespace aerend {

template <typename E>
EventHandler<E>::EventHandler(std::shared_ptr<Action<E>> action, std::function<bool(std::shared_ptr<E>)> cond) : IEventHandler(E::type), action(action), cond(cond) {}

template <typename E>
void EventHandler<E>::handle(std::shared_ptr<E> event) {
    if (cond(event)) {
        action->execute(event);
    }
}

}

#endif // EVENT_HANDLER_TPP
