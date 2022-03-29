#include "Bitmap.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace aerend {

void Bitmap::set_w(const int32_t w) {
    set_size(w, h);
}

void Bitmap::set_h(const int32_t h) {
    set_size(w, h);
}

uint32_t* Bitmap::get_map() const noexcept {
    return map;
}

int32_t Bitmap::get_w() const noexcept {
    return w;
}

int32_t Bitmap::get_h() const noexcept {
    return h;
}

int32_t Bitmap::get_size() const noexcept {
    return size;
}

void Bitmap::set_pixel(const int32_t x, const int32_t y, const Colour c) noexcept {
    if (x < 0) throw std::invalid_argument("Bitmap::set_pixel: x cannot be negative");
    if (y < 0) throw std::invalid_argument("Bitmap::set_pixel: y cannot be negative");
    if (x >= w) throw std::invalid_argument("Bitmap::set_pixel: x cannot be greater than width");
    if (y >= h) throw std::invalid_argument("Bitmap::set_pixel: y cannot be greater than height");
    int32_t i = (y*w+x);
    map[i] = c.to_int();
}

Colour Bitmap::get_pixel(const int32_t x, const int32_t y) const noexcept {
    if (x < 0) throw std::invalid_argument("Bitmap::get_pixel: x cannot be negative");
    if (y < 0) throw std::invalid_argument("Bitmap::get_pixel: y cannot be negative");
    if (x >= w) throw std::invalid_argument("Bitmap::get_pixel: x cannot be greater than width");
    if (y >= h) throw std::invalid_argument("Bitmap::get_pixel: y cannot be greater than height");
    int32_t i = (y*w+x);
    uint32_t v = map[i];
    return Colour{v};
}

void Bitmap::clear() const noexcept {
    memset(map, 0, size);
}

void Bitmap::fill(const Colour c) const noexcept {
    if (c.r == c.g && c.g == c.b && c.b == c.a) {
        memset(map, c.r, size);
        return;
    }
    uint32_t v = c.to_int();
    for (int32_t i = 0; i < w*h; i++) {
        map[i] = v;
    }
}

void Bitmap::composite(const Bitmap& bmp, const int32_t x, const int32_t y) noexcept {
    composite(bmp, x, y, 0, 0, bmp.w, bmp.h, BlendMode::SRC_OVER);
}

void Bitmap::composite(const Bitmap& bmp, const int32_t x, const int32_t y, const BlendMode mode) noexcept {
    composite(bmp, x, y, 0, 0, bmp.w, bmp.h, mode);
}

void Bitmap::composite(const Bitmap& bmp, const int32_t x, const int32_t y, const int32_t src_x, const int32_t src_y, const int32_t src_w, const int32_t src_h, const BlendMode mode) noexcept {

    uint32_t* src_map = bmp.map;
    int32_t src_map_w = bmp.w;
    int32_t src_map_h = bmp.h;

    if (w < 0) throw std::invalid_argument("Bitmap::composite: width cannot be negative");
    if (h < 0) throw std::invalid_argument("Bitmap::composite: height cannot be negative");
    if (src_x < 0) throw std::invalid_argument("Bitmap::composite: source region x cannot be negative");
    if (src_y < 0) throw std::invalid_argument("Bitmap::composite: source region y cannot be negative");
    if (src_w < 0) throw std::invalid_argument("Bitmap::composite: source region width cannot be negative");
    if (src_h < 0) throw std::invalid_argument("Bitmap::composite: source region height cannot be negative");
    if (src_x+src_w > src_map_w) throw std::invalid_argument("Bitmap::composite: source region cannot be wider than source bitmap");
    if (src_y+src_h > src_map_h) throw std::invalid_argument("Bitmap::composite: source region cannot be taller than source bitmap");

    int32_t clipped_x = std::min(std::max(x, 0), w);
    int32_t clipped_y = std::min(std::max(y, 0), h);
    int32_t clipped_src_x = src_x + (clipped_x-x);
    int32_t clipped_src_y = src_y + (clipped_y-y);
    int32_t clipped_src_w = std::min(src_w-(clipped_x-x), w-x); // TODO: should this be w-clipped_x?
    int32_t clipped_src_h = std::min(src_h-(clipped_y-y), h-y);

    switch (mode) {
        case BlendMode::CLEAR: clear(); break;
        case BlendMode::SRC: src_blend(src_map, src_map_w, clipped_x, clipped_y, clipped_src_x, clipped_src_y, clipped_src_w, clipped_src_h); break;
        case BlendMode::SRC_OVER: src_over_blend(src_map, src_map_w, clipped_x, clipped_y, clipped_src_x, clipped_src_y, clipped_src_w, clipped_src_h); break;
        default: break;
    }
}

void Bitmap::src_blend(const uint32_t* src_map, const int32_t src_map_w, const int32_t x, const int32_t y, const int32_t src_x, const int32_t src_y, const int32_t src_w, const int32_t src_h) noexcept {
    int32_t size = src_w*4;
    // TODO: factor some multiplications out of the loop
    for (int32_t i = 0; i < src_h; i++) {
        int32_t src_off = (src_y+i)*src_map_w + src_x;
        int32_t dst_off = (y+i)*w + x;
        memcpy(map + dst_off, src_map + src_off, size);
    }
}

void Bitmap::src_over_blend(const uint32_t* src_map, const int32_t src_map_w, const int32_t x, const int32_t y, const int32_t src_x, const int32_t src_y, const int32_t src_w, const int32_t src_h) noexcept {
    for (int32_t i = 0; i < src_h; i++) {
        int32_t src_off_base = (src_y+i)*src_map_w + src_x;
        int32_t dst_off_base = (y+i)*w + x;
        for (int32_t j = 0; j < src_w; j++) {
            int32_t src_off = src_off_base + j;
            uint32_t src_v = src_map[src_off];
            if (src_v <= 0xFFFFFF) {
                continue;
            }

            int32_t dst_off = dst_off_base + j;
            uint32_t dst_v = map[dst_off];
            if (src_v >= 0xFF000000 || dst_v <= 0xFFFFFF) {
                map[dst_off] = src_v;
                continue;
            }

            map[dst_off] = Colour::src_over(dst_v, src_v);
        }
    }
}

}
