#ifndef PICTURE_H
#define PICTURE_H

#include "Widget.h"
#include "bitmap/SimpleBitmap.h"
#include "shape/Rectangle.h"
#include "style/Colour.h"
#include "style/Border.h"
#include <cstdint>
#include <vector>

namespace aerend {

class Picture : public Widget {
public:
    Picture(Client& client, int32_t pic_w, int32_t pic_h, std::vector<uint32_t> data);
    void set_pos(const int32_t x, const int32_t y) noexcept;
    void set_size(const int32_t w, const int32_t h);
    void set_data(std::vector<uint32_t> data);
    void paint(Bitmap& dst);
private:
    SimpleBitmap bmp;
};

}

#endif // PICTURE_H

