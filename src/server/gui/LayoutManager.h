#ifndef LAYOUT_MANAGER_H
#define LAYOUT_MANAGER_H

namespace aerend {

class Container;
class Widget;

class LayoutManager {
public:
    virtual ~LayoutManager() = default;
    virtual void place(Container& parent, Widget& child);
    virtual void reset();
};

}

#endif // LAYOUT_MANAGER_H

