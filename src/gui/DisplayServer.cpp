#include "DisplayServer.h"
#include <iostream>

namespace aerend {

DisplayServer::DisplayServer() : card(DRMCard{"/dev/dri/card0"}) {}

DisplayServer& DisplayServer::the() {
    static DisplayServer instance;
    return instance;
}

void DisplayServer::repaint() {
    for (const auto& conn: card.get_conns()) {
        conn->get_back_buf().clear();
        for (const auto& win: windows) {
            conn->get_back_buf().composite(win->get_bmp(), win->get_x(), win->get_y());
        }
        conn->repaint();
    }
}

void DisplayServer::add_win(Window* win) {
    windows.push_back(win);
}

void DisplayServer::rm_win(Window* win) {
    auto i = std::find(windows.begin(), windows.end(), win);
    if (i != windows.end()) {
        windows.erase(i);
    }
}

void DisplayServer::bump_win(Window* win) {
    rm_win(win);
    add_win(win);
}

SimpleBitmap& DisplayServer::get_bmp(Window* window) {
    return window->get_bmp();
}

void DisplayServer::push_event(std::shared_ptr<Event> event) {
    events_mtx.lock();
    events.push(event);
    events_mtx.unlock();
}

std::shared_ptr<Event> DisplayServer::pop_event() {
    events_mtx.lock();
    auto event = events.front();
    events.pop();
    events_mtx.unlock();
    return event;
}

void DisplayServer::wait_for_mouse() {

}

void DisplayServer::wait_for_keyboard() {

}

void DisplayServer::wait_for_client() {

}

}
