#ifndef GRID_LAYOUT_H
#define GRID_LAYOUT_H

#include "Widget.h"
#include "LayoutManager.h"
#include <cstdint>
#include <vector>

namespace aerend {

class Container;

class GridLayout : public LayoutManager {
public:
    GridLayout(int32_t cols=1, int32_t rows=1);
    GridLayout(std::vector<int16_t> x_props, std::vector<int16_t> y_props);
    void place(Container& parent, Widget& child);
    void reset();
private:
    std::vector<int16_t> x_props, y_props;
    int32_t next_col = 0, next_row = 0, next_x = 0, next_y = 0;
};

}

#endif // GRID_LAYOUT_H

