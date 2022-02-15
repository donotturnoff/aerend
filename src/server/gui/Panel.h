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
    Panel(Client& client, std::unique_ptr<LayoutManager> lm = std::move(Panel::def_lm()), Colour bg_colour = Panel::def_bg_colour, Border border = Panel::def_border, Margin margin = Panel::def_margin, Padding padding = Panel::def_padding);
    void set_bg_colour(Colour bg_colour) noexcept;
    void set_border(Border border) noexcept;
    void set_margin(Margin margin) noexcept;
    void set_padding(Padding padding) noexcept;
    void set_pos(const int32_t x, const int32_t y) noexcept;
    void set_size(const int32_t w, const int32_t h);
    void paint(Bitmap& bmp);

    static std::unique_ptr<LayoutManager> def_lm();
    static const Colour def_bg_colour;
    static const Border def_border;
    static const Margin def_margin;
    static const Padding def_padding;
private:
    Rectangle rect;
};

}

#endif // PANEL_H

