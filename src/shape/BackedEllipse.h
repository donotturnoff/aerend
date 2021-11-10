#ifndef BACKED_ELLIPSE_H
#define BACKED_ELLIPSE_H

#include "Shape.h"
#include "Ellipse.h"
#include "bitmap/Bitmap.h"
#include "bitmap/SimpleBitmap.h"
#include "utils/Colour.h"
#include "utils/Border.h"
#include <cstdint>

class BackedEllipse : public Ellipse {
public:
    BackedEllipse(int32_t x, int32_t y, int32_t w, int32_t h, Colour colour, Border border);
    void set_size(int32_t w, int32_t h);
    void set_colour(Colour colour);
    void set_border(Border border);
    void paint(Bitmap& bmp);
private:
    SimpleBitmap bmp;
};

#endif // BACKED_ELLIPSE_H
