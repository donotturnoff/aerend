#ifndef SIMPLE_BITMAP_H
#define SIMPLE_BITMAP_H

#include "Bitmap.h"
#include <algorithm>

namespace aerend {

class SimpleBitmap : public Bitmap {
public:
    SimpleBitmap(const int32_t w = 0, const int32_t h = 0);
    SimpleBitmap(uint32_t* map, const int32_t w, const int32_t h);
    SimpleBitmap(const SimpleBitmap& bmp);
    SimpleBitmap(SimpleBitmap&& bmp);
    SimpleBitmap& operator=(SimpleBitmap bmp);
    ~SimpleBitmap();

    friend void swap(SimpleBitmap& bmp1, SimpleBitmap& bmp2) {
        using std::swap;
        swap(bmp1.size, bmp2.size);
        swap(bmp1.w, bmp2.w);
        swap(bmp1.h, bmp2.h);
        swap(bmp1.map, bmp2.map);
    }

    void set_size(const int32_t w, const int32_t h);
};

}

#endif // SIMPLE_BITMAP_H

