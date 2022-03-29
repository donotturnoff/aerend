#ifndef TEXT_H
#define TEXT_H

// TODO: make portable
#define DPI 96

#include "Font.h"
#include "bitmap/Bitmap.h"
#include "bitmap/SimpleBitmap.h"
#include "shape/Shape.h"
#include "style/Colour.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace aerend {

class Text : public Shape {
public:
    // TODO: reference to str?
    Text(Client& client, const std::string str, const std::string font_path, const int32_t font_size, const Colour colour, const int32_t x, const int32_t y, const int32_t wrap = 0);
    void set_str(std::string str);
    void set_colour(Colour colour);
    void set_font_size(const int32_t font_size);
    std::string get_str() const noexcept;
    void set_wrap(const int32_t wrap);
    int32_t get_w() const noexcept;
    int32_t get_h() const noexcept;
    void update();
    void paint(Bitmap& dst);
private:
    static const char BREAKPOINTS[];
    std::string str;
    Font font;
    int32_t font_size, x, y, wrap, width, height;
    std::vector<SimpleBitmap> bmps;
    std::vector<int32_t> xs, ys, advs, seg_ws;
    std::vector<size_t> segs;
};

}

#endif // TEXT_H

