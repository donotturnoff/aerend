#ifndef CANVAS_H
#define CANVAS_H

#include "Widget.h"
#include "bitmap/SimpleBitmap.h"
#include "shape/Rectangle.h"
#include "util/Colour.h"
#include "util/Border.h"
#include <cstdint>

namespace aerend {

class Canvas : public Widget {
public:
    Canvas();
    SimpleBitmap& get_bmp() noexcept;
    void set_pos(const int32_t x, const int32_t y) noexcept;
    void set_size(const int32_t w, const int32_t h);
    void paint(Bitmap& dst);
private:
    SimpleBitmap bmp;
};

}

#endif // CANVAS_H

