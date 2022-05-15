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
    void rm_from_under_mouse(Widget* widget);
    void dispatch(Event& event);
private:
    Widget* last_under_mouse{nullptr};
};

}

#endif // EVENT_DISPATCHER_H

