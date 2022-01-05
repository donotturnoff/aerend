#include "EventDispatcher.h"
#include "AerendServer.h"
#include "gui/MergedUpdates.h"
#include "gui/Button.h"
#include <iostream>
#include <cstdio>

namespace aerend {

EventDispatcher::EventDispatcher() : running(true), last_under_mouse(nullptr) {
    thread = std::thread(&EventDispatcher::run, this);
}

EventDispatcher::~EventDispatcher() {
    running.store(false);
    push_event(std::make_shared<HaltEvent>());
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
        EventType type = event->type;

        if (type == EventType::HALT) {
            break;
        } else if (type == EventType::MOUSE_MOVE) {
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
                handler(event);
            }
        }

        if (type == EventType::MOUSE_MOVE) {
            auto widget = widgets.size() > 0 ? widgets[0] : nullptr;
            if (widget != last_under_mouse) {
                if (last_under_mouse) {
                    auto mouse_exit_event = std::make_shared<MouseExitEvent>(last_under_mouse);
                    auto last_handlers = last_under_mouse->get_event_handlers(EventType::MOUSE_EXIT);
                    for (const auto& handler: last_handlers) {
                        handler(mouse_exit_event);
                    }
                }
                if (widget) {
                    auto mouse_enter_event = std::make_shared<MouseEnterEvent>(widget);
                    auto widget_handlers = widget->get_event_handlers(EventType::MOUSE_ENTER);
                    for (const auto& handler: widget_handlers) {
                        handler(mouse_enter_event);
                    }
                }

                last_under_mouse = widget;
            }
        }

        if (widgets.size() > 0) {
            if (type == EventType::MOUSE_RELEASE) {
                auto mre = (MouseReleaseEvent*) event.get();
                auto widget = widgets[0];
                // TODO: simplify creating MOUSE_CLICK from MOUSE_RELEASE
                auto click_event = std::make_shared<MouseClickEvent>(mre->x, mre->y, mre->left, mre->middle, mre->right);
                auto handlers = widget->get_event_handlers(EventType::MOUSE_CLICK);
                for (const auto& handler: handlers) {
                    handler(click_event);
                }
                Button* btn = dynamic_cast<Button*>(widget);
                if (btn) {
                    auto action_event = std::make_shared<ActionEvent>(btn);
                    auto handlers = btn->get_event_handlers(EventType::ACTION);
                    for (const auto& handler: handlers) {
                        handler(action_event);
                    }
                }
            } else if (type == EventType::KEY_RELEASE) {
                auto kre = (KeyReleaseEvent*) event.get();
                auto widget = widgets[0];
                auto type_event = std::make_shared<KeyTypeEvent>(kre->c, kre->shift, kre->ctrl, kre->alt, kre->meta, kre->fn);
                auto handlers = widget->get_event_handlers(EventType::KEY_TYPE);
                for (const auto& handler: handlers) {
                    handler(type_event);
                }
            }
        }
    }
}

}
