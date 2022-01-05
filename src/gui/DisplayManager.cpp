#include "DisplayManager.h"
#include "MergedUpdates.h"
#include <iostream>
#include <cstdio>

namespace aerend {

DisplayManager::DisplayManager() : merged_updates(std::unique_ptr<MergedUpdates>{new MergedUpdates{}}), card(DRMCard{"/dev/dri/card0"}), ARROW_CURSOR(std::make_shared<Cursor>(card.get_fd(), CursorPreset::ARROW)), cursor_x(0), cursor_y(0), focused(nullptr), running(true) {
    int32_t w = card.get_conns()[0]->get_back_buf().get_w();
    int32_t h = card.get_conns()[0]->get_back_buf().get_h();

    wmp.set_size(w, h);

    set_cursor(ARROW_CURSOR);
    thread = std::thread(&DisplayManager::run, this);
}

DisplayManager::~DisplayManager() {
    running.store(false);
    push_update([](){});
    thread.join();
}

void DisplayManager::repaint() {
    for (const auto& conn: card.get_conns()) {
        auto dst = conn->get_back_buf();
        dst.clear();
        for (const auto& win: windows) {
            dst.composite(win->get_bmp(), win->get_x(), win->get_y());
        }
        conn->repaint();
    }
}

void DisplayManager::remap() {
    wmp.clear();
    for (const auto& window: windows) {
        wmp.add(window);
    }
}

void DisplayManager::add_win(Window* win) {
    windows.push_back(win);
    wmp.add(win);
}

void DisplayManager::rm_win(Window* win) {
    // TODO: shorten (likewise in bump_win)
    auto i = std::find(windows.begin(), windows.end(), win);
    if (i != windows.end()) {
        windows.erase(i);
    }
    // TODO: no need to remap all the windows
    remap();
}

void DisplayManager::bump_win(Window* win) {
    auto i = std::find(windows.begin(), windows.end(), win);
    if (i != windows.end()) {
        windows.erase(i);
    }
    add_win(win);
}

Window* DisplayManager::get_window_at(int32_t x, int32_t y) {
    // TODO: remove cast by creating WindowMap or something
    return (Window*) wmp.get(x, y);
}

SimpleBitmap& DisplayManager::get_bmp(Window* window) {
    return window->get_bmp();
}

void DisplayManager::set_cursor(std::shared_ptr<Cursor> cursor) {
    this->cursor = cursor;
    for (const auto& conn: card.get_conns()) {
        conn->set_cursor(cursor, cursor_x, cursor_y);
    }
}

void DisplayManager::update_cursor_displacement(int32_t dx, int32_t dy) {
    cursor_dx += dx;
    cursor_dy += dy;
}

void DisplayManager::move_cursor(int32_t dx, int32_t dy) {
    cursor_x += dx;
    cursor_y += dy;
    // TODO: more than one screen
    // TODO: improve method of getting dimensions
    int32_t w = card.get_conns()[0]->get_back_buf().get_w();
    int32_t h = card.get_conns()[0]->get_back_buf().get_h();
    if (cursor_x < 0) cursor_x = 0;
    if (cursor_y < 0) cursor_y = 0;
    if (cursor_x >= w) cursor_x = w-1;
    if (cursor_y >= h) cursor_y = h-1;
    for (const auto& conn: card.get_conns()) {
        conn->set_cursor(cursor, cursor_x, cursor_y);
    }
}

std::vector<Widget*> DisplayManager::get_widgets(std::shared_ptr<Event> event) {
    EventType type = event->type;
    Widget* widget = nullptr;
    if (type == EventType::MOUSE_MOVE || type == EventType::MOUSE_PRESS || type == EventType::MOUSE_RELEASE || type == EventType::MOUSE_SCROLL) {
        int32_t x = cursor_x;
        int32_t y = cursor_y;
        Window* window = get_window_at(x, y);
        if (window) {
            widget = window->get_widget_at(x-window->get_x(), y-window->get_y());
        }
    } else if (type == EventType::KEY_PRESS || type == EventType::KEY_RELEASE) {
        widget = focused;
    }
    std::vector<Widget*> widgets {};
    if (widget) {
        do {
            widgets.push_back(widget);
            widget = widget->get_parent();
        } while (widget->get_parent() != widget);
        widgets.push_back(widget);
    }
    return widgets;
}

void DisplayManager::focus_on(Widget* widget) {
    focused = widget;
}

void DisplayManager::unfocus() {
    focused = nullptr;
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
