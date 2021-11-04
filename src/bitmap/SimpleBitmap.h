#ifndef SIMPLE_BITMAP_H
#define SIMPLE_BITMAP_H

#include "Bitmap.h"
#include <cstdint>

class SimpleBitmap : public Bitmap {
public:
    SimpleBitmap(const int32_t w = 0, const int32_t h = 0);
    ~SimpleBitmap();

    void set_size(const int32_t w, const int32_t h);
};

#endif // SIMPLE_BITMAP_H
