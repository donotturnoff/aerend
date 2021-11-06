#ifndef TEXT_H
#define TEXT_H

// TODO: make portable
#define DPI 96

#include "Font.h"
#include "bitmap/Bitmap.h"
#include "bitmap/SimpleBitmap.h"
#include "utils/Colour.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class Text {
public:
    // TODO: reference to str?
    Text(const std::string str, Font& font, const int32_t size, const Colour colour, const int32_t x, const int32_t y);
    void paint(Bitmap& bitmap);
private:
    std::string str;
    Font& font;
    int32_t size, x, y;
    Colour colour;
    std::vector<SimpleBitmap> bmps;
    std::vector<int32_t> xs, ys;
};

#endif // TEXT_H
