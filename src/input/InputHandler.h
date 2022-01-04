#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "event/EventDispatcher.h"
#include "InputDevice.h"
#include <linux/input.h>
#include <atomic>
#include <map>
#include <memory>
#include <thread>

namespace aerend {

class InputHandler {
public:
    InputHandler();
    ~InputHandler();
    void add_device(std::shared_ptr<InputDevice> dev);
private:
    void run();
    static const int32_t MAX_EVENTS = 5;
    int epoll_fd, halt_fd;
    std::map<int, std::shared_ptr<InputDevice>> devs;
    std::thread thread;
    std::atomic<bool> running;
};

}

#endif // INPUT_HANDLER_H

