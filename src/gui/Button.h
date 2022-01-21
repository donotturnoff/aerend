#ifndef BUTTON_H
#define BUTTON_H

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

class Button : public Widget {
public:
    Button(Client& client, const std::string str=Button::str, const std::string font_path=Button::font_path, const int32_t font_size=Button::font_size, const Colour colour=Button::colour, const Colour bg_colour=Button::bg_colour, const Border border=Button::border, const Margin margin=Button::margin, const Padding padding=Button::padding, const int32_t wrap=Button::wrap);
    void set_colour(Colour colour);
    void set_bg_colour(Colour bg_colour) noexcept;
    void set_str(std::string str);
    void set_wrap(const int32_t wrap);
    void set_font_size(const int32_t font_size);
    void set_border(Border border) noexcept;
    void set_pos(const int32_t x, const int32_t y) noexcept;
    void set_size(const int32_t w, const int32_t h);
    void paint(Bitmap& dst);

    static const std::string str, font_path;
    static const int32_t font_size;
    static const Colour colour, bg_colour;
    static const Border border;
    static const Margin margin;
    static const Padding padding;
    static const int32_t wrap;
private:
    Rectangle rect;
    Text text;
    SimpleBitmap bmp;
};

}

#endif // BUTTON_H

