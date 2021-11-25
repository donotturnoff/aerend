#include "Panel.h"
#include <iostream>

namespace aerend {

Panel::Panel(std::shared_ptr<LayoutManager> lm, Colour colour, Border border) : rect(Rectangle{0, 0, 0, 0, colour, border}) {
    this->lm = lm;
}

void Panel::set_colour(Colour colour) {
    rect.set_colour(colour);
    autorepaint();
}

void Panel::set_pos(const int32_t x, const int32_t y) noexcept {
    this->x = x;
    this->y = y;
    rect.set_pos(x, y);
}

void Panel::set_size(const int32_t w, const int32_t h) {
    this->w = w;
    this->h = h;
    rect.set_size(w, h);
}

void Panel::paint(Bitmap& bmp) {
    rect.paint(bmp);
}

}
