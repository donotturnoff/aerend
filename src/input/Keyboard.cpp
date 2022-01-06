#include "Keyboard.h"
#include "event/Event.h"
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <linux/input.h>

namespace aerend {

const char Keyboard::CHARS[] = {
    0, 0x1B,
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
    ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm',
    ',', '.', '/', 0, '*', 0, ' ', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,
    0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
    0,
    0, 0,
    0, 0,
    0, 0, 0, 0, 0, 0, 0,
    '\n', 0, '/', 0, 0, '\n', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // ...
};

const char Keyboard::SHIFTED_CHARS[] = {
    0, 0x1B,
    '!', '"', '3', '$', '%', '^', '&', '*', '(', ')',
    '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
    '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
    ':', '@', '`', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M',
    '<', '>', '?', 0, '*', 0, ' ', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,
    0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
    0,
    0, 0,
    0, 0,
    0, 0, 0, 0, 0, 0, 0,
    '\n', 0, '/', 0, 0, '\n', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

Keyboard::Keyboard(std::string path) : InputDevice(path), shift(false), ctrl(false), alt(false), meta(false), fn(false) {}

std::vector<std::shared_ptr<Event>> Keyboard::get_events() {
    struct input_event ev;
    ssize_t bytes = read(fd, &ev, sizeof(ev));
    if (bytes < 0) {
        throw InputException{"failed to read event from input device " + path, errno};
    } else if ((size_t) bytes < sizeof(ev)) {
        throw InputException{"read truncated event from input device " + path};
    }

    if (ev.type == EV_KEY) {
        EventType type = (ev.value == 0) ? EventType::KEY_RELEASE : EventType::KEY_PRESS;
        if (ev.code == KEY_LEFTSHIFT || ev.code == KEY_RIGHTSHIFT) {
            shift = (type == EventType::KEY_PRESS);
        }

        if (ev.code == KEY_LEFTCTRL || ev.code == KEY_RIGHTCTRL) {
            ctrl = (type == EventType::KEY_PRESS);
        }

        if (ev.code == KEY_LEFTALT || ev.code == KEY_RIGHTALT) {
            alt = (type == EventType::KEY_PRESS);
        }

        if (ev.code == KEY_LEFTMETA || ev.code == KEY_RIGHTMETA) {
            meta = (type == EventType::KEY_PRESS);
        }

        if (ev.code == KEY_FN) {
            fn = (type == EventType::KEY_PRESS);
        }

        char c = shift ? SHIFTED_CHARS[ev.code] : CHARS[ev.code];

        if (ev.value == 0) {
            return std::vector<std::shared_ptr<Event>>{std::make_shared<KeyReleaseEvent>(c, shift, ctrl, alt, meta, fn)};
        } else if (ev.value == 1 || ev.value == 2) {
            return std::vector<std::shared_ptr<Event>>{std::make_shared<KeyPressEvent>(c, shift, ctrl, alt, meta, fn, ev.value == 2)};
        }
    }

    return std::vector<std::shared_ptr<Event>>{};
}

bool Keyboard::get_shift() const noexcept {
    return shift;
}

bool Keyboard::get_ctrl() const noexcept {
    return ctrl;
}

bool Keyboard::get_alt() const noexcept {
    return alt;
}

bool Keyboard::get_meta() const noexcept {
    return meta;
}

bool Keyboard::get_fn() const noexcept {
    return fn;
}

}
