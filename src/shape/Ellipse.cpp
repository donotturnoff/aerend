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
    int32_t t = border.t;
    assert(x-t >= 0);
    assert(y-t >= 0);
    assert(x+w+t < dst_w);
    assert(y+h+t < dst_h);
    if (colour.a == 0) {
        return;
    }
    uint32_t* map = dst.get_map();
    uint32_t v = colour.to_int();
    uint32_t border_v = border.c.to_int();
    int32_t cx = w/2;
    int32_t cy = h/2;
    if (colour.a == 255) {
        for (int32_t j = -cy-t; j < cy+t; j++) {
            int32_t off = (y+j)*dst_w+x+cx;
            int32_t a2 = cx*cx;
            int32_t b2 = cy*cy;
            int32_t c2 = (cx+t)*(cx+t);
            int32_t d2 = (cy+t)*(cy+t);
            int32_t xlim = a2-(a2*j*j)/b2;
            int32_t border_xlim = c2-(c2*j*j)/d2;
            int32_t i = 0;
            while (i*i < xlim) {
                map[off+i] = map[off-i-1] = v;
                i++;
            }
            if (t > 0 && border.c.a > 0) {
                if (border.c.a == 255) {
                    while (i*i < border_xlim) {
                        map[off+i] = map[off-i-1] = border_v;
                        i++;
                    }
                } else {
                    while (i*i < border_xlim) {
                        uint32_t dst_v1 = map[off+i];
                        if (dst_v1 <= 0xFFFFFF) {
                            map[off+i] = border_v;
                        } else {
                            map[off+i] = Colour::src_over(dst_v1, border_v);
                        }
                        uint32_t dst_v2 = map[off-i-1];
                        if (dst_v2 <= 0xFFFFFF) {
                            map[off-i-1] = border_v;
                        } else {
                            map[off-i-1] = Colour::src_over(dst_v2, border_v);
                        }
                        i++;
                    }
                }
            }
        }
    } else {
        for (int32_t j = -cy-t; j < cy+t; j++) {
            int32_t off = (y+j)*dst_w+x+cx;
            int32_t a2 = cx*cx;
            int32_t b2 = cy*cy;
            int32_t c2 = (cx+t)*(cx+t);
            int32_t d2 = (cy+t)*(cy+t);
            int32_t xlim = a2-(a2*j*j)/b2;
            int32_t border_xlim = c2-(c2*j*j)/d2;
            int32_t i = 0;
            while (i*i < xlim) {
                uint32_t dst_v1 = map[off+i];
                if (dst_v1 <= 0xFFFFFF) {
                    map[off+i] = v;
                } else {
                    map[off+i] = Colour::src_over(dst_v1, v);
                }
                uint32_t dst_v2 = map[off-i-1];
                if (dst_v2 <= 0xFFFFFF) {
                    map[off-i-1] = v;
                } else {
                    map[off-i-1] = Colour::src_over(dst_v2, v);
                }
                i++;
            }
            if (t > 0 && border.c.a > 0) {
                if (border.c.a == 255) {
                    while (i*i < border_xlim) {
                        map[off+i] = map[off-i-1] = border_v;
                        i++;
                    }
                } else {
                    while (i*i < border_xlim) {
                        uint32_t dst_v1 = map[off+i];
                        if (dst_v1 <= 0xFFFFFF) {
                            map[off+i] = border_v;
                        } else {
                            map[off+i] = Colour::src_over(dst_v1, border_v);
                        }
                        uint32_t dst_v2 = map[off-i-1];
                        if (dst_v2 <= 0xFFFFFF) {
                            map[off-i-1] = border_v;
                        } else {
                            map[off-i-1] = Colour::src_over(dst_v2, border_v);
                        }
                        i++;
                    }
                }
            }
        }
    }
}
