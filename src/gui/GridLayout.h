#ifndef GRID_LAYOUT_H
#define GRID_LAYOUT_H

#include "Container.h"
#include "Widget.h"
#include <cstdint>

namespace aerend {

class GridLayout : public LayoutManager {
public:
    GridLayout(int32_t cols=1, int32_t rows=1);
    void place(Container& parent, Widget& child);
    void reset();
private:
    int32_t cols, rows, next_col, next_row;
};

}

#endif // GRID_LAYOUT_H

