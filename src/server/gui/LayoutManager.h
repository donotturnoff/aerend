#ifndef LAYOUT_MANAGER_H
#define LAYOUT_MANAGER_H

#include <cstdint>

namespace aerend {

class Container;
class Widget;

class LayoutManager {
public:
    virtual ~LayoutManager() = default;
    virtual void place(Container& parent, Widget& child);
    virtual void reset();
    virtual int32_t index_from_position(Container& parent, int32_t x, int32_t y);
};

}

#endif // LAYOUT_MANAGER_H

