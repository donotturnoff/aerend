#include "Keyboard.h"
#include "event/KeyboardEvent.h"
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
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

Keyboard::Keyboard(std::string path) : shift(false), ctrl(false), alt(false), meta(false), fn(false) {
    fd = open(path.c_str(), O_RDONLY);
    // TODO: error handling
}

std::vector<std::shared_ptr<Event>> Keyboard::get_events() {
    struct input_event ev;
    int s = read(fd, &ev, sizeof(ev));
    if (ev.type == EV_KEY) {
        char c = shift ? SHIFTED_CHARS[ev.code] : CHARS[ev.code];
        EventType type;

        if (ev.value == 0) {
            type = EventType::KEY_RELEASE;
        } else if (ev.value == 1 || ev.value == 2) {
            type = EventType::KEY_PRESS;
        } else {
            return std::vector<std::shared_ptr<Event>>{};
        }

        if (c && type == EventType::KEY_RELEASE) {
            type = EventType::KEY_TYPE;
        }

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

        return std::vector<std::shared_ptr<Event>>{std::make_shared<KeyboardEvent>(type, c, shift, ctrl, alt, meta, fn)};
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
