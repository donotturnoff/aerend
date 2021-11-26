#ifndef LABEL_H
#define LABEL_H

#include "Widget.h"
#include "LayoutManager.h"
#include "GridLayout.h"
#include "bitmap/SimpleBitmap.h"
#include "shape/Rectangle.h"
#include "text/Font.h"
#include "text/Text.h"
#include "style/Colour.h"
#include "style/Border.h"
#include <cstdint>
#include <memory>

namespace aerend {

class Label : public Widget {
public:
    Label(std::string str, Font font, int32_t font_size, Colour colour=Colour::black(), Colour bg_colour=Colour::white(), Border border=Border{}, Margin margin=Margin{});
    void set_colour(Colour colour);
    void set_bg_colour(Colour bg_colour) noexcept;
    void set_str(std::string str);
    void set_wrap(const int32_t wrap);
    void set_font_size(const int32_t font_size);
    void set_border(Border border) noexcept;
    void set_pos(const int32_t x, const int32_t y) noexcept;
    void set_size(const int32_t w, const int32_t h);
    void paint(Bitmap& dst);
private:
    Rectangle rect;
    Text text;
    SimpleBitmap bmp;
};

}

#endif // LABEL_H

