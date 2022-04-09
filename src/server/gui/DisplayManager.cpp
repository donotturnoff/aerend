#include "DisplayManager.h"
#include "MergedUpdates.h"
#include <iostream>
#include <cstdio>

namespace aerend {

// TODO: use this by making background DRM plane
const Colour DisplayManager::def_bg_colour{16, 49, 73};

//DisplayManager::DisplayManager() {}

DisplayManager::DisplayManager() : merged_updates(std::make_unique<MergedUpdates>()), card(DRMCard{"/dev/dri/card0"}), cursors(card.get_fd()) {
    int32_t w = card.get_conns()[0].get_w();
    int32_t h = card.get_conns()[0].get_h();

    update_cursor(cursors.get_cursor(CursorType::ARROW), w/2, h/2);

    std::cout << "Opened display manager" << std::endl;
    thread = std::thread(&DisplayManager::run, this);
}
//*/

DisplayManager::~DisplayManager() {
    running.store(false);
    push_update([](){});
    thread.join();
    std::cout << "Closed display manager" << std::endl;
}

void DisplayManager::repaint() {
    for (auto& conn: card.get_conns()) {
        conn.clear();
        for (const auto& win: window_stack) {
            conn.composite(win->get_bmp(), win->get_x(), win->get_y());
        }
        conn.repaint();
    }
}

void DisplayManager::open_window(Window* window) {
    auto i = std::find(window_stack.begin(), window_stack.end(), window);
    if (i == window_stack.end()) {
        window_stack.push_back(window);
    }
}

void DisplayManager::close_window(Window* window) {
    // TODO: shorten (likewise in bump_win)
    auto i = std::find(window_stack.begin(), window_stack.end(), window);
    if (i != window_stack.end()) {
        window_stack.erase(i);
    }
}

void DisplayManager::bump_window(Window* window) {
    auto i = std::find(window_stack.begin(), window_stack.end(), window);
    if (i != window_stack.end()) {
        window_stack.erase(i);
    }
    open_window(window);
}

Window* DisplayManager::get_window_at(int32_t x, int32_t y) {
    for (auto it{window_stack.rbegin()}; it != window_stack.rend(); ++it) {
        auto window{*it};
        auto win_x{window->get_x()};
        auto win_y{window->get_y()};
        auto win_w{window->get_w()};
        auto win_h{window->get_h()};
        if (x >= win_x && x < win_x + win_w && y >= win_y && y < win_y + win_h) { // TODO: make method for this
            return window;
        }
    }
    return nullptr;
}

int32_t DisplayManager::get_cursor_x() {
    return cursor_x;
}

int32_t DisplayManager::get_cursor_y() {
    return cursor_y;
}

void DisplayManager::update_cursor(Cursor* cursor) {
    update_cursor(cursor, 0, 0);
}

void DisplayManager::update_cursor(int32_t dx, int32_t dy) {
    update_cursor(cursor, dx, dy);
}

void DisplayManager::update_cursor(Cursor* cursor, int32_t dx, int32_t dy) {
    this->cursor = cursor;
    cursor_x += dx;
    cursor_y += dy;
    // TODO: more than one screen
    int32_t w = card.get_conns()[0].get_w();
    int32_t h = card.get_conns()[0].get_h();
    if (cursor_x < 0) cursor_x = 0;
    if (cursor_y < 0) cursor_y = 0;
    if (cursor_x >= w) cursor_x = w-1;
    if (cursor_y >= h) cursor_y = h-1;
    for (auto& conn: card.get_conns()) {
        conn.set_cursor(cursor, cursor_x, cursor_y);
    }
}

void DisplayManager::set_mouse_sensitivity(float mouse_sensitivity) {
    this->mouse_sensitivity = mouse_sensitivity;
}

void DisplayManager::set_scroll_sensitivity(float scroll_sensitivity) {
    this->scroll_sensitivity = scroll_sensitivity;
}

float DisplayManager::get_mouse_sensitivity() {
    return mouse_sensitivity;
}

float DisplayManager::get_scroll_sensitivity() {
    return scroll_sensitivity;
}

std::vector<Widget*> DisplayManager::get_widgets(Event* event) {
    EventType type{event->get_type()};
    std::vector<Widget*> widgets;
    if (type == EventType::MOUSE_MOVE || type == EventType::MOUSE_PRESS || type == EventType::MOUSE_RELEASE || type == EventType::MOUSE_SCROLL) {
        if (!grabbed.empty()) {
            widgets = grabbed;
        } else {
            int32_t x{cursor_x};
            int32_t y{cursor_y};
            Window* window{get_window_at(x, y)};
            if (window) {
                // TODO: make method for converting to window-space
                window->get_widgets_at(widgets, x-window->get_x(), y-window->get_y());
            }
        }
    } else if (type == EventType::KEY_PRESS || type == EventType::KEY_RELEASE) {
        widgets = focused;
    }
    return widgets;
}

void DisplayManager::focus_on(std::vector<Widget*> widgets) {
    focused = widgets;
}

void DisplayManager::unfocus() {
    focused.clear();
}

void DisplayManager::grab(std::vector<Widget*> widgets) {
    grabbed = widgets;
}

void DisplayManager::drop() {
    grabbed.clear();
}

void DisplayManager::push_update(std::function<void()> update) {
    std::lock_guard<std::mutex> lock{upq_mtx};
    update_queue.push(update);
    upq_cond.notify_one();
}

std::vector<std::function<void()>> DisplayManager::pop_updates() {
    std::vector<std::function<void()>> updates;
    std::unique_lock<std::mutex> lock{upq_mtx};
    upq_cond.wait(lock, [&]{ return !update_queue.empty(); });
    while (!update_queue.empty()) {
        updates.push_back(update_queue.front());
        update_queue.pop();
    }
    return updates;
}

void DisplayManager::run() {
    while (running) {
        auto updates = pop_updates();
        for (const auto& update: updates) {
            update();
        }
        merged_updates->apply();
    }
}

}
