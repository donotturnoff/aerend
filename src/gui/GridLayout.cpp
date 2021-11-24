#include "GridLayout.h"
#include <iostream>

namespace aerend {

GridLayout::GridLayout(int32_t cols, int32_t rows) : cols(cols), rows(rows), next_col(0), next_row(0) {}

void GridLayout::place(Container& parent, Widget& child) {
    int32_t pw = parent.get_w();
    int32_t ph = parent.get_h();
    int32_t px = parent.get_x();
    int32_t py = parent.get_y();

    int32_t w = pw/cols;
    int32_t h = ph/rows;
    int32_t x = px + next_col*w;
    int32_t y = py + next_row*h;

    child.set_pos(x, y);
    child.set_size(w, h);

    next_col = (next_col + 1) % cols;
    next_row = (next_col + 1) % rows;
}

}
