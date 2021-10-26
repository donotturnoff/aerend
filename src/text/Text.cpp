#include "Text.h"
#include "TextException.h"
#include "bitmap/SimpleBitmap.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <cassert>
#include <iostream>

Text::Text(const std::string str, Font& font, const int32_t size, const Colour colour, const int32_t x, const int32_t y) : str(str), font(font), size(size), x(x), y(y), colour(colour) {
    assert(size >= 0);
    assert(x >= 0);
    assert(y >= 0);

    // Determine if given size is available for this font
    FT_Face& face {font.get_face()};
    if (!(face->face_flags & FT_FACE_FLAG_SCALABLE)) {
        bool valid_size {false};
        for (int32_t i = 0; i < face->num_fixed_sizes; i++) {
            if (size == face->available_sizes[i].height) {
                valid_size = true;
                break;
            }
        }
        if (!valid_size) {
            throw TextException{"invalid font size"};
        }
    }

    FT_GlyphSlot slot {face->glyph};

    auto err {FT_Set_Char_Size(face, 0, size*64, DPI, 0)};
    if (err) {
        throw TextException{"cannot set font size", err};
    }

    size_t n {str.length()};

    int32_t pen_x {x};

    for (size_t i = 0; i < n; i++) {
        // Load glyph image into slot
        err = FT_Load_Char(face, str[i], FT_LOAD_RENDER);
        if (err) {
            continue;
        }

        FT_Bitmap bmp {slot->bitmap};

        size_t w {bmp.width}; 
        size_t h {bmp.rows};
        auto src {std::make_shared<SimpleBitmap>(w, h)};
        uint8_t* map {(uint8_t*) src->get_map()};

        src->fill(colour);

        if (bmp.pixel_mode == FT_PIXEL_MODE_MONO) {
            for (size_t j = 0; j < w*h; j++) {
                uint8_t byte {bmp.buffer[j/8]};
                int index {1 << (7-j%8)};
                map[j*4+3] = ((byte&index) > 0)*255;
            }
        } else if (bmp.pixel_mode == FT_PIXEL_MODE_GRAY) {
            for (size_t j = 0; j < w*h; j++) {
                map[j*4+3] = bmp.buffer[j];
            }
        } else {
            throw TextException{"unsupported pixel mode"};
        }
        bmps.push_back(src);
        xs.push_back(pen_x+slot->bitmap_left);
        ys.push_back(y-slot->bitmap_top);
        pen_x += slot->advance.x >> 6;
    }
}

void Text::paint(Bitmap& dst) {
    size_t n {str.length()};
    for (size_t i = 0; i < n; i++) {
        dst.composite(*bmps[i], xs[i], ys[i], BlendMode::SRC_OVER);
    }
}
