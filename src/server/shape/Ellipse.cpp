#include "Ellipse.h"
#include "bitmap/SimpleBitmap.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace aerend {

const Border Ellipse::def_border{};

Ellipse::Ellipse(Client& client, int32_t x, int32_t y, int32_t w, int32_t h, Colour colour, Border border) : Shape{client, colour}, x(x), y(y), border(border) {}

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
    if (w < 0) throw std::invalid_argument("Ellipse::set_size: width cannot be negative");
    if (h < 0) throw std::invalid_argument("Ellipse::set_size: height cannot be negative");
    this->w = w;
    this->h = h;
}

void Ellipse::set_border(Border border) {
    this->border = border;
}

void Ellipse::paint(Bitmap& dst) {
    if (colour.a == 0) {
        return;
    }

    int32_t dst_w = dst.get_w();
    int32_t dst_h = dst.get_h();
    int32_t t = border.t;

    uint32_t* map = dst.get_map();
    uint32_t v = colour.to_int();
    uint32_t border_v = border.c.to_int();

    int32_t a = w/2, b = h/2, c = a+t, d = b+t;
    int32_t a2 = a*a, b2 = b*b, c2 = c*c, d2 = d*d;

    int32_t src_y0 = std::min(std::max(y-t, 0), dst_h);
    int32_t src_y1 = std::min(std::max(y+h+t, 0), dst_h);

    if (colour.a == 255) { /* Optimised code for opaque ellipses */
        for (int32_t j = src_y0; j < src_y1; j++) {
            int32_t src_y = j-b-y;
            int32_t xlim = a2-((int64_t)a2*src_y*src_y)/b2;
            int32_t border_xlim = c2-((int64_t)c2*src_y*src_y)/d2;
            int32_t off = j*dst_w+x+a;
            int32_t l, r;
            /* Fill row from centre to the left limit of ellipse */
            for (l = 0; l*l < xlim && x+a-l > 0; l++) {
                map[off-l-1] = v;
            }
            /* Then to the right limit of ellipse */
            for (r = 0; r*r < xlim && x+a+r < dst_w; r++) {
                map[off+r] = v;
            }
            if (t > 0 && border.c.a > 0) {
                if (border.c.a == 255) { /* Optimised code for opaque border */
                    /* Fill from left limit of ellipse to left limit of border */
                    for (; l*l < border_xlim && x+a-l > 0; l++) {
                        map[off-l-1] = border_v;
                    }
                    /* And the right */
                    for (; r*r < border_xlim && x+a+r < dst_w; r++) {
                        map[off+r] = border_v;
                    }
                } else { /* Transparent border */
                    for (; l*l < border_xlim && x+a-l > 0; l++) {
                        uint32_t dst_v2 = map[off-l-1];
                        if (dst_v2 <= 0xFFFFFF) { /* Completely transparent destination, no need to blend */
                            map[off-l-1] = border_v;
                        } else {
                            map[off-l-1] = Colour::src_over(dst_v2, border_v);
                        }
                    }
                    for (; r*r < border_xlim && x+a+r < dst_w; r++) {
                        uint32_t dst_v1 = map[off+r];
                        if (dst_v1 <= 0xFFFFFF) {
                            map[off+r] = border_v;
                        } else {
                            map[off+r] = Colour::src_over(dst_v1, border_v);
                        }
                    }
                }
            }
        }
    } else { /* Transparent ellipse */
        for (int32_t j = src_y0; j < src_y1; j++) {
            int32_t src_y = j-b-y;
            int32_t xlim = a2-((int64_t)a2*src_y*src_y)/b2;
            int32_t border_xlim = c2-((int64_t)c2*src_y*src_y)/d2;
            int32_t off = j*dst_w+x+a;
            int32_t l, r;
            for (l = 0; l*l < xlim && x+a-l > 0; l++) {
                uint32_t dst_v1 = map[off-l-1];
                if (dst_v1 <= 0xFFFFFF) {
                    map[off-l-1] = v;
                } else {
                    map[off-l-1] = Colour::src_over(dst_v1, v);
                }
            }
            for (r = 0; r*r < xlim && x+a+r < dst_w; r++) {
                uint32_t dst_v2 = map[off+r];
                if (dst_v2 <= 0xFFFFFF) {
                    map[off+r] = v;
                } else {
                    map[off+r] = Colour::src_over(dst_v2, v);
                }
            }
            if (t > 0 && border.c.a > 0) {
                if (border.c.a == 255) {
                    for (; l*l < border_xlim && x+a-l > 0; l++) {
                        map[off-l-1] = border_v;
                    }
                    for (; r*r < border_xlim && x+a+r < dst_w; r++) {
                        map[off+r] = border_v;
                    }
                } else {
                    for (; l*l < border_xlim && x+a-l > 0; l++) {
                        uint32_t dst_v2 = map[off-l-1];
                        if (dst_v2 <= 0xFFFFFF) {
                            map[off-l-1] = border_v;
                        } else {
                            map[off-l-1] = Colour::src_over(dst_v2, border_v);
                        }
                    }
                    for (; r*r < border_xlim && x+a+r < dst_w; r++) {
                        uint32_t dst_v1 = map[off+r];
                        if (dst_v1 <= 0xFFFFFF) {
                            map[off+r] = border_v;
                        } else {
                            map[off+r] = Colour::src_over(dst_v1, border_v);
                        }
                    }
                }
            }
        }
    }
}

}
