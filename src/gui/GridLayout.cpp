#include "GridLayout.h"
#include <numeric>
#include <iostream>

namespace aerend {

GridLayout::GridLayout(int32_t cols, int32_t rows) : x_props(std::vector<int32_t>(cols, 1)), y_props(std::vector<int32_t>(rows, 1)), next_col(0), next_row(0), next_x(0), next_y(0) {}

GridLayout::GridLayout(std::vector<int32_t> x_props, std::vector<int32_t> y_props) : x_props(x_props), y_props(y_props), next_col(0), next_row(0), next_x(0), next_y(0) {}

void GridLayout::place(Container& parent, Widget& child) {
    int32_t pw = parent.get_w();
    int32_t ph = parent.get_h();
    int32_t px = parent.get_x();
    int32_t py = parent.get_y();
    int32_t padding = parent.get_padding().t;
    int32_t border = child.get_border().t;
    int32_t margin = child.get_margin().t;

    int32_t cols = x_props.size();
    int32_t rows = y_props.size();

    int32_t x_props_sum = std::accumulate(x_props.begin(), x_props.end(), decltype(x_props)::value_type(0));
    int32_t y_props_sum = std::accumulate(y_props.begin(), y_props.end(), decltype(y_props)::value_type(0));

    int32_t cell_w = pw * x_props[next_col] / x_props_sum;
    int32_t cell_h = ph * y_props[next_row] / y_props_sum;

    int32_t padded_cell_w = cell_w;
    int32_t padded_cell_h = cell_h;
    if (next_col == 0 || next_col == cols-1) {
        padded_cell_w -= padding;
    }

    if (next_row == 0 || next_row == rows-1) {
        padded_cell_h -= padding;
    }

    int32_t preferred_full_w = child.get_preferred_full_w();
    int32_t preferred_full_h = child.get_preferred_full_h();
    int32_t w = (preferred_full_w < 0) ? padded_cell_w : std::min(padded_cell_w, preferred_full_w);
    int32_t h = (preferred_full_h < 0) ? padded_cell_h : std::min(padded_cell_h, preferred_full_h);
    int32_t extra_x = (padded_cell_w - w)/2;
    int32_t extra_y = (padded_cell_h - h)/2;

    if (next_col == 0) {
        extra_x += padding;
    }

    if (next_row == 0) {
        extra_y += padding;
    }

    extra_x += border + margin;
    extra_y += border + margin;

    int32_t x = px + next_x + extra_x;
    int32_t y = py + next_y + extra_y;

    child.set_pos(x, y);
    child.set_full_size(w, h);

    next_col = (next_col + 1) % cols;
    next_x += cell_w;
    if (next_x >= pw) {
        next_x = 0;
    }
    if (next_col == 0) {
        next_y += cell_h;
        if (next_y >= ph) {
            next_y = 0;
        }
        next_row = (next_col + 1) % rows;
    }
}

void GridLayout::reset() {
    next_col = 0;
    next_row = 0;
    next_x = 0;
    next_y = 0;
}

}
