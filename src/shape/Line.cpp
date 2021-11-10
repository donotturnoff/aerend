#include "Line.h"
#include <algorithm>
#include <cassert>
#include <iostream>

Line::Line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, Colour colour) : x0(x0), y0(y0), x1(x1), y1(y1) {
    this->colour = colour;
}

void Line::set_x0(int32_t x0) {
    this->x0 = x0;
}

void Line::set_y0(int32_t y0) {
    this->y0 = y0;
}

void Line::set_start(int32_t x0, int32_t y0) {
    this->x0 = x0;
    this->y0 = y0;
}

void Line::set_x1(int32_t x1) {
    this->x1 = x1;
}

void Line::set_y1(int32_t y1) {
    this->y1 = y1;
}

void Line::set_end(int32_t x1, int32_t y1) {
    this->x1 = x1;
    this->y1 = y1;
}

void Line::paint(Bitmap& dst) {
    int32_t v = colour.to_int();
    uint32_t* map = dst.get_map();
    int32_t w = dst.get_w();
    if (colour.a == 255) {
        int32_t x = x0;
        int32_t y = y0;
        int32_t dx = std::abs(x1-x0);
        int32_t sx = (x0 < x1) ? 1 : -1;
        int32_t dy = -std::abs(y1-y0);
        int32_t sy = (y0 < y1) ? 1 : -1;
        int32_t e = dx + dy;
        while (true) {
            map[y*w+x] = v;
            if (x == x1 && y == y1) {
                break;
            }
            int32_t e2 = e * 2;
            if (e2 >= dy) {
                x += sx;
                e += dy;
            }
            if (e2 <= dx) {
                y += sy;
                e += dx;
            }
        }
    } else {
    }
}
