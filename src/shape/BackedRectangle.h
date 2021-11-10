#ifndef BACKED_RECTANGLE_H
#define BACKED_RECTANGLE_H

#include "Shape.h"
#include "Rectangle.h"
#include "bitmap/Bitmap.h"
#include "bitmap/SimpleBitmap.h"
#include "utils/Colour.h"
#include "utils/Border.h"
#include <cstdint>

class BackedRectangle : public Rectangle {
public:
    BackedRectangle(int32_t x, int32_t y, int32_t w, int32_t h, Colour colour, Border border);
    void set_size(int32_t w, int32_t h);
    void set_colour(Colour colour);
    void set_border(Border border);
    void paint(Bitmap& bmp);
private:
    SimpleBitmap bmp;
};

#endif // RECTANGLE_H
