#include "EventHandler.h"
#include <iostream>

namespace aerend {

EventHandler::EventHandler(EventType type, Widget* widget) : type(type), widget(widget), window(widget->get_root()) {}

EventHandler::EventHandler(EventType type, Window* window) : type(type), widget(nullptr), window(window) {}

void EventHandler::handle(std::shared_ptr<Event> event) {
    std::cout << "Event" << std::endl;
}

EventType EventHandler::get_type() {
    return type;
}

Widget* EventHandler::get_widget() {
    return widget;
}

Window* EventHandler::get_window() {
    return window;
}

}
