#include "Label.h"
#include "Container.h"
#include <iostream>
#include <cstdio>

namespace aerend {

Label::Label(Client& client, std::string str, std::string font_path, int32_t font_size, Colour colour, Colour bg_colour, Border border, Margin margin) : Widget(client, bg_colour, border, margin), rect(Rectangle{0, 0, 0, 0, bg_colour, border}), text(Text{str, font_path, font_size, colour, 0, 0, -1}), bmp(SimpleBitmap{}) {}

void Label::set_str(std::string str) {
    text.set_str(str);
    autorepaint();
}

void Label::set_colour(Colour colour) {
    text.set_colour(colour);
    autorepaint();
}

void Label::set_bg_colour(Colour bg_colour) noexcept {
    this->bg_colour = bg_colour;
    rect.set_colour(bg_colour);
    autorepaint();
}

void Label::set_border(Border border) noexcept {
    this->border = border;
    rect.set_border(border);
    if (parent) {
        parent->autolayout();
        parent->autorepaint();
    }
}

void Label::set_wrap(const int32_t wrap) {
    text.set_wrap(wrap);
    autorepaint();
}

void Label::set_pos(const int32_t x, const int32_t y) noexcept {
    this->x = x;
    this->y = y;
    rect.set_pos(x, y);
}

void Label::set_size(const int32_t w, const int32_t h) {
    this->w = w;
    this->h = h;
    rect.set_size(w, h);
    bmp.set_size(w, h);
    bmp.fill(Colour::white(0));
}

void Label::paint(Bitmap& dst) {
    rect.paint(dst);
    text.paint(bmp);
    dst.composite(bmp, x, y);
}

}
