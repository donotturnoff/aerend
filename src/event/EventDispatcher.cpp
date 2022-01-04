#include "EventDispatcher.h"
#include "AerendServer.h"
#include "event/MouseClickEvent.h"
#include "event/MouseMoveEvent.h"
#include "event/MouseReleaseEvent.h"
#include "event/MouseScrollEvent.h"
#include "event/MousePressEvent.h"
#include "event/KeyTypeEvent.h"
#include "event/KeyPressEvent.h"
#include "event/KeyReleaseEvent.h"
#include "gui/MergedUpdates.h"
#include <iostream>
#include <cstdio>

namespace aerend {

EventDispatcher::EventDispatcher() : running(true) {
    thread = std::thread(&EventDispatcher::run, this);
}

EventDispatcher::~EventDispatcher() {
    running.store(false);
    push_event(std::make_shared<Event>());
    thread.join();
}

void EventDispatcher::push_event(std::shared_ptr<Event> event) {
    std::lock_guard<std::mutex> lock{q_mtx};
    queue.push(event);
    q_cond.notify_one();
}

std::shared_ptr<Event> EventDispatcher::pop_event() {
    std::unique_lock<std::mutex> lock{q_mtx};
    q_cond.wait(lock, [&]{ return !queue.empty(); });
    std::shared_ptr<Event> event = queue.front();
    queue.pop();
    return event;
}

void EventDispatcher::run() {
    while (running.load()) {
        auto event = pop_event();
        EventType type = event->get_type();

        if (type == EventType::HALT) {
            continue;
        } else if (type == EventType::MOUSE_MOVE) {
            // TODO: alter this? Use EventHandlers/create similar?
            auto mme = (MouseMoveEvent*) event.get();
            int32_t dx = mme->dx;
            int32_t dy = mme->dy;
            auto update = [dx, dy] () { AerendServer::the().get_display_manager().merged_updates->move_cursor(dx, dy); };
            AerendServer::the().get_display_manager().push_update(update);
        }

        auto widgets = AerendServer::the().get_display_manager().get_widgets(event);

        for (const auto& widget: widgets) {
            auto handlers = widget->get_event_handlers(type);
            for (const auto& handler: handlers) {
                auto h = handler.get();
                switch (type) {
                    case EventType::MOUSE_MOVE: ((EventHandler<MouseMoveEvent>*) h)->handle(std::dynamic_pointer_cast<MouseMoveEvent>(event)); break;
                    case EventType::MOUSE_PRESS: ((EventHandler<MousePressEvent>*) h)->handle(std::dynamic_pointer_cast<MousePressEvent>(event)); break;
                    case EventType::MOUSE_CLICK: ((EventHandler<MouseClickEvent>*) h)->handle(std::dynamic_pointer_cast<MouseClickEvent>(event)); break;
                    case EventType::MOUSE_RELEASE: ((EventHandler<MouseReleaseEvent>*) h)->handle(std::dynamic_pointer_cast<MouseReleaseEvent>(event)); break;
                    case EventType::MOUSE_SCROLL: ((EventHandler<MouseScrollEvent>*) h)->handle(std::dynamic_pointer_cast<MouseScrollEvent>(event)); break;
                    case EventType::KEY_TYPE: ((EventHandler<KeyTypeEvent>*) h)->handle(std::dynamic_pointer_cast<KeyTypeEvent>(event)); break;
                    case EventType::KEY_PRESS: ((EventHandler<KeyPressEvent>*) h)->handle(std::dynamic_pointer_cast<KeyPressEvent>(event)); break;
                    case EventType::KEY_RELEASE: ((EventHandler<KeyReleaseEvent>*) h)->handle(std::dynamic_pointer_cast<KeyReleaseEvent>(event)); break;
                    default: h->handle(event);
                }
            }
        }

        if (widgets.size() > 0) {
            if (type == EventType::MOUSE_RELEASE) {
                auto mre = (MouseReleaseEvent*) event.get();
                auto widget = widgets[0];
                // TODO: simplify creating MOUSE_CLICK from MOUSE_RELEASE
                auto click_event = std::make_shared<MouseClickEvent>(widget, mre->x, mre->y, mre->left, mre->middle, mre->right);
                auto handlers = widget->get_event_handlers(EventType::MOUSE_CLICK);
                for (const auto& handler: handlers) {
                    handler->handle(click_event);
                }
            } else if (type == EventType::KEY_RELEASE) {
                auto kre = (KeyReleaseEvent*) event.get();
                auto widget = widgets[0];
                auto type_event = std::make_shared<KeyTypeEvent>(kre->c, kre->shift, kre->ctrl, kre->alt, kre->meta, kre->fn);
                auto handlers = widget->get_event_handlers(EventType::KEY_TYPE);
                for (const auto& handler: handlers) {
                    handler->handle(type_event);
                }
            }
        }
    }
}

}
