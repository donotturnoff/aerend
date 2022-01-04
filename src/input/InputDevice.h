#ifndef INPUT_DEVICE_H
#define INPUT_DEVICE_H

#include "event/Event.h"
#include "InputException.h"
#include <fcntl.h>
#include <memory>
#include <string>
#include <vector>
#include <cstdlib>

namespace aerend {

class InputDevice {
public:
    InputDevice(std::string path);
    virtual ~InputDevice();
    virtual std::vector<std::shared_ptr<Event>> get_events() = 0;
    int get_fd() const noexcept;
    std::string path;
protected:
    int fd;
};

}

#endif // INPUT_DEVICE_H

