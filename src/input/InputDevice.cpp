#include "InputDevice.h"
#include "InputException.h"
#include <unistd.h>
#include <cerrno>

namespace aerend {

InputDevice::InputDevice(std::string path) : path(path), fd(open(path.c_str(), O_RDONLY)) {
    if (fd < 0) {
        throw InputException("failed to open input device " + path, errno);
    }
}

InputDevice::~InputDevice() {
    close(fd);
}

int InputDevice::get_fd() const noexcept {
    return fd;
}

}
