#ifndef MOUSE_H
#define MOUSE_H

#include "InputDevice.h"
#include <string>

namespace aerend {

class Mouse: public InputDevice {
public:
    Mouse(std::string path);
    std::vector<std::shared_ptr<Event>> get_events();
    bool get_left() const noexcept;
    bool get_middle() const noexcept;
    bool get_right() const noexcept;
private:
    static const int32_t TOUCHPAD_TAP_RADIUS_2 = 900;
    int32_t transform_coords(int32_t n);
    int32_t transform_scroll(int32_t n);
    void reset();
    void reset_diffs();
    int32_t w = 2442;
    EventType pending_type;
    bool left, middle, right, retroactive_mouse_press;
    int32_t slot, fingers, x0[5], y0[5], x[5], y[5], dx[5], dy[5];
};

}

#endif // MOUSE_H

