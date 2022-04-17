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
#include <unordered_map>
#include <iostream>

namespace aerend {

class InputHandler {
public:
    InputHandler();
    ~InputHandler();
    template <class D> void add_device(std::string uid, std::string path);
    template <class D> void rm_device(std::string uid);
private:
    void run();
    static const int32_t MAX_EVENTS{5};
    int epoll_fd, halt_fd;
    std::map<int, std::unique_ptr<InputDevice>> fd_devs;
    std::map<std::string, int> uid_fds;
    std::thread thread;
    std::atomic<bool> running{true};
};

template <class D>
void InputHandler::add_device(std::string uid, std::string path) {
    auto dev{std::make_unique<D>(path)};
    int fd{dev->get_fd()};

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;
    int ret{epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event)};
    if (ret < 0) {
        throw InputException{"failed to add input device " + uid + " to epoll", errno};
    }

    fd_devs[fd] = std::move(dev);
    uid_fds[uid] = fd;

    std::cout << "Attached device " << uid << std::endl;
}

template <class D>
void InputHandler::rm_device(std::string uid) {
    int fd{uid_fds[uid]};

    int ret{epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr)};
    if (ret < 0) {
        throw InputException{"failed to remove input device " + uid + " from epoll", errno};
    }

    uid_fds.erase(uid);
    fd_devs.erase(fd);

    std::cout << "Detached device " << uid << std::endl;
}

}

#endif // INPUT_HANDLER_H

