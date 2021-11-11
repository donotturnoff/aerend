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

void Rectangle::paint(Bitmap& dst) {
    int32_t dst_w = dst.get_w();
    int32_t dst_h = dst.get_h();
    assert(x >= 0);
    assert(y >= 0);
    assert(x+w < dst_w);
    assert(y+h < dst_h);
    uint32_t* map = dst.get_map();
    uint32_t v = colour.to_int();
    if (colour.a == 0) {
        return;
    } else if (colour.a == 255) {
        for (int32_t j = y; j < y+h; j++) {
            uint32_t* off_base = map+j*dst_w+x;
            std::fill(off_base, off_base+w, v);
        }
    } else {
        for (int32_t j = y; j < y+h; j++) {
            int32_t off_base = j*dst_w;
            for (int32_t i = x; i < x+w; i++) {
                int32_t off = off_base + i;
                uint32_t dst_v = map[off];
                if (dst_v <= 0xFFFFFF) {
                    map[off] = v;
                } else {
                    map[off] = Colour::src_over(map[off], v);
                }
            }
        }
    }
}
