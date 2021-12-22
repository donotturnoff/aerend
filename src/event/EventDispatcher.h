#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

#include "Event.h"
#include "EventHandler.h"
#include <atomic>
#include <memory>
#include <set>
#include <queue>
#include <condition_variable>

namespace aerend {

class EventDispatcher {
public:
    void push_event(std::shared_ptr<Event> event);
    void run();
    void stop();
private:
    std::shared_ptr<Event> pop_event();
    std::queue<std::shared_ptr<Event>> queue;
    std::mutex q_mtx, q_cond_mtx;
    std::condition_variable q_cond;
    std::atomic<bool> running;
};

}

#endif // EVENT_DISPATCHER_H

