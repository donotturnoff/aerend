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
    virtual void process_events() = 0;
    int get_fd() const noexcept;
    std::string get_path() const noexcept;
protected:
    std::string path;
    int fd;
};

}

#endif // INPUT_DEVICE_H

