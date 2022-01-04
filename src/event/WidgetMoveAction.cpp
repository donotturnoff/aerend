#include "WidgetMoveAction.h"
#include "MouseMoveEvent.h"
#include "AerendServer.h"
#include "gui/MergedUpdates.h"
#include <iostream>

namespace aerend {

template <>
void WidgetMoveAction<MouseMoveEvent>::execute(std::shared_ptr<MouseMoveEvent> event) {
    Widget* widget = this->widget;
    if (value == WidgetMoveActionValue::MOUSE_DISPLACEMENT) {
        AerendServer::the().get_display_manager().push_update(
                [widget] () {
                    AerendServer::the().get_display_manager().merged_updates->follow_mouse(widget);
                }
        );
    } else {
        int32_t dx = this->dx;
        int32_t dy = this->dy;
        AerendServer::the().get_display_manager().push_update(
                [widget, dx, dy] () {
                    AerendServer::the().get_display_manager().merged_updates->move_widget(widget, dx, dy);
                }
        );
    }
}

}
