#include "Picture.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>

namespace aerend {

Picture::Picture(int32_t pic_w, int32_t pic_h, uint32_t* map) : bmp(SimpleBitmap{pic_w, pic_h}) {
    std::memcpy(bmp.get_map(), map, pic_w*pic_h*4);
}

void Picture::set_pos(const int32_t x, const int32_t y) noexcept {
    this->x = x;
    this->y = y;
}

void Picture::set_size(const int32_t w, const int32_t h) {
    this->w = w;
    this->h = h;
}

void Picture::paint(Bitmap& dst) {
    dst.composite(bmp, x, y, 0, 0, std::min(w, bmp.get_w()), std::min(h, bmp.get_h()), BlendMode::SRC_OVER);
}

}
