#ifndef PANEL_H
#define PANEL_H

#include "Container.h"
#include "LayoutManager.h"
#include "GridLayout.h"
#include "shape/Rectangle.h"
#include "util/Colour.h"
#include "util/Border.h"
#include <cstdint>
#include <memory>

namespace aerend {

class Panel : public Container {
public:
    Panel(std::shared_ptr<LayoutManager> lm = std::make_shared<GridLayout>(1, 1), Colour colour = Colour::white(), Border border = Border{});
    void set_colour(Colour colour);
    void set_pos(const int32_t x, const int32_t y) noexcept;
    void set_size(const int32_t w, const int32_t h);
    void paint(Bitmap& bmp);
private:
    Rectangle rect;
};

}

#endif // PANEL_H

