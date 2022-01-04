#ifndef WIDGET_UPDATE_ACTION_H
#define WIDGET_UPDATE_ACTION_H

#include "Action.h"
#include "MouseEvent.h"
#include "gui/Update.h"
#include "gui/Widget.h"

namespace aerend {

template <typename E>
class WidgetUpdateAction : public Action<E> {
public:
    WidgetUpdateAction(UpdateType type, Widget* widget) : type(type), widget(widget) {};
protected:
    UpdateType type;
    Widget* widget;
};

}

#endif // WIDGET_UPDATE_ACTION_H

