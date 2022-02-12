#ifndef WIDGET_MAP_H
#define WIDGET_MAP_H

#include "Widget.h"
#include <cstdint>
#include <vector>

namespace aerend {

class WidgetMap {
public:
    void set_size(int32_t w, int32_t h);
    void clear();
    void add(Widget* widget);
    void rm(Widget* widget);
    Widget* get(int32_t x, int32_t y);
    int32_t get_w();
    int32_t get_h();
private:
    void fill(Widget* widget, int32_t x, int32_t y, int32_t w, int32_t h);
    std::vector<std::vector<Widget*>> map;
};

}

#endif // WIDGET_MAP_H

