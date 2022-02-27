#include "Line.h"
#include <algorithm>
#include <iostream>

namespace aerend {

Line::Line(Client& client, int32_t x0, int32_t y0, int32_t x1, int32_t y1, Colour colour) : Shape(client, colour), x0(x0), y0(y0), x1(x1), y1(y1) {
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

uint8_t Line::classify_point(Bitmap& dst, int32_t x, int32_t y) {
    uint8_t code = 0;
    if (x < 0) {
        code = 1;
    } else if (x >= dst.get_w()) {
        code = 2;
    }
    if (y < 0) {
        code |= 4;
    } else if (y >= dst.get_h()) {
        code |= 8;
    }
    return code;
}

void Line::intersect(Bitmap& dst, int32_t& x0, int32_t& y0, int32_t x1, int32_t y1) {
    int32_t dst_w = dst.get_w();
    int32_t dst_h = dst.get_h();
    if (y0 >= dst_h) {
        x0 += (x1 - x0) * (dst_h-1 - y0) / (y1 - y0);
        y0 = dst_h;
    } else if (y0 < 0) {
        x0 += (x1 - x0) * -y0 / (y1 - y0);
        y0 = 0;
    }
    if (x0 >= dst_w) {
        y0 += (y1 - y0) * (dst_w-1 - x0) / (x1 - x0);
        x0 = dst_w;
    } else if (x0 < 0) {
        y0 += (y1 - y0) * -x0 / (x1 - x0);
        x0 = 0;
    }
}

void Line::paint(Bitmap& dst) {
    if (colour.a == 0) {
        return;
    }

    int32_t w = dst.get_w();
    int32_t v = colour.to_int();
    uint32_t* map = dst.get_map();
    int32_t x0_ = x0, y0_ = y0, x1_ = x1, y1_ = y1;

    uint8_t code0 = Line::classify_point(dst, x0_, y0_);
    uint8_t code1 = Line::classify_point(dst, x1_, y1_);

    if (code0 & code1) {
        return;
    } else if (!(code0 == 0 && code1 == 0)) {
        Line::intersect(dst, x0_, y0_, x1_, y1_);
        Line::intersect(dst, x1_, y1_, x0_, y0_);
    }

    int32_t dx = std::abs(x1_-x0_);
    int32_t sx = (x0_ < x1_) ? 1 : -1;
    int32_t dy = -std::abs(y1_-y0_);
    int32_t sy = (y0_ < y1_) ? 1 : -1;
    int32_t e = dx + dy;

    if (y0_ == y1_) {
        int32_t start = y0_*w+std::min(x0_, x1_);
        int32_t end = y0_*w+std::max(x0_, x1_);
        if (colour.a == 255) {
            std::fill(map+start, map+end+1, v);
        } else {
            for (int32_t i = start; i <= end; i++) {
                uint32_t dst_v = map[i];
                if (dst_v < 0xFFFFFF) {
                    map[i] = v;
                } else {
                    map[i] = Colour::src_over(dst_v, v);
                }
            }
        }
    } else if (x0_ == x1_) {
        int32_t start = std::min(y0_, y1_)*w+x0_;
        int32_t end = std::max(y0_, y1_)*w+x0_;
        if (colour.a == 255) {
            for (int32_t j = start; j <= end; j += w) {
                map[j] = v;
            }
        } else {
            for (int32_t j = start; j <= end; j += w) {
                uint32_t dst_v = map[j];
                if (dst_v < 0xFFFFFF) {
                    map[j] = v;
                } else {
                    map[j] = Colour::src_over(dst_v, v);
                }
            }
        }
    } else {
        if (colour.a == 255) {
            while (true) {
                map[y0_*w+x0_] = v;
                if (x0_ == x1_ && y0_ == y1_) {
                    break;
                }
                int32_t e2 = e * 2;
                if (e2 >= dy) {
                    x0_ += sx;
                    e += dy;
                }
                if (e2 <= dx) {
                    y0_ += sy;
                    e += dx;
                }
            }
        } else {
            while (true) {
                int32_t off = y0_*w+x0_;
                uint32_t dst_v = map[off];
                if (dst_v < 0xFFFFFF) {
                    map[off] = v;
                } else {
                    map[off] = Colour::src_over(dst_v, v);
                }

                if (x0_ == x1_ && y0_ == y1_) {
                    break;
                }
                int32_t e2 = e * 2;
                if (e2 >= dy) {
                    x0_ += sx;
                    e += dy;
                }
                if (e2 <= dx) {
                    y0_ += sy;
                    e += dx;
                }
            }
        }
    }
}

}
