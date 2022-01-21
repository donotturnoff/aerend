#include "MergedUpdates.h"
#include <iostream>

namespace aerend {

void MergedUpdates::move_cursor(int32_t dx, int32_t dy) {
    cursor_dx += dx;
    cursor_dy += dy;
}

void MergedUpdates::move_widget(Widget* widget, int32_t dx, int32_t dy) {
    widget_dx[widget] += dx;
    widget_dy[widget] += dy;
}

void MergedUpdates::follow_mouse(Widget* widget) {
    mouse_followers.insert(widget);
}

void MergedUpdates::set_cursor(Cursor* cursor) {
    this->cursor = cursor;
}

void MergedUpdates::apply() {
    auto& dm = AerendServer::the().get_display_manager();
    if (cursor_dx != 0 || cursor_dy != 0) {
        if (cursor) {
            dm.update_cursor(cursor, cursor_dx, cursor_dy);
            cursor = nullptr;
        } else {
            dm.update_cursor(cursor_dx, cursor_dy);
        }
    } else if (cursor) {
        dm.update_cursor(cursor);
        cursor = nullptr;
    }
    for (const auto& pair: widget_dx) {
        Widget* widget = pair.first;
        int32_t dx = pair.second;
        int32_t dy = widget_dy[widget];
        int32_t x = widget->get_x();
        int32_t y = widget->get_y();
        widget->set_pos(x+dx, y+dy);
    }

    for (const auto& widget: mouse_followers) {
        int32_t x = widget->get_x();
        int32_t y = widget->get_y();
        widget->set_pos(x+cursor_dx, y+cursor_dy);
    }

    cursor_dx = 0;
    cursor_dy = 0;
    widget_dx.clear();
    widget_dy.clear();
    mouse_followers.clear();
}

}
