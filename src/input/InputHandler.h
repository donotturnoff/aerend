#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "event/EventDispatcher.h"
#include "InputDevice.h"
#include <linux/input.h>
#include <sys/epoll.h>
#include <atomic>
#include <map>
#include <memory>
#include <thread>

namespace aerend {

class InputHandler {
public:
    InputHandler();
    ~InputHandler();
    template <class D> void add_device(std::string path);
private:
    void run();
    static const int32_t MAX_EVENTS = 5;
    int epoll_fd, halt_fd;
    std::map<int, std::unique_ptr<InputDevice>> devs;
    std::thread thread;
    std::atomic<bool> running;
};

template <class D>
void InputHandler::add_device(std::string path) {
    auto dev = std::make_unique<D>(path);
    int fd = dev->get_fd();
    devs[fd] = std::move(dev);
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;
    int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    if (ret < 0) {
        throw InputException{"failed to add input device " + dev->get_path() + " to epoll", errno};
    }
}

}

#endif // INPUT_HANDLER_H

