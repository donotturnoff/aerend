#include "GridLayout.h"
#include "Container.h"
#include <numeric>
#include <iostream>

namespace aerend {

GridLayout::GridLayout(int32_t cols, int32_t rows) : x_props(std::vector<uint8_t>(cols, 1)), y_props(std::vector<uint8_t>(rows, 1)), x_props_sum(cols), y_props_sum(rows), cols(cols), rows(rows), regular(true) {}

GridLayout::GridLayout(std::vector<uint8_t> x_props, std::vector<uint8_t> y_props) : x_props(x_props), y_props(y_props), x_props_sum(std::accumulate(x_props.begin(), x_props.end(), decltype(x_props)::value_type(0))), y_props_sum(std::accumulate(y_props.begin(), y_props.end(), decltype(y_props)::value_type(0))), cols(x_props.size()), rows(y_props.size()), regular(false) {}

void GridLayout::place(Container& parent, Widget& child) {
    int32_t pw{parent.get_w()};
    int32_t ph{parent.get_h()};
    int32_t px{parent.get_x()};
    int32_t py{parent.get_y()};
    int32_t padding{parent.get_padding().t};
    int32_t border{child.get_border().t};
    int32_t margin{child.get_margin().t};

    int32_t cell_w{pw * x_props[next_col] / x_props_sum};
    int32_t cell_h{ph * y_props[next_row] / y_props_sum};

    int32_t padded_cell_w{cell_w};
    int32_t padded_cell_h{cell_h};
    if (next_col == 0 || next_col == cols-1) {
        padded_cell_w -= padding;
    }

    if (next_row == 0 || next_row == rows-1) {
        padded_cell_h -= padding;
    }

    /* Compute dimensions from preferred dimensions and parent padding */
    int32_t preferred_full_w{child.get_preferred_full_w()};
    int32_t preferred_full_h{child.get_preferred_full_h()};
    int32_t w{(preferred_full_w < 0) ? padded_cell_w : std::min(padded_cell_w, preferred_full_w)};
    int32_t h{(preferred_full_h < 0) ? padded_cell_h : std::min(padded_cell_h, preferred_full_h)};
    int32_t extra_x{(padded_cell_w - w)/2};
    int32_t extra_y{(padded_cell_h - h)/2};

    /* Compute position */
    if (next_col == 0) { /* In first column */
        extra_x += padding;
    }
    if (next_row == 0) { /* In first row */
        extra_y += padding;
    }
    extra_x += border + margin;
    extra_y += border + margin;
    int32_t x{px + next_x + extra_x};
    int32_t y{py + next_y + extra_y};

    child.set_pos(x, y);
    child.set_full_size(w, h);

    /* Prepare for next cell */
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

int32_t GridLayout::index_from_position(Container& parent, int32_t x, int32_t y) {
    auto padding{parent.get_padding().t};
    x -= parent.get_x() + padding;
    y -= parent.get_y() + padding;
    int32_t w{parent.get_w() - 2*padding};
    int32_t h{parent.get_h() - 2*padding};
    if (x < 0 || y < 0 || x >= w || y >= h) {
        return -1;
    }
    if (regular) { /* Regular grid */
        int32_t col{x*cols/w};
        int32_t row{y*rows/h};
        return row*cols+col;
    } else { /* Irregular grid */
        int32_t cum_x{0};
        int32_t col{0};
        /* Add column widths until target x is reached */
        for (; col < cols && cum_x < x; col++) {
            cum_x += w * x_props[col] / x_props_sum;
        }
        if (x <= cum_x) {
            col--;
        }

        int32_t cum_y{0};
        int32_t row{0};
        /* Same for rows */
        for (; row < rows && y > cum_y; row++) {
            cum_y += h * y_props[row] / y_props_sum;
        }
        if (y <= cum_y) {
            row--;
        }
        return row*cols+col;
    }
}

}
