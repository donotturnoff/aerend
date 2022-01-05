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
    void dispatch(std::shared_ptr<Event> event, std::vector<std::function<void(std::shared_ptr<Event>)>> handlers);
private:
    std::shared_ptr<Event> pop_event();
    void run();
    std::thread thread;
    std::queue<std::shared_ptr<Event>> queue;
    std::mutex q_mtx;
    std::condition_variable q_cond;
    std::atomic<bool> running;
};

}

#endif // EVENT_DISPATCHER_H

