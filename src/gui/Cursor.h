#ifndef CURSOR_H
#define CURSOR_H

#include "drm/DRMBitmap.h"
#include <cstdint>

namespace aerend {

class Cursor {
public:
    Cursor(DRMBitmap bmp=DRMBitmap{}, int32_t off_x=0, int32_t off_y=0);
    int32_t get_w() const noexcept;
    int32_t get_h() const noexcept;
    DRMBitmap& get_bmp();
private:
    DRMBitmap bmp;
    int32_t off_x, off_y;
};

}

#endif // CURSOR_H

