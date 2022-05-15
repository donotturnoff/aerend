#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "InputDevice.h"
#include <string>

namespace aerend {

class Keyboard: public InputDevice {
public:
    Keyboard(std::string path);
    void process_events();
    bool get_shift() const noexcept;
    bool get_ctrl() const noexcept;
    bool get_alt() const noexcept;
    bool get_meta() const noexcept;
    bool get_fn() const noexcept;
private:
    const static char CHARS[256];
    const static char SHIFTED_CHARS[256];
    bool shift, ctrl, alt, meta, fn;
};

}

#endif // KEYBOARD_H

