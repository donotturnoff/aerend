#ifndef WIDGET_MOVE_ACTION_H
#define WIDGET_MOVE_ACTION_H

#include "WidgetUpdateAction.h"
#include "MouseEvent.h"
#include "gui/Widget.h"

namespace aerend {

enum class WidgetMoveActionValue {
    CONSTANT, MOUSE_DISPLACEMENT
};

template <typename E>
class WidgetMoveAction : public WidgetUpdateAction<E> {
public:
    WidgetMoveAction(Widget* widget, WidgetMoveActionValue value) : WidgetUpdateAction<E>(widget), value(value), dx(0), dy(0) {};
    WidgetMoveAction(Widget* widget, int32_t dx, int32_t dy) : WidgetUpdateAction<E>(widget), value(WidgetMoveActionValue::CONSTANT), dx(dx), dy(dy) {};
    void execute(std::shared_ptr<E> event);
private:
    WidgetMoveActionValue value;
    int32_t dx, dy;
};

}

#endif // WIDGET_MOVE_ACTION_H

