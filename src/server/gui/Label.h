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

class Container;

class Label : public Widget {
public:
    Label(Client& client, std::string str = Label::def_str, std::string font_path = Label::def_font_path, int32_t font_size = Label::def_font_size, Colour colour=Label::def_colour, Colour bg_colour=Label::def_bg_colour, Border border=Label::def_border, Margin margin=Label::def_margin, Padding padding = Label::def_padding, int32_t wrap = Label::def_wrap);
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
};

}

#endif // LABEL_H

