#include "Ellipse.h"
#include "bitmap/SimpleBitmap.h"
#include <algorithm>
#include <cassert>
#include <iostream>

Ellipse::Ellipse(int32_t x, int32_t y, int32_t w, int32_t h, Colour colour, Border border) : x(x), y(y), w(w), h(h), border(border) {
    this->colour = colour;
}

void Ellipse::set_x(int32_t x) {
    this->x = x;
}

void Ellipse::set_y(int32_t y) {
    this->y = y;
}

void Ellipse::set_pos(int32_t x, int32_t y) {
    this->x = x;
    this->y = y;
}

void Ellipse::set_w(int32_t w) {
    set_size(w, h);
}

void Ellipse::set_h(int32_t h) {
    set_size(w, h);
}

void Ellipse::set_size(int32_t w, int32_t h) {
    this->w = w;
    this->h = h;
}

void Ellipse::set_border(Border border) {
    this->border = border;
}

SimpleBitmap Ellipse::create_bmp() {
    SimpleBitmap bmp{w, h};
    uint32_t* map = bmp.get_map();
    int32_t cx = w/2;
    int32_t cy = h/2;
    uint32_t v = colour.to_int();
    for (int32_t j = 0; j < cy; j++) {
        int32_t off1 = (cy+j)*w;
        int32_t off2 = (cy-j-1)*w;
        int32_t a2 = cx*cx;
        int32_t b2 = cy*cy;
        int32_t xlim = a2-(a2*j*j)/b2;
        int32_t i = 0;
        while (i*i < xlim) {
            int32_t x1 = cx+i;
            int32_t x2 = cx-i-1;
            map[off1+x1] = v;
            map[off1+x2] = v;
            map[off2+x1] = v;
            map[off2+x2] = v;
            i++;
        }
        while (i < cx) {
            int32_t x1 = cx+i;
            int32_t x2 = cx-i-1;
            map[off1+x1] = 0;
            map[off1+x2] = 0;
            map[off2+x1] = 0;
            map[off2+x2] = 0;
            i++;
        }
    }
    return bmp;
}

void Ellipse::paint(Bitmap& dst) {
    SimpleBitmap bmp {create_bmp()};
    if (colour.a == 255) {
        dst.composite(bmp, x, y, BlendMode::SRC);
    } else {
        dst.composite(bmp, x, y, BlendMode::SRC_OVER);
    }
}
