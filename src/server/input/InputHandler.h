#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "event/EventDispatcher.h"
#include "InputDevice.h"
#include <linux/input.h>
#include <poll.h>
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
    void signal();
    static const int32_t MAX_EVENTS{5};
    int sig_fd;
    std::map<int, std::unique_ptr<InputDevice>> fd_devs;
    std::map<std::string, int> uid_fds;
    std::vector<struct pollfd> poll_fds;
    std::thread thread;
    std::mutex poll_fds_mtx, fd_devs_mtx;
    std::atomic<bool> running{true};
};

template <class D>
void InputHandler::add_device(std::string uid, std::string path) {
    auto dev{std::make_unique<D>(path)};
    int fd{dev->get_fd()};

    {
        /* Add device file to poll */
        std::unique_lock<std::mutex> lock{poll_fds_mtx};
        struct pollfd poll_fd = {fd, POLLIN, 0};
        poll_fds.push_back(poll_fd);
    }

    {
        /* Register device object */
        std::unique_lock<std::mutex> lock{fd_devs_mtx};
        fd_devs[fd] = std::move(dev);
    }

    /* Map device identifier to file descriptor */
    uid_fds[uid] = fd;

    std::cout << "InputHandler: attached device " << uid << std::endl;
    signal();
}

template <class D>
void InputHandler::rm_device(std::string uid) {
    int fd{uid_fds[uid]};

    poll_fds.erase(std::remove_if(poll_fds.begin(), poll_fds.end(), [fd](struct pollfd& poll_fd){return poll_fd.fd == fd;}), poll_fds.end());

    uid_fds.erase(uid);
    fd_devs.erase(fd);

    std::cout << "InputHandler: detached device " << uid << std::endl;
}

}

#endif // INPUT_HANDLER_H

