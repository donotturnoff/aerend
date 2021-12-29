#include "InputHandler.h"
#include "AerendServer.h"
#include "event/MouseEvent.h"
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <sys/epoll.h>
#include <sys/eventfd.h>

namespace aerend {

// TODO: integrate run here?
InputHandler::InputHandler() : epoll_fd(epoll_create1(0)), running(true) {
    stop_fd = eventfd(0, 0);
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = stop_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, stop_fd, &event);
    thread = std::thread(&InputHandler::run, this);
}

// TODO: integrate stop here?
InputHandler::~InputHandler() {
    running.store(false);
    int64_t sig = 1;
    write(stop_fd, &sig, 1);
    thread.join();
    close(epoll_fd);
}

void InputHandler::add_device(std::shared_ptr<InputDevice> dev) {
    int fd = dev->get_fd();
    devs[fd] = dev;
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
}

void InputHandler::run() {
    struct epoll_event events[MAX_EVENTS];
    EventDispatcher& ed = AerendServer::the().get_event_dispatcher();
    while (running.load()) {
        int32_t event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int32_t i = 0; i < event_count; i++) {
            if (events[i].events & EPOLLIN) {
                auto dev = devs[events[i].data.fd];
                auto evs = dev->get_events();
                if (!evs.empty()) {
                    for (const auto& ev: evs) {
                        ed.push_event(ev);
                    }
                }
            }
        }
    }
}

}
