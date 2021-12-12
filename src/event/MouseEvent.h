#ifndef MOUSE_EVENT_H
#define MOUSE_EVENT_H

#include "Event.h"
#include <cstdint>

namespace aerend {

class MouseEvent: public Event {
public:
    MouseEvent(EventType type, int32_t x, int32_t y, int32_t dx, int32_t dy, int32_t scroll_x, int32_t scroll_y, bool left, bool middle, bool right);
    int32_t get_x() const noexcept;
    int32_t get_y() const noexcept;
    int32_t get_dx() const noexcept;
    int32_t get_dy() const noexcept;
    int32_t get_scroll_x() const noexcept;
    int32_t get_scroll_y() const noexcept;
    bool get_left() const noexcept;
    bool get_middle() const noexcept;
    bool get_right() const noexcept;
private:
    int32_t x, y, dx, dy, scroll_x, scroll_y;
    bool left, middle, right;
};

}

#endif // MOUSE_EVENT_H

