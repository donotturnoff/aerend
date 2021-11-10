#ifndef SHAPE_H
#define SHAPE_H

#include "utils/Colour.h"
#include "bitmap/Bitmap.h"

class Shape {
public:
    virtual void paint(Bitmap& bmp) = 0;
    virtual void set_colour(Colour colour);
protected:
    Colour colour;
};

#endif // SHAPE_H
