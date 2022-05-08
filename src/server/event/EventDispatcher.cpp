#include "EventDispatcher.h"
#include "AerendServer.h"
#include "gui/MergedUpdates.h"
#include "gui/Button.h"
#include <iostream>
#include <cstdio>

namespace aerend {

EventDispatcher::EventDispatcher() {
    thread = std::thread(&EventDispatcher::run, this);
}

EventDispatcher::~EventDispatcher() {
    running.store(false);
    push_event(std::make_unique<HaltEvent>());
    thread.join();
}

void EventDispatcher::rm_from_under_mouse(Widget* widget) {
    if (widget == last_under_mouse) {
        last_under_mouse = nullptr;
    }
}

void EventDispatcher::push_event(std::shared_ptr<Event> event) {
    std::lock_guard<std::mutex> lock{q_mtx};
    queue.push(std::move(event));
    q_cond.notify_one();
}

std::shared_ptr<Event> EventDispatcher::pop_event() {
    // TODO: pop multiple events?
    std::unique_lock<std::mutex> lock{q_mtx};
    q_cond.wait(lock, [&]{ return !queue.empty(); });
    auto event{queue.front()};
    queue.pop();
    return event;
}

void EventDispatcher::run() {
    while (running.load()) {
        auto event_s{pop_event()};
        auto event{event_s.get()};
        auto type{event->get_type()};

        if (type == EventType::HALT) {
            break;
        } else if (type == EventType::MOUSE_MOVE) {
            int32_t dx{event->get_dx()};
            int32_t dy{event->get_dy()};
            auto update{ [dx, dy] () { AerendServer::the().dm().merged_updates->move_cursor(dx, dy); } };
            AerendServer::the().dm().push_update(update);
        }

        auto widgets{AerendServer::the().dm().get_widgets(event)};

        for (const auto& widget: widgets) {
            auto handlers{widget->get_event_handlers(type)};
            event->set_source(widget);
            for (const auto& handler: handlers) {
                handler(event);
            }
        }

        if (type == EventType::MOUSE_MOVE) {
            auto widget{widgets.size() > 0 ? widgets[0] : nullptr};
            if (widget != last_under_mouse) {
                if (last_under_mouse) {
                    MouseExitEvent mee{last_under_mouse};
                    auto last_handlers{last_under_mouse->get_event_handlers(EventType::MOUSE_EXIT)};
                    for (const auto& handler: last_handlers) {
                        handler(&mee);
                    }
                }
                if (widget) {
                    MouseEnterEvent mee{widget};
                    auto widget_handlers{widget->get_event_handlers(EventType::MOUSE_ENTER)};
                    for (const auto& handler: widget_handlers) {
                        handler(&mee);
                    }
                }

                last_under_mouse = widget;
            }
        }

        if (widgets.size() > 0) {
            if (type == EventType::MOUSE_RELEASE) {
                auto widget{widgets[0]};
                MouseClickEvent mce{event};
                mce.set_source(widget);
                auto handlers{widget->get_event_handlers(EventType::MOUSE_CLICK)};
                for (const auto& handler: handlers) {
                    handler(&mce);
                }
                Button* btn{dynamic_cast<Button*>(widget)};
                if (btn) {
                    ActionEvent ae{btn};
                    auto handlers{btn->get_event_handlers(EventType::ACTION)};
                    for (const auto& handler: handlers) {
                        handler(&ae);
                    }
                }
            } else if (type == EventType::KEY_RELEASE) {
                auto widget{widgets[0]};
                KeyTypeEvent kte{event};
                kte.set_source(widget);
                auto handlers{widget->get_event_handlers(EventType::KEY_TYPE)};
                for (const auto& handler: handlers) {
                    handler(&kte);
                }
            }
        }
    }
}

}
