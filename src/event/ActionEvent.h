#ifndef ACTION_EVENT_H
#define ACTION_EVENT_H

#include "Event.h"
#include "gui/Widget.h"
#include <cstdint>

namespace aerend {

class ActionEvent: public Event {
public:
    ActionEvent(Widget* widget, bool left, bool middle, bool right);
    Widget* get_widget() const noexcept;
    bool get_left() const noexcept;
    bool get_middle() const noexcept;
    bool get_right() const noexcept;
private:
    Widget* widget;
    bool left, middle, right;
};

}

#endif // ACTION_EVENT_H

