#include "InputDevice.h"
#include <unistd.h>

namespace aerend {

InputDevice::~InputDevice() {
    close(fd);
}

int InputDevice::get_fd() const noexcept {
    return fd;
}

}
