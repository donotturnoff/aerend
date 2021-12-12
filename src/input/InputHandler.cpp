#include "InputHandler.h"
#include "event/MouseEvent.h"
#include <unistd.h>
#include <cstdio>
#include <sys/epoll.h>

namespace aerend {

InputHandler::InputHandler(EventDispatcher& ed) : epoll_fd(epoll_create1(0)), ed(ed) {}

InputHandler::~InputHandler() {
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
    while (running.load()) {
        int32_t event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int32_t i = 0; i < event_count; i++) {
            if (events[i].events & EPOLLIN) {
                auto dev = devs[events[i].data.fd];
                auto evs = dev->get_events();
                if (!evs.empty()) {
                    for (const auto& ev: evs) {
                        EventType type = ev->get_type();
                        if (type == EventType::MOUSE_MOVE || type == EventType::MOUSE_CLICK || type == EventType::MOUSE_PRESS || type == EventType::MOUSE_RELEASE || type == EventType::MOUSE_SCROLL) {
                            MouseEvent* me = (MouseEvent*) ev.get();
                            if (type == EventType::MOUSE_CLICK) {
                                std::printf("CLICK ");
                            } else if (type == EventType::MOUSE_PRESS) {
                                std::printf("PRESS ");
                            } else if (type == EventType::MOUSE_RELEASE) {
                                std::printf("RELEASE ");
                            }
                            std::printf("x=%d y=%d dx=%d dy=%d sx=%d sy=%d left=%d right=%d\n", me->get_x(), me->get_y(), me->get_dx(), me->get_dy(), me->get_scroll_x(), me->get_scroll_y(), me->get_left(), me->get_right());
                        }
                    }
                }
            }
        }
    }
}

}
