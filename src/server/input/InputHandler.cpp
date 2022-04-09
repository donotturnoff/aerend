#include "InputHandler.h"
#include "AerendServer.h"
#include "event/Event.h"
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <sys/eventfd.h>

namespace aerend {

InputHandler::InputHandler() : epoll_fd(epoll_create1(0)), running(true) {
    if (epoll_fd < 0) {
        throw InputException{"failed to create epoll instance", errno};
    }
    halt_fd = eventfd(0, 0);
    if (halt_fd < 0) {
        throw InputException{"failed to create halt eventfd", errno};
    }
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = halt_fd;
    int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, halt_fd, &event);
    if (ret < 0) {
        throw InputException{"failed to add halt eventfd to epoll", errno};
    }
    thread = std::thread(&InputHandler::run, this);
}

InputHandler::~InputHandler() {
    running.store(false);
    int64_t sig = 1;
    write(halt_fd, &sig, 1);
    thread.join();
    close(epoll_fd);
}

void InputHandler::run() {
    struct epoll_event events[MAX_EVENTS];
    EventDispatcher& ed = AerendServer::the().ed();
    while (running.load()) {
        int32_t event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int32_t i = 0; i < event_count; i++) {
            if (events[i].events & EPOLLIN) {
                auto dev = fd_devs[events[i].data.fd].get();
                try {
                    auto evs = dev->get_events();
                    if (!evs.empty()) {
                        for (const auto& ev: evs) {
                            ed.push_event(ev);
                        }
                    }
                } catch (InputException& ie) {
                    std::cerr << "InputHandler::run(): " << ie.what() << std::endl;
                }
            }
        }
    }
}

}
