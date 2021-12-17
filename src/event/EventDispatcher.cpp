#include "EventDispatcher.h"
#include <iostream>

namespace aerend {

void EventDispatcher::add_handler(std::shared_ptr<EventHandler> handler) {
    handlers[(int) (handler->get_type())].insert(handler);
}

void EventDispatcher::rm_handler(std::shared_ptr<EventHandler> handler) {
    auto event_handlers = handlers[(int) (handler->get_type())];
    event_handlers.erase(event_handlers.find(handler));
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
    running.store(true);
    while (running.load()) {
        std::shared_ptr<Event> event = pop_event();
        if (event == nullptr) {
            continue;
        }
        std::set<std::shared_ptr<EventHandler>> event_handlers = handlers[(int) (event->get_type())];
        for (const auto& handler: event_handlers) {
            Widget* event_widget = event->get_widget();
            Widget* handler_widget = handler->get_widget();
            //if (event_widget == handler_widget || (event_widget == nullptr && event->get_window() == handler_widget->get_root())) {
                handler->handle(event);
            //}
        }
    }
}

void EventDispatcher::stop() {
    running.store(false);
    push_event(std::make_shared<Event>());
}

}
