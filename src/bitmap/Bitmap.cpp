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

uint8_t *Bitmap::get_map() const noexcept {
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

// TODO: x, y -> Point?
// TODO: compositing
void Bitmap::set_pixel(const int32_t x, const int32_t y, const Colour c) noexcept {
    assert(x >= 0);
    assert(x < w);
    assert(y >= 0);
    assert(y < h);
    int32_t i = (y*w+x)*4;
    map[i] = c.b;
    map[i+1] = c.g;
    map[i+2] = c.r;
    map[i+3] = c.a;
}

Colour Bitmap::get_pixel(const int32_t x, const int32_t y) const noexcept {
    assert(x >= 0);
    assert(x < w);
    assert(y >= 0);
    assert(y < h);
    int32_t i = (y*w+x)*4;
    return Colour{map[i+2], map[i+1], map[i], map[i+3]};
}

void Bitmap::clear() noexcept {
    memset(map, 0, size);
}

void Bitmap::fill(const Colour c) noexcept {
    for (int32_t i = 0; i < size; i+=4) {
        map[i] = c.b;
        map[i+1] = c.g;
        map[i+2] = c.r;
        map[i+3] = c.a;
    }
}

void Bitmap::composite(const Bitmap& bmp, const int32_t x, const int32_t y) noexcept {
    composite(bmp.map, bmp.w, bmp.h, x, y, 0, 0, bmp.w, bmp.h, OPAQUE_BLEND);
}

void Bitmap::composite(const Bitmap& bmp, const int32_t x, const int32_t y, const BlendMode mode) noexcept {
    composite(bmp.map, bmp.w, bmp.h, x, y, 0, 0, bmp.w, bmp.h, mode);
}

void Bitmap::composite(const Bitmap& bmp, const int32_t x, const int32_t y, const int32_t src_x, const int32_t src_y, const int32_t src_w, const int32_t src_h, const BlendMode mode) noexcept {
    composite(bmp.map, bmp.w, bmp.h, x, y, src_x, src_y, src_w, src_h, mode);
}

void Bitmap::composite(const uint8_t* src_map, const int32_t src_map_w, const int32_t src_map_h, const int32_t x, const int32_t y, const int32_t src_x, const int32_t src_y, const int32_t src_w, const int32_t src_h, const BlendMode mode) noexcept {
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
        case OPAQUE_BLEND: opaque_blend(src_map, src_map_w, x, y, src_x, src_y, src_w, src_h); break;
        case OVER_BLEND: over_blend(src_map, src_map_w, x, y, src_x, src_y, src_w, src_h); break;
        default: break;
    }
}

void Bitmap::opaque_blend(const uint8_t* src_map, const int32_t src_map_w, const int32_t x, const int32_t y, const int32_t src_x, const int32_t src_y, const int32_t src_w, const int32_t src_h) noexcept {
    int32_t size = src_w*4;
    // TODO: factor some multiplications out of the loop
    for (int32_t i = 0; i < src_h; i++) {
        int32_t src_off = (src_y+i)*src_map_w + src_x;
        int32_t dst_off = (y+i)*w + x;
        memcpy(map + dst_off*4, src_map + src_off*4, size);
    }
}

void Bitmap::over_blend(const uint8_t* src_map, const int32_t src_map_w, const int32_t x, const int32_t y, const int32_t src_x, const int32_t src_y, const int32_t src_w, const int32_t src_h) noexcept {
    for (int32_t i = 0; i < src_h; i++) {
        for (int32_t j = 0; j < src_w; j++) {
            int32_t src_off = ((src_y+i)*src_map_w + src_x + j) * 4;
            int32_t src_a = src_map[src_off+3];
            if (src_a == 0) continue;
            int32_t src_b = src_map[src_off];
            int32_t src_g = src_map[src_off+1];
            int32_t src_r = src_map[src_off+2];

            int32_t dst_off = ((y+i)*w + x + j) * 4;
            int32_t dst_b = map[dst_off];
            int32_t dst_g = map[dst_off+1];
            int32_t dst_r = map[dst_off+2];
            int32_t dst_a = map[dst_off+3];

            // TODO: investigate replacing /255 with >>8
            // TODO: investigate replacing individual calculations with one big calculation
            int32_t p = (255 - src_a);
            int32_t a = src_a + dst_a*p/255;
            int32_t r = (src_r*src_a + dst_r*p*dst_a/255)/a;
            int32_t g = (src_g*src_a + dst_g*p*dst_a/255)/a;
            int32_t b = (src_b*src_a + dst_b*p*dst_a/255)/a;

            map[dst_off] = b;
            map[dst_off+1] = g;
            map[dst_off+2] = r;
            map[dst_off+3] = a;
        }
    }
}
