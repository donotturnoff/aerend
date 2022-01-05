#ifndef STYLE_H
#define STYLE_H

#include "Border.h"
#include "Padding.h"
#include "Margin.h"
#include "Colour.h"
#include "gui/Cursor.h"
#include "text/Font.h"
#include <cstdint>

namespace aerend {

struct Style {
    Border border;
    Padding padding;
    Margin margin;
    Cursor cursor;
    Colour bg_colour, fg_colour;
    Font font;
    int32_t font_size;
};

}

#endif // STYLE_H

