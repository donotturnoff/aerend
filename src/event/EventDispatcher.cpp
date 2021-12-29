#include "EventDispatcher.h"
#include "AerendServer.h"
#include "event/MouseEvent.h"
#include "event/MouseClickEvent.h"
#include <iostream>

namespace aerend {

EventDispatcher::EventDispatcher() : running(true) {
    thread = std::thread(&EventDispatcher::run, this);
}

EventDispatcher::~EventDispatcher() {
    running.store(false);
    push_event(std::make_shared<Event>(EventType::HALT));
    thread.join();
}

void EventDispatcher::push_event(std::shared_ptr<Event> event) {
    q_mtx.lock();
    queue.push(event);
    q_mtx.unlock();
    q_cond.notify_one();
}

std::shared_ptr<Event> EventDispatcher::pop_event() {
    std::unique_lock<std::mutex> lock(q_cond_mtx);
    q_cond.wait(lock, [&]{ return !queue.empty(); });
    //q_mtx.lock();
    std::shared_ptr<Event> event = queue.front();
    queue.pop();
    //q_mtx.unlock();
    return event;
}

void EventDispatcher::run() {
    while (running.load()) {
        std::shared_ptr<Event> event = pop_event();
        if (event->get_type() == EventType::HALT) {
            continue;
        }

        // TODO: alter this? Use EventHandlers/create similar?
        if (event->get_type() == EventType::MOUSE_MOVE) {
            auto me = (MouseEvent*) event.get();
            auto update = std::make_shared<CursorUpdate>(UpdateType::CURSOR_MOVE, me->get_dx(), me->get_dy());
            AerendServer::the().get_display_manager().push_update(update);
        }

        auto widgets = AerendServer::the().get_display_manager().get_widgets(event);

        for (const auto& widget: widgets) {
            auto handlers = widget->get_event_handlers(event->get_type());
            for (const auto& handler: handlers) {
                handler->handle(event);
            }
        }

        if (widgets.size() > 0) {
            if (event->get_type() == EventType::MOUSE_RELEASE) {
                auto me = (MouseEvent*) event.get();
                auto widget = widgets[0];
                auto action_event = std::make_shared<MouseClickEvent>(widget, me->get_left(), me->get_middle(), me->get_right());
                std::set<std::shared_ptr<EventHandler>> handlers = widget->get_event_handlers(EventType::MOUSE_CLICK);
                for (const auto& handler: handlers) {
                    handler->handle(action_event);
                }
            }
            // TODO: spawn key type event
        }
    }
}

}
