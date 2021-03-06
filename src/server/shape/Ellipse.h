#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "Shape.h"
#include "bitmap/Bitmap.h"
#include "style/Colour.h"
#include "style/Border.h"
#include <cstdint>

namespace aerend {

class Ellipse : public Shape {
public:
    Ellipse(Client& client, int32_t x, int32_t y, int32_t w, int32_t h, Colour colour, Border border=Border{});
    void set_x(int32_t x);
    void set_y(int32_t y);
    void set_pos(int32_t x, int32_t y);
    void set_w(int32_t w);
    void set_h(int32_t h);
    virtual void set_size(int32_t w, int32_t h);
    virtual void set_border(Border border);
    void paint(Bitmap& bmp);

    static const Border def_border;
protected:
    int32_t x, y, w, h;
    Border border;
};

}

#endif // RECTANGLE_H

