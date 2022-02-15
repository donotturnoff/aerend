#include "Cursor.h"
#include <iostream>

namespace aerend {

Cursor::Cursor() : w(0), h(0), off_x(0), off_y(0) {}

Cursor::Cursor(int fd, int32_t w, int32_t h, const uint32_t* map, int32_t off_x, int32_t off_y) : bmp(DRMBitmap{fd, w*2, h*2}), w(w), h(h), off_x(off_x), off_y(off_y) {
    bmp.src_blend(map, w, w, h, 0, 0, w, h);
}

Cursor::Cursor(Cursor&& cursor) noexcept : Cursor() {
    swap(*this, cursor);
}

int32_t Cursor::get_w() const noexcept {
    return w;
}

int32_t Cursor::get_h() const noexcept {
    return h;
}

DRMBitmap& Cursor::get_bmp() {
    return bmp;
}

}
