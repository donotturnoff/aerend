#include "Canvas.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>

namespace aerend {

Canvas::Canvas() : bmp(SimpleBitmap{}) {}

SimpleBitmap& Canvas::get_bmp() noexcept {
    return bmp;
}

void Canvas::set_pos(const int32_t x, const int32_t y) noexcept {
    this->x = x;
    this->y = y;
}

void Canvas::set_size(const int32_t w, const int32_t h) {
    this->w = w;
    this->h = h;
    int32_t tmp_size = bmp.get_size();
    uint32_t* tmp_map = (uint32_t*) malloc(tmp_size);
    if (!tmp_map) {
        throw std::bad_alloc();
    }
    memcpy(tmp_map, bmp.get_map(), tmp_size);
    bmp.set_size(w, h);
    bmp.fill(Colour::white());
    memcpy(bmp.get_map(), tmp_map, tmp_size);
    free(tmp_map);
}

void Canvas::paint(Bitmap& dst) {
    dst.composite(bmp, x, y);
}

}
