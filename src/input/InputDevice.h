#ifndef INPUT_DEVICE_H
#define INPUT_DEVICE_H

#include "event/Event.h"
#include <memory>
#include <vector>

namespace aerend {

class InputDevice {
public:
    virtual ~InputDevice();
    virtual std::vector<std::shared_ptr<Event>> get_events() = 0;
    int get_fd() const noexcept;
protected:
    int fd;
};

}

#endif // INPUT_DEVICE_H

