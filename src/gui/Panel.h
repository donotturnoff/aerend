#ifndef PANEL_H
#define PANEL_H

#include "Container.h"
#include "LayoutManager.h"
#include "GridLayout.h"
#include "shape/Rectangle.h"
#include "style/Colour.h"
#include "style/Border.h"
#include "style/Margin.h"
#include <cstdint>
#include <memory>

namespace aerend {

class Panel : public Container {
public:
    Panel(Client& client, std::unique_ptr<LayoutManager> lm = std::make_unique<GridLayout>(1, 1), Colour bg_colour = Colour::white(), Border border = Border{}, Margin margin = Margin{});
    void set_bg_colour(Colour bg_colour) noexcept;
    void set_border(Border border) noexcept;
    void set_pos(const int32_t x, const int32_t y) noexcept;
    void set_size(const int32_t w, const int32_t h);
    void paint(Bitmap& bmp);
private:
    Rectangle rect;
};

}

#endif // PANEL_H

