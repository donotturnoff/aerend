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
    Button(Client& client, const std::string str=Button::def_str, const std::string font_path=Button::def_font_path, const int32_t font_size=Button::def_font_size, const Colour colour=Button::def_colour, const Colour bg_colour=Button::def_bg_colour, const Border border=Button::def_border, const Margin margin=Button::def_margin, const Padding padding=Button::def_padding, const int32_t wrap=Button::def_wrap);
    void set_colour(Colour colour);
    void set_bg_colour(Colour bg_colour) noexcept;
    void set_str(std::string str);
    void set_wrap(const int32_t wrap);
    void set_font_size(const int32_t font_size);
    void set_border(Border border) noexcept;
    void set_pos(const int32_t x, const int32_t y) noexcept;
    void set_size(const int32_t w, const int32_t h);
    void paint(Bitmap& dst);

    static const std::string def_str, def_font_path;
    static const int32_t def_font_size;
    static const Colour def_colour, def_bg_colour;
    static const Border def_border;
    static const Margin def_margin;
    static const Padding def_padding;
    static const int32_t def_wrap;
private:
    Rectangle rect;
    Text text;
    SimpleBitmap bmp;
    Colour old_bg_colour;
};

}

#endif // BUTTON_H

