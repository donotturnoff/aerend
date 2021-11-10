#include "BackedEllipse.h"
#include <algorithm>
#include <cassert>
#include <iostream>

BackedEllipse::BackedEllipse(int32_t x, int32_t y, int32_t w, int32_t h, Colour colour, Border border) : Ellipse(x, y, w, h, colour, border) {
    bmp = create_bmp();
}

void BackedEllipse::set_size(int32_t w, int32_t h) {
    this->w = w;
    this->h = h;
    bmp = create_bmp();
}

void BackedEllipse::set_colour(Colour colour) {
    this->colour = colour;
    bmp = create_bmp();
}

void BackedEllipse::set_border(Border border) {
    this->border = border;
}

void BackedEllipse::paint(Bitmap& dst) {
    if (colour.a == 255) {
        dst.composite(bmp, x, y, BlendMode::SRC);
    } else {
        dst.composite(bmp, x, y, BlendMode::SRC_OVER);
    }
}
