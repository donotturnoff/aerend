#ifndef CURSOR_H
#define CURSOR_H

#include "bitmap/DRMBitmap.h"
#include <cstdint>
#include <cstring>

namespace aerend {

class Cursor {
public:
    Cursor();
    Cursor(int fd, int32_t w, int32_t h, const uint32_t* map, int32_t off_x=0, int32_t off_y=0);
    Cursor(Cursor&& cursor) noexcept;
    ~Cursor() = default;
    friend void swap(Cursor& cursor1, Cursor& cursor2) noexcept {
        using std::swap;
        swap(cursor1.bmp, cursor2.bmp);
        swap(cursor1.w, cursor2.w);
        swap(cursor1.h, cursor2.h);
        swap(cursor1.off_x, cursor2.off_x);
        swap(cursor1.off_y, cursor2.off_y);
    }
    int32_t get_w() const noexcept;
    int32_t get_h() const noexcept;
    DRMBitmap& get_bmp();
private:
    Cursor(Cursor&) = delete;
    Cursor& operator=(Cursor) = delete;
    DRMBitmap bmp;
    int32_t w, h, off_x, off_y;
};

}

#endif // CURSOR_H

