#ifndef GRID_LAYOUT_H
#define GRID_LAYOUT_H

#include "Container.h"
#include "Widget.h"
#include <cstdint>
#include <vector>

namespace aerend {

class GridLayout : public LayoutManager {
public:
    GridLayout(int32_t cols=1, int32_t rows=1);
    GridLayout(std::vector<int32_t> x_props, std::vector<int32_t> y_props);
    void place(Container& parent, Widget& child);
    void reset();
private:
    std::vector<int32_t> x_props, y_props;
    int32_t next_col, next_row, next_x, next_y;
};

}

#endif // GRID_LAYOUT_H

