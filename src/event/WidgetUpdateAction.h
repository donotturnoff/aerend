#ifndef WIDGET_UPDATE_ACTION_H
#define WIDGET_UPDATE_ACTION_H

#include "Action.h"
#include "MouseEvent.h"
#include "gui/Widget.h"

namespace aerend {

template <typename E>
class WidgetUpdateAction : public Action<E> {
public:
    WidgetUpdateAction(Widget* widget) : widget(widget) {};
protected:
    Widget* widget;
};

}

#endif // WIDGET_UPDATE_ACTION_H

