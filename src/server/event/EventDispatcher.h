#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

#include "Event.h"
#include <atomic>
#include <memory>
#include <set>
#include <queue>
#include <thread>
#include <condition_variable>
#include <functional>
#include <vector>

namespace aerend {

class EventDispatcher {
public:
    EventDispatcher();
    ~EventDispatcher();
    void push_event(std::shared_ptr<Event> event);
    void rm_from_under_mouse(Widget* widget);
private:
    std::shared_ptr<Event> pop_event();
    void run();
    std::thread thread;
    std::queue<std::shared_ptr<Event>> queue;
    std::mutex q_mtx;
    std::condition_variable q_cond;
    std::atomic<bool> running;
    Widget* last_under_mouse;
};

}

#endif // EVENT_DISPATCHER_H
