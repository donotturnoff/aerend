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
    static const int32_t TOUCHPAD_TAP_RADIUS_2{900};
    int32_t transform_coords(int32_t n);
    int32_t transform_scroll(int32_t n);
    void reset();
    void reset_diffs();
    int32_t w{3512}, h{2442}, active_h{2000};
    bool left{false}, middle{false}, right{false}, tap{false};
    uint8_t old_fingers{0}, fingers{0}, to_reset{0}, slot{0};
    int32_t x0[5]{-1, -1, -1, -1, -1}, y0[5]{-1, -1, -1, -1, -1};
    int32_t x[5]{0, 0, 0, 0, 0}, y[5]{0, 0, 0, 0, 0};
    int32_t dx[5]{0, 0, 0, 0, 0}, dy[5]{0, 0, 0, 0, 0};
    std::vector<std::shared_ptr<Event>> events;
};

}

#endif // MOUSE_H

