#ifndef LINE_H
#define LINE_H

#include "Shape.h"
#include "bitmap/Bitmap.h"
#include "util/Colour.h"
#include <cstdint>

namespace aerend {

class Line : public Shape {
public:
    Line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, Colour colour);
    void set_x0(int32_t x0);
    void set_y0(int32_t y0);
    void set_start(int32_t x0, int32_t y0);
    void set_x1(int32_t x1);
    void set_y1(int32_t y1);
    void set_end(int32_t x1, int32_t y1);
    void paint(Bitmap& bmp);
protected:
    int32_t x0, y0, x1, y1;
private:
    static uint8_t classify_point(Bitmap& dst, int32_t x, int32_t y);
    static void intersect(Bitmap& dst, int32_t& x0, int32_t& y0, int32_t x1, int32_t y1);
};

}

#endif // LINE_H

