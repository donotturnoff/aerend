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

void Ellipse::paint(Bitmap& dst) {
    int32_t dst_w = dst.get_w();
    int32_t dst_h = dst.get_h();
    assert(x >= 0);
    assert(y >= 0);
    assert(x+w < dst_w);
    assert(y+h < dst_h);
    uint32_t* map = dst.get_map();
    uint32_t v = colour.to_int();
    int32_t cx = w/2;
    int32_t cy = h/2;
    if (colour.a == 0) {
        return;
    } else if (colour.a == 255) {
        for (int32_t j = 0; j < cy; j++) {
            int32_t off_base1 = (y+cy+j)*dst_w;
            int32_t off_base2 = (y+cy-j-1)*dst_w;
            int32_t a2 = cx*cx;
            int32_t b2 = cy*cy;
            int32_t xlim = a2-(a2*j*j)/b2;
            int32_t i = 0;
            while (i*i < xlim) {
                int32_t x1 = cx+i;
                int32_t x2 = cx-i-1;
                map[off_base1+x+x1] = v;
                map[off_base1+x+x2] = v;
                map[off_base2+x+x1] = v;
                map[off_base2+x+x2] = v;
                i++;
            }
        }
    } else {
        for (int32_t j = 0; j < cy; j++) {
            int32_t off_base1 = (y+cy+j)*dst_w;
            int32_t off_base2 = (y+cy-j-1)*dst_w;
            int32_t a2 = cx*cx;
            int32_t b2 = cy*cy;
            int32_t xlim = a2-(a2*j*j)/b2;
            int32_t i = 0;
            while (i*i < xlim) {
                int32_t x1 = cx+i;
                int32_t x2 = cx-i-1;
                uint32_t dst_v1 = map[off_base1+x+x1];
                uint32_t dst_v2 = map[off_base1+x+x2];
                uint32_t dst_v3 = map[off_base2+x+x1];
                uint32_t dst_v4 = map[off_base2+x+x2];
                map[off_base1+x+x1] = Colour::src_over(dst_v1, v);
                map[off_base1+x+x2] = Colour::src_over(dst_v2, v);
                map[off_base2+x+x1] = Colour::src_over(dst_v3, v);
                map[off_base2+x+x2] = Colour::src_over(dst_v4, v);
                i++;
            }
        }
    }
}
