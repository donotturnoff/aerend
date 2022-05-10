#include "InputHandler.h"
#include "AerendServer.h"
#include "event/Event.h"
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <sys/eventfd.h>

namespace aerend {

InputHandler::InputHandler() {
    sig_fd = eventfd(1, 0);
    if (sig_fd < 0) {
        throw InputException{"failed to create signal eventfd", errno};
    }

    struct pollfd poll_fd = {sig_fd, POLLIN, 0};
    poll_fds.push_back(poll_fd);

    thread = std::thread(&InputHandler::run, this);
}

InputHandler::~InputHandler() {
    running.store(false);
    signal();
    thread.join();
}

void InputHandler::signal() {
    int64_t sig{1};
    write(sig_fd, &sig, 1);
}

void InputHandler::run() {
    EventDispatcher& ed{AerendServer::the().ed()};
    while (running.load()) {
        /* Poll device files */
        std::unique_lock<std::mutex> lock{poll_fds_mtx};
        auto event_count{poll(poll_fds.data(), poll_fds.size(), -1)};
        if (event_count > 0) {
            int32_t i{0};
            for (const auto& fd: poll_fds) {
                if (fd.fd == sig_fd) continue;
                if (fd.revents & POLLIN) {
                    /* Get input device */
                    InputDevice *dev;
                    {
                        std::unique_lock<std::mutex> lock{fd_devs_mtx};
                        dev = fd_devs[fd.fd].get();
                    }
                    try {
                        /* Push events from device to event dispatcher */
                        auto evs{dev->get_events()};
                        if (!evs.empty()) {
                            for (const auto& ev: evs) {
                                ed.push_event(ev);
                            }
                        }
                    } catch (InputException& ie) {
                        std::cerr << "InputHandler: " << ie.what() << std::endl;
                    }
                }
                if (++i > event_count) break; /* All events read */
            }
        } else if (event_count < 0) {
            auto err{errno};
            std::cerr << "InputHandler: poll failed: " << errno << std::endl;
            if (err == EINTR) break;
        }
    }
}

}
