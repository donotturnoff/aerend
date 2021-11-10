#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "Shape.h"
#include "bitmap/Bitmap.h"
#include "bitmap/SimpleBitmap.h"
#include "utils/Colour.h"
#include "utils/Border.h"
#include <cstdint>

class Ellipse : public Shape {
public:
    Ellipse(int32_t x, int32_t y, int32_t w, int32_t h, Colour colour, Border border);
    void set_x(int32_t x);
    void set_y(int32_t y);
    void set_pos(int32_t x, int32_t y);
    void set_w(int32_t w);
    void set_h(int32_t h);
    virtual void set_size(int32_t w, int32_t h);
    virtual void set_border(Border border);
    void paint(Bitmap& bmp);
protected:
    SimpleBitmap create_bmp();
    int32_t x, y, w, h;
    Colour colour;
    Border border;
};

#endif // RECTANGLE_H
