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

    wmp.set_size(w, h);

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

void DisplayManager::remap() {
    wmp.clear();
    for (const auto& window: window_stack) {
        wmp.add(window);
    }
}

void DisplayManager::open_window(Window* window) {
    window_stack.push_back(window);
    wmp.add(window);
}

void DisplayManager::close_window(Window* window) {
    // TODO: shorten (likewise in bump_win)
    auto i = std::find(window_stack.begin(), window_stack.end(), window);
    if (i != window_stack.end()) {
        window_stack.erase(i);
    }
    // TODO: no need to remap all the windows
    remap();
}

void DisplayManager::bump_window(Window* window) {
    auto i = std::find(window_stack.begin(), window_stack.end(), window);
    if (i != window_stack.end()) {
        window_stack.erase(i);
    }
    open_window(window);
}

Window* DisplayManager::get_window_at(int32_t x, int32_t y) {
    // TODO: remove cast by creating WindowMap or something
    return (Window*) wmp.get(x, y);
}

SimpleBitmap& DisplayManager::get_bmp(Window* window) {
    return window->get_bmp();
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
    EventType type = event->get_type();
    Widget* widget = nullptr;
    if (type == EventType::MOUSE_MOVE || type == EventType::MOUSE_PRESS || type == EventType::MOUSE_RELEASE || type == EventType::MOUSE_SCROLL) {
        if (grabbed) {
            widget = grabbed;
        } else {
            int32_t x = cursor_x;
            int32_t y = cursor_y;
            Window* window = get_window_at(x, y);
            if (window) {
                widget = window->get_widget_at(x-window->get_x(), y-window->get_y());
            }
        }
    } else if (type == EventType::KEY_PRESS || type == EventType::KEY_RELEASE) {
        widget = focused;
    }
    std::vector<Widget*> widgets {};
    if (widget) {
        do {
            if (widget) {
                widgets.push_back(widget);
                widget = widget->get_parent();
            }
        } while (widget->get_parent() != widget);
        if (widget) {
            widgets.push_back(widget);
        }
    }
    return widgets;
}

void DisplayManager::focus_on(Widget* widget) {
    focused = widget;
}

void DisplayManager::unfocus() {
    focused = nullptr;
}


void DisplayManager::grab(Widget* widget) {
    grabbed = widget;
}

void DisplayManager::drop() {
    grabbed = nullptr;
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
