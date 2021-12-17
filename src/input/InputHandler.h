#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "event/EventDispatcher.h"
#include "InputDevice.h"
#include <linux/input.h>
#include <atomic>
#include <map>
#include <memory>

namespace aerend {

class InputHandler {
public:
    InputHandler();
    ~InputHandler();
    void add_device(std::shared_ptr<InputDevice> dev);
    void run();
    void stop();
private:
    static const int32_t MAX_EVENTS = 5;
    std::atomic<bool> running;
    int epoll_fd;
    std::map<int, std::shared_ptr<InputDevice>> devs;
};

}

#endif // INPUT_HANDLER_H

