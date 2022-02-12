#ifndef CANVAS_H
#define CANVAS_H

#include "Widget.h"
#include "bitmap/SimpleBitmap.h"
#include "shape/Rectangle.h"
#include "shape/Shape.h"
#include "style/Colour.h"
#include "style/Border.h"
#include <cstdint>

namespace aerend {

class Canvas : public Widget {
public:
    Canvas(Client& client, int32_t cvs_w, int32_t cvs_h);
    SimpleBitmap& get_bmp() noexcept;
    void set_pos(const int32_t x, const int32_t y) noexcept;
    void set_size(const int32_t w, const int32_t h);
    int32_t get_cvs_w() const noexcept;
    int32_t get_cvs_h() const noexcept;
    void paint(Bitmap& dst);
    void draw(Shape& shape);
    void clear();
    void fill(Colour colour);
private:
    int32_t cvs_w, cvs_h;
    SimpleBitmap bmp;
};

}

#endif // CANVAS_H

