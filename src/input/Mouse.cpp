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
    std::fill(x0, x0+5, -1);
    std::fill(y0, y0+5, -1);
    std::fill(x, x+5, 0);
    std::fill(y, y+5, 0);
    std::fill(dx, dx+5, 0);
    std::fill(dy, dy+5, 0);
    left = false;
    middle = false;
    right = false;
}

std::vector<std::shared_ptr<Event>> Mouse::get_events() {
    struct input_event ev;
    ssize_t bytes = read(fd, &ev, sizeof(ev));
    if (bytes < 0) {
        throw InputException{"failed to read event from input device " + path, errno};
    } else if ((size_t) bytes < sizeof(ev)) {
        throw InputException{"read truncated event from input device " + path};
    }

    std::vector<std::shared_ptr<Event>> events;

    if (ev.type == EV_SYN) {
        int32_t dx_ = transform_coords(dx[0]);
        int32_t dy_ = transform_coords(dy[0]);
        int32_t scroll_x = transform_scroll((dx[0]+dx[1])/2);
        int32_t scroll_y = transform_scroll((dy[0]+dy[1])/2);
        if (retroactive_mouse_press) {
            retroactive_mouse_press = false;
            events.push_back(std::make_shared<MousePressEvent>(left, middle, right));
        }

        if (pending_type == EventType::MOUSE_PRESS) {
            events.push_back(std::make_shared<MousePressEvent>(left, middle, right));
        } else if (pending_type == EventType::MOUSE_RELEASE) {
            events.push_back(std::make_shared<MouseReleaseEvent>(left, middle, right));
        } else if (pending_type == EventType::MOUSE_MOVE) {
            events.push_back(std::make_shared<MouseMoveEvent>(dx_, dy_, left, middle, right));
        } else if (pending_type == EventType::MOUSE_SCROLL) {
            events.push_back(std::make_shared<MouseScrollEvent>(scroll_x, scroll_y, left, middle, right));
        }
        if (fingers == 0x1 || (fingers == 0x2 && (left || middle || right))) {
            pending_type = EventType::MOUSE_MOVE;
            reset_diffs();
        } else if (fingers == 0x2) {
            pending_type = EventType::MOUSE_SCROLL;
            reset_diffs();
        } else if (fingers == 0) {
            reset();
        }
    } else if (ev.type == EV_KEY) {
        if (ev.code == BTN_TOOL_FINGER) {
            if (ev.value == 1) {
                fingers = fingers | 0x1;
                pending_type = EventType::MOUSE_MOVE;
            } else if (ev.value == 0) {
                fingers = fingers & 0x1E;
                int32_t dx2 = (x[0]-x0[0])*(x[0]-x0[0]);
                int32_t dy2 = (y[0]-y0[0])*(y[0]-y0[0]);
                if (dx2+dy2 < TOUCHPAD_TAP_RADIUS_2) {
                    retroactive_mouse_press = true;
                    pending_type = EventType::MOUSE_RELEASE;
                    left = true;
                    middle = false;
                    right = false;
                }
            }
        } else if (ev.code == BTN_TOOL_DOUBLETAP) {
            if (ev.value == 1) {
                fingers = fingers | 0x2;
                pending_type = EventType::MOUSE_SCROLL;
            } else if (ev.value == 0) {
                fingers = fingers & 0x1D;
                int32_t dx2 = (x[0]-x0[0])*(x[0]-x0[0]);
                int32_t dy2 = (y[0]-y0[0])*(y[0]-y0[0]);
                if (dx2+dy2 < TOUCHPAD_TAP_RADIUS_2) {
                    retroactive_mouse_press = true;
                    pending_type = EventType::MOUSE_RELEASE;
                    left = false;
                    middle = false;
                    right = true;
                }
            }
        } else if (ev.code == BTN_LEFT) {
            if (ev.value == 1) {
                pending_type = EventType::MOUSE_PRESS;
                if (x[slot] < w/2) {
                    left = true;
                } else {
                    right = true;
                }
            } else if (ev.value == 0) {
                pending_type = EventType::MOUSE_RELEASE;
                left = false;
                middle = false;
                right = false;
            }
        }
    } else if (ev.type == EV_ABS) {
        if (ev.code == ABS_MT_SLOT) {
            slot = ev.value;
        } else if (ev.code == ABS_MT_POSITION_X) {
            if (x0[slot] < 0) {
                x0[slot] = ev.value;
                dx[slot] = 0;
                dy[slot] = 0;
            } else {
                dx[slot] = ev.value - x[slot];
            }
            x[slot] = ev.value;
        } else if (ev.code == ABS_MT_POSITION_Y) {
            if (y0[slot] < 0) {
                y0[slot] = ev.value;
                dx[slot] = 0;
                dy[slot] = 0;
            } else {
                dy[slot] = ev.value - y[slot];
            }
            y[slot] = ev.value;
        }
    } else if (ev.type == EV_REL) {

    }

    return events;
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
    float s = AerendServer::the().get_display_manager().get_mouse_sensitivity();
    return n*s;
}

int32_t Mouse::transform_scroll(int32_t n) {
    float s = AerendServer::the().get_display_manager().get_scroll_sensitivity();
    return n*s;
}

}
