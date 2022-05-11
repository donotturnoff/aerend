#include "Mouse.h"
#include "AerendServer.h"
#include "event/Event.h"
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <linux/input.h>

namespace aerend {

Mouse::Mouse(std::string path) : InputDevice(path), left(false), middle(false), right(false), slot(0), fingers(0), x0{-1, -1, -1, -1, -1}, y0{-1, -1, -1, -1, -1}, x{-1, -1, -1, -1, -1}, y{-1, -1, -1, -1, -1}, dx{0, 0, 0, 0, 0}, dy{0, 0, 0, 0, 0} {}

void Mouse::reset_diffs() {
    std::fill(dx, dx+5, 0);
    std::fill(dy, dy+5, 0);
}

void Mouse::reset() {
   for (uint8_t i{0}; i < 5; i++) {
        if (to_reset & (1 << i)) {
            x0[i] = -1;
            y0[i] = -1;
        }
   }
}

std::vector<std::shared_ptr<Event>> Mouse::get_events() {
    struct input_event ev;
    ssize_t bytes = read(fd, &ev, sizeof(ev));
    if (bytes < 0) {
        throw InputException{"failed to read event from input device " + path, errno};
    } else if ((size_t) bytes < sizeof(ev)) {
        throw InputException{"read truncated event from input device " + path};
    }

    if (ev.type == EV_SYN) {
        if (tap) {
            /* After touchpad tap, generate press and release events */
            events.push_back(std::make_shared<MousePressEvent>(left, middle, right));
            events.push_back(std::make_shared<MouseReleaseEvent>(left, middle, right));
            tap = false;
        }
        if ((fingers & 0x01) && y0[slot] < active_h) {
            auto dx_{transform_coords(dx[slot])};
            auto dy_{transform_coords(dy[slot])};
            if (dx_ != 0 || dy_ != 0) {
                events.push_back(std::make_shared<MouseMoveEvent>(dx_, dy_, left, middle, right));
            }
        } else if (fingers & 0x02) {
            if (y0[0] > active_h) { /* First touch was in inactive (button) zone, so move instead of scrolling */
                auto dx_{transform_coords(dx[1])};
                auto dy_{transform_coords(dy[1])};
                if (dx_ != 0 || dy_ != 0) {
                    events.push_back(std::make_shared<MouseMoveEvent>(dx_, dy_, left, middle, right));
                }
            } else if (y0[1] > active_h) { /* Ditto for second touch */
                auto dx_{transform_coords(dx[0])};
                auto dy_{transform_coords(dy[0])};
                if (dx_ != 0 || dy_ != 0) {
                    events.push_back(std::make_shared<MouseMoveEvent>(dx_, dy_, left, middle, right));
                }
            } else { /* Both in active region, so scroll */
                auto dx_{transform_scroll((dx[0] + dx[1])/2)};
                auto dy_{transform_scroll((dy[0] + dy[1])/2)};
                if (dx_ != 0 || dy_ != 0) {
                    events.push_back(std::make_shared<MouseScrollEvent>(dx_, dy_, left, middle, right));
                }
            }
        }

        reset_diffs();
        reset();
        to_reset = 0;

        auto e{events};
        events.clear();
        return e;
    } else if (ev.type == EV_KEY) {
        if (ev.code == BTN_TOOL_FINGER) {
            if (ev.value == 1) {
                fingers = fingers | 0x1;
            } else if (ev.value == 0) {
                fingers = fingers & 0x1E;
                int32_t dx2 = (x[slot]-x0[slot])*(x[slot]-x0[slot]);
                int32_t dy2 = (y[slot]-y0[slot])*(y[slot]-y0[slot]);
                if (dx2+dy2 < TOUCHPAD_TAP_RADIUS_2) {
                    left = true;
                    middle = false;
                    right = false;
                    tap = true;
                }
            }
        } else if (ev.code == BTN_TOOL_DOUBLETAP) {
            if (ev.value == 1) {
                fingers = fingers | 0x2;
                tap = false;
            } else if (ev.value == 0) {
                fingers = fingers & 0x1D;
                int32_t dx2 = (x[slot]-x0[slot])*(x[slot]-x0[slot]);
                int32_t dy2 = (y[slot]-y0[slot])*(y[slot]-y0[slot]);
                if (dx2+dy2 < TOUCHPAD_TAP_RADIUS_2) {
                    left = false;
                    middle = false;
                    right = true;
                    tap = true;
                }
            }
        } else if (ev.code == BTN_LEFT) {
            if (ev.value == 1) {
                if (x[slot] < w/2) {
                    left = true;
                    events.push_back(std::make_shared<MousePressEvent>(true, false, false));
                } else {
                    right = true;
                    events.push_back(std::make_shared<MousePressEvent>(false, false, true));
                }
            } else if (ev.value == 0) {
                events.push_back(std::make_shared<MouseReleaseEvent>(left, false, right));
                left = false;
                right = false;
            }
        }
    } else if (ev.type == EV_ABS) {
        if (ev.code == ABS_MT_SLOT) { /* Slot specifier */
            slot = ev.value;
        } else if (ev.code == ABS_MT_POSITION_X) { /* x coord */
            if (x0[slot] < 0) { /* First movement */
                x0[slot] = ev.value;
            } else {
                dx[slot] = ev.value - x[slot];
            }
            x[slot] = ev.value;
        } else if (ev.code == ABS_MT_POSITION_Y) { /* y coord */
            if (y0[slot] < 0) { /* First movement */
                y0[slot] = ev.value;
            } else {
                dy[slot] = ev.value - y[slot];
            }
            y[slot] = ev.value;
        } else if (ev.code == ABS_MT_TRACKING_ID) { /* finger up/down */
            if (ev.value < 0) {
                to_reset |= (1 << slot);
            }
        }
    } else if (ev.type == EV_REL) {

    }

    return {};
}

bool Mouse::get_left() const noexcept {
    return left;
}

bool Mouse::get_middle() const noexcept {
    return middle;
}

bool Mouse::get_right() const noexcept {
    return right;
}

int32_t Mouse::transform_coords(int32_t n) {
    float s = AerendServer::the().dm().get_mouse_sensitivity();
    return n*s;
}

int32_t Mouse::transform_scroll(int32_t n) {
    float s = AerendServer::the().dm().get_scroll_sensitivity();
    return n*s;
}

}
