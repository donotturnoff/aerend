#include "Panel.h"
#include <iostream>

namespace aerend {

Panel::Panel(Client& client, std::unique_ptr<LayoutManager> lm, Colour colour, Border border, Margin margin, Padding padding) : Container(client, std::move(lm), bg_colour, border, margin, padding), rect(Rectangle{0, 0, 0, 0, colour, border}) {}

void Panel::set_bg_colour(Colour bg_colour) noexcept {
    this->bg_colour = bg_colour;
    rect.set_colour(bg_colour);
    autorepaint();
}

void Panel::set_border(Border border) noexcept {
    this->border = border;
    rect.set_border(border);
    if (parent) {
        parent->layout();
        parent->autorepaint();
    }
}

void Panel::set_margin(Margin margin) noexcept {
    this->margin = margin;
    if (parent) {
        parent->layout();
        parent->autorepaint();
    }
}

void Panel::set_padding(Padding padding) noexcept {
    this->padding = padding;
    if (parent) {
        parent->layout();
        parent->autorepaint();
    }
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
