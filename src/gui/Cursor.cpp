#include "Cursor.h"

namespace aerend {

Cursor::Cursor(DRMBitmap bmp, int32_t off_x, int32_t off_y) : bmp(bmp), off_x(off_x), off_y(off_y) {}

int32_t Cursor::get_w() const noexcept {
    return bmp.get_w();
}

int32_t Cursor::get_h() const noexcept {
    return bmp.get_h();
}

DRMBitmap& Cursor::get_bmp() {
    return bmp;
}

}
