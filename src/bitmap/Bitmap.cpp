#include "Bitmap.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>

void Bitmap::set_w(const int32_t w) {
    assert(w >= 0);
    set_size(w, h);
}

void Bitmap::set_h(const int32_t h) {
    assert(h >= 0);
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
    assert(x >= 0);
    assert(x < w);
    assert(y >= 0);
    assert(y < h);
    int32_t i = (y*w+x);
    map[i] = (c.a << 24) | (c.r << 16) | (c.g << 8) | c.b;
}

Colour Bitmap::get_pixel(const int32_t x, const int32_t y) const noexcept {
    assert(x >= 0);
    assert(x < w);
    assert(y >= 0);
    assert(y < h);
    int32_t i = (y*w+x);
    uint32_t v = map[i];
    uint8_t b = v;
    uint8_t g = v >> 8;
    uint8_t r = v >> 16;
    uint8_t a = v >> 24;
    return Colour{r, g, b, a};
}

void Bitmap::clear() noexcept {
    memset(map, 0, size);
}

void Bitmap::fill(const Colour c) noexcept {
    for (int32_t i = 0; i < w*h; i++) {
        map[i] = (c.a << 24) | (c.r << 16) | (c.g << 8) | c.b;
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

    assert(x >= 0);
    assert(y >= 0);
    assert(w >= 0);
    assert(h >= 0);
    assert(src_x >= 0);
    assert(src_y >= 0);
    assert(src_w >= 0);
    assert(src_h >= 0);
    assert(x+src_w <= w);
    assert(y+src_h <= h);
    assert(src_x+src_w <= src_map_w);
    assert(src_y+src_h <= src_map_h);

    switch (mode) {
        case BlendMode::CLEAR: clear(); break;
        case BlendMode::SRC: opaque_blend(src_map, src_map_w, x, y, src_x, src_y, src_w, src_h); break;
        case BlendMode::SRC_OVER: over_blend(src_map, src_map_w, x, y, src_x, src_y, src_w, src_h); break;
        default: break;
    }
}

void Bitmap::opaque_blend(const uint32_t* src_map, const int32_t src_map_w, const int32_t x, const int32_t y, const int32_t src_x, const int32_t src_y, const int32_t src_w, const int32_t src_h) noexcept {
    int32_t size = src_w*4;
    // TODO: factor some multiplications out of the loop
    for (int32_t i = 0; i < src_h; i++) {
        int32_t src_off = (src_y+i)*src_map_w + src_x;
        int32_t dst_off = (y+i)*w + x;
        memcpy(map + dst_off, src_map + src_off, size);
    }
}

void Bitmap::over_blend(const uint32_t* src_map, const int32_t src_map_w, const int32_t x, const int32_t y, const int32_t src_x, const int32_t src_y, const int32_t src_w, const int32_t src_h) noexcept {
    for (int32_t i = 0; i < src_h; i++) {
        for (int32_t j = 0; j < src_w; j++) {
			int32_t src_off = ((src_y+i)*src_map_w + src_x + j);
            uint32_t src_v = src_map[src_off];
            uint32_t src_a = src_v >> 24;
            if (src_a == 0) {
                continue;
            }

            int32_t dst_off = ((y+i)*w + x + j);
            uint32_t dst_v = map[dst_off];
			uint32_t dst_a = dst_v >> 24;
            if (src_a == 255 || dst_a == 0) {
                map[dst_off] = src_v;
                continue;
            }

            uint32_t src_b = src_v & 0xFF;
            uint32_t src_g = (src_v >> 8) & 0xFF;
            uint32_t src_r = (src_v >> 16) & 0xFF;

            uint32_t dst_b = dst_v & 0xFF;
            uint32_t dst_g = (dst_v >> 8) & 0xFF;
            uint32_t dst_r = (dst_v >> 16) & 0xFF;

            int32_t p = (255 - src_a);
            int32_t a = src_a + ((dst_a*p)>>8);
            int32_t r = src_r + ((dst_r*p)>>8);
            int32_t g = src_g + ((dst_g*p)>>8);
            int32_t b = src_b + ((dst_b*p)>>8);

            map[dst_off] = (a << 24 | r << 16 | g << 8 | b);
        }
    }
}
