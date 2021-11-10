#include "BackedRectangle.h"
#include <algorithm>
#include <cassert>
#include <iostream>

BackedRectangle::BackedRectangle(int32_t x, int32_t y, int32_t w, int32_t h, Colour colour, Border border) : Rectangle(x, y, w, h, colour, border) {
    bmp = create_bmp();
}

void BackedRectangle::set_size(int32_t w, int32_t h) {
    this->w = w;
    this->h = h;
    bmp = create_bmp();
}

void BackedRectangle::set_colour(Colour colour) {
    this->colour = colour;
    bmp = create_bmp();
}

void BackedRectangle::set_border(Border border) {
    this->border = border;
    bmp = create_bmp();
}

void BackedRectangle::paint(Bitmap& dst) {
    if (colour.a == 255) {
        dst.composite(bmp, x, y, BlendMode::SRC);
    } else {
        dst.composite(bmp, x, y, BlendMode::SRC_OVER);
    }
}
