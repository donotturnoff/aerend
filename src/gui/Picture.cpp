#include "Picture.h"
#include "bitmap/BitmapException.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

namespace aerend {

Picture::Picture(Client& client, int32_t pic_w, int32_t pic_h, std::vector<uint32_t> data) : Widget(client), bmp(SimpleBitmap{pic_w, pic_h}) {
    set_data(data);
}

void Picture::set_pos(const int32_t x, const int32_t y) noexcept {
    this->x = x;
    this->y = y;
}

void Picture::set_size(const int32_t w, const int32_t h) {
    this->w = w;
    this->h = h;
}

void Picture::set_data(std::vector<uint32_t> data) {
    if (data.size()*4 > bmp.get_size()) {
        throw BitmapException{"picture data exceeds picture size"};
    }
    std::copy(data.begin(), data.end(), bmp.get_map());
    autorepaint();
}

int32_t Picture::get_pixels_size() {
    return bmp.get_w() * bmp.get_h();
}

void Picture::paint(Bitmap& dst) {
    dst.composite(bmp, x, y, 0, 0, std::min(w, bmp.get_w()), std::min(h, bmp.get_h()), BlendMode::SRC_OVER);
}

}
