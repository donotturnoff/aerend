#include "WidgetMap.h"
#include <algorithm>
#include <iostream>
#include <cstdio>

namespace aerend {

void WidgetMap::set_size(int32_t w, int32_t h) {
    map.resize(h, std::vector<Widget*>{(uint32_t) w, nullptr});
    for (auto& row: map) {
        row.resize(w, nullptr);
    }
}

void WidgetMap::add(Widget* widget) {
    fill(widget, widget->get_x(), widget->get_y(), widget->get_w(), widget->get_h());
}

void WidgetMap::rm(Widget* widget) {
    fill(nullptr, widget->get_x(), widget->get_y(), widget->get_w(), widget->get_h());
}

void WidgetMap::fill(Widget* widget, int32_t x, int32_t y, int32_t w, int32_t h) {
    int32_t map_h = map.size();
    if (map_h == 0) return;
    int32_t map_w = map[0].size();
    int32_t cx = std::max(std::min(x, map_w), 0);
    int32_t cy = std::max(std::min(y, map_h), 0);
    int32_t cw = w - (cx - x) - std::max(x+w-map_w, 0);
    int32_t ch = h - (cy - y) - std::max(y+h-map_h, 0);
    for (int32_t j = cy; j < cy+ch; j++) {
        std::fill_n(map[j].begin()+cx, cw, widget);
    }
}

void WidgetMap::clear() {
    for (auto& row: map) {
        std::fill(row.begin(), row.end(), nullptr);
    }
}

Widget* WidgetMap::get(int32_t x, int32_t y) {
    int32_t map_h = map.size();
    if (map_h == 0) return nullptr;
    int32_t map_w = map[0].size();
    if (x < 0 || x >= map_w || y < 0 || y >= map_h) {
        return nullptr;
    }
    return map[y][x];
}

int32_t WidgetMap::get_w() {
    if (map.size() == 0) return 0;
    return map[0].size();
}

int32_t WidgetMap::get_h() {
    return map.size();
}

}
