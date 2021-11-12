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
    if (colour.a == 0) {
        return;
    }

    int32_t dst_w = dst.get_w();
    int32_t dst_h = dst.get_h();
    int32_t t = border.t;
    uint32_t* map = dst.get_map();
    uint32_t v = colour.to_int();

    // Clip bounds
    int32_t src_x0 = std::min(std::max(x, 0), dst_w);
    int32_t src_y0 = std::min(std::max(y, 0), dst_h);
    int32_t src_x1 = std::min(std::max(x+w, 0), dst_w);
    int32_t src_y1 = std::min(std::max(y+h, 0), dst_h);
    int32_t src_border_x0 = std::min(std::max(x-t, 0), dst_w);
    int32_t src_border_y0 = std::min(std::max(y-t, 0), dst_h);
    int32_t src_border_x1 = std::min(std::max(x+w+t, 0), dst_w);
    int32_t src_border_y1 = std::min(std::max(y+h+t, 0), dst_h);

    // Rectangle
    if (colour.a == 255) {
        for (int32_t j = src_y0; j < src_y1; j++) {
            uint32_t* row = map+j*dst_w;
            uint32_t* start = row + src_x0;
            uint32_t* end = row + src_x1;
            std::fill(start, end, v);
        }
    } else {
        for (int32_t j = src_y0; j < src_y1; j++) {
            int32_t row = j*dst_w;
            for (int32_t i = src_x0; i < src_x1; i++) {
                int32_t off = row + i;
                uint32_t dst_v = map[off];
                if (dst_v <= 0xFFFFFF) {
                    map[off] = v;
                } else {
                    map[off] = Colour::src_over(map[off], v);
                }
            }
        }
    }

    // Border
    v = border.c.to_int();
    if (border.t > 0 && border.c.a > 0) {
        if (border.c.a == 255) {
            for (int32_t j = src_border_y0; j < src_y0; j++) {
                uint32_t* row = map+j*dst_w;
                uint32_t* start = row+src_border_x0;
                uint32_t* end = row+src_border_x1;
                std::fill(start, end, v);
            }
            for (int32_t j = src_y0; j < src_y1; j++) {
                uint32_t* row = map+j*dst_w;
                uint32_t* start = row + src_border_x0;
                uint32_t* end = row + src_x0;
                std::fill(start, end, v);
                start = row + src_x1;
                end = row + src_border_x1;
                std::fill(start, end, v);
            }
            for (int32_t j = src_y1; j < src_border_y1; j++) {
                uint32_t* row = map+j*dst_w;
                uint32_t* start = row+src_border_x0;
                uint32_t* end = row+src_border_x1;
                std::fill(start, end, v);
            }
        } else {
            for (int32_t j = src_border_y0; j < src_y0; j++) {
                int32_t start = j*dst_w;
                for (int32_t i = src_border_x0; i < src_border_x1; i++) {
                    int32_t off = start + i;
                    uint32_t dst_v = map[off];
                    if (dst_v <= 0xFFFFFF) {
                        map[off] = v;
                    } else {
                        map[off] = Colour::src_over(map[off], v);
                    }
                }
            }
            for (int32_t j = src_y0; j < src_y1; j++) {
                int32_t start = j*dst_w;
                for (int32_t i = src_border_x0; i < src_x0; i++) {
                    int32_t off = start + i;
                    uint32_t dst_v = map[off];
                    if (dst_v <= 0xFFFFFF) {
                        map[off] = v;
                    } else {
                        map[off] = Colour::src_over(map[off], v);
                    }
                }
                start = j*dst_w;
                for (int32_t i = src_x1; i < src_border_x1; i++) {
                    int32_t off = start + i;
                    uint32_t dst_v = map[off];
                    if (dst_v <= 0xFFFFFF) {
                        map[off] = v;
                    } else {
                        map[off] = Colour::src_over(map[off], v);
                    }
                }
            }
            for (int32_t j = src_y1; j < src_border_y1; j++) {
                int32_t start = j*dst_w;
                for (int32_t i = src_border_x0; i < src_border_x1; i++) {
                    int32_t off = start + i;
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
}
