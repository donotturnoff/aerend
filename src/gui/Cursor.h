#ifndef CURSOR_H
#define CURSOR_H

#include "bitmap/DRMBitmap.h"
#include <cstdint>
#include <cstring>

namespace aerend {

enum class CursorPreset {
    ARROW, MAX_NUM=ARROW
};

class Cursor {
public:
    Cursor(int fd, CursorPreset preset);
    Cursor(int fd, int32_t w, int32_t h, const uint32_t* map, int32_t off_x=0, int32_t off_y=0);
    int32_t get_w() const noexcept;
    int32_t get_h() const noexcept;
    DRMBitmap& get_bmp();
private:
    static const uint32_t* MAPS[(int) CursorPreset::MAX_NUM+1];
    static const int32_t WS[(int) CursorPreset::MAX_NUM+1];
    static const int32_t HS[(int) CursorPreset::MAX_NUM+1];
    static const int32_t OFF_XS[(int) CursorPreset::MAX_NUM+1];
    static const int32_t OFF_YS[(int) CursorPreset::MAX_NUM+1];
    static const uint32_t ARROW_MAP[];
    DRMBitmap bmp;
    int32_t w, h, off_x, off_y;
};

}

#endif // CURSOR_H

