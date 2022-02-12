#ifndef MERGED_UPDATES_H
#define MERGED_UPDATES_H

#include "AerendServer.h"
#include "Widget.h"
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

namespace aerend {

class MergedUpdates {
public:
    void move_cursor(int32_t dx, int32_t dy);
    void move_widget(Widget* widget, int32_t dx, int32_t dy);
    void set_cursor(Cursor* cursor);
    void follow_mouse(Widget* widget);
    void apply();
private:
    int32_t cursor_dx{0}, cursor_dy{0};
    std::unordered_map<Widget*, int32_t> widget_dx, widget_dy;
    std::unordered_set<Widget*> mouse_followers;
    Cursor* cursor{nullptr};
};

}

#endif // MERGED_UPDATES_H

