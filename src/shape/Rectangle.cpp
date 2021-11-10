#include "Rectangle.h"
#include "bitmap/SimpleBitmap.h"
#include <algorithm>
#include <cassert>
#include <iostream>

Rectangle::Rectangle(int32_t x, int32_t y, int32_t w, int32_t h, Colour colour, Border border) : x(x), y(y), w(w), h(h), border(border) {
    this->colour = colour;
}

void Rectangle::set_x(int32_t x) {
    this->x = x;
}

void Rectangle::set_y(int32_t y) {
    this->y = y;
}

void Rectangle::set_pos(int32_t x, int32_t y) {
    this->x = x;
    this->y = y;
}

void Rectangle::set_w(int32_t w) {
    set_size(w, h);
}

void Rectangle::set_h(int32_t h) {
    set_size(w, h);
}

void Rectangle::set_size(int32_t w, int32_t h) {
    this->w = w;
    this->h = h;
}

void Rectangle::set_border(Border border) {
    this->border = border;
}

SimpleBitmap Rectangle::create_bmp() {
    SimpleBitmap bmp {w, h};
    bmp.fill(colour);
    return bmp;
}

void Rectangle::paint(Bitmap& dst) {
    SimpleBitmap bmp{create_bmp()};
    if (colour.a == 255) {
        dst.composite(bmp, x, y, BlendMode::SRC);
    } else {
        dst.composite(bmp, x, y, BlendMode::SRC_OVER);
    }
}
