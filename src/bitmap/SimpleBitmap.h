#ifndef SIMPLE_BITMAP_H
#define SIMPLE_BITMAP_H

#include "Bitmap.h"
#include <cstdint>

class SimpleBitmap : public Bitmap {
public:
    SimpleBitmap();
    SimpleBitmap(int32_t w, int32_t h);
    ~SimpleBitmap();

    void set_size(int32_t w, int32_t h);
};

#endif // SIMPLE_BITMAP_H
