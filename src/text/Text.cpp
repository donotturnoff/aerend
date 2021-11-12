#include "Text.h"
#include "TextException.h"
#include "bitmap/SimpleBitmap.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <cassert>
#include <iostream>

Text::Text(const std::string str, Font& font, const int32_t size, const Colour colour, const int32_t x, const int32_t y, int32_t wrap) : str(str), font(font), size(size), x(x), y(y), wrap(wrap), colour(colour) {
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

    for (size_t i = 0; i < n; i++) {
        // Load glyph image into slot
        err = FT_Load_Char(face, str[i], FT_LOAD_RENDER);
        if (err) {
            continue;
        }

        FT_Bitmap bmp {slot->bitmap};

        size_t w {bmp.width};
        size_t h {bmp.rows};
        SimpleBitmap src{(int32_t) w, (int32_t) h};
        uint8_t* map {(uint8_t*) src.get_map()};

        src.fill(colour);

        if (bmp.pixel_mode == FT_PIXEL_MODE_MONO) {
            for (size_t j = 0; j < w*h; j++) {
                uint8_t byte {bmp.buffer[j/8]};
                int index {1 << (7-j%8)};
                uint8_t a = ((byte&index) > 0)*255;
                map[j*4] = map[j*4]*a/255;
                map[j*4+1] = map[j*4]*a/255;
                map[j*4+2] = map[j*4]*a/255;
                map[j*4+3] = a;
            }
        } else if (bmp.pixel_mode == FT_PIXEL_MODE_GRAY) {
            for (size_t j = 0; j < w*h; j++) {
                uint8_t a = bmp.buffer[j];
                map[j*4] = map[j*4]*a/255;
                map[j*4+1] = map[j*4]*a/255;
                map[j*4+2] = map[j*4]*a/255;
                map[j*4+3] = a;
            }
        } else {
            throw TextException{"unsupported pixel mode"};
        }

        bmps.push_back(std::move(src));
        advs.push_back(slot->advance.x >> 6);
        xs.push_back(slot->bitmap_left);
        ys.push_back(y-slot->bitmap_top);
    }
}

void Text::paint(Bitmap& dst) {
    FT_Face& face {font.get_face()};
    int64_t line_height {face->size->metrics.height >> 6};
    size_t n {str.length()};
    int32_t w {wrap};
    if (w == -1) {
        w = dst.get_w();
    }
    if (w == 0) {
        for (size_t i = 0; i < n; i++) {
            dst.composite(bmps[i], xs[i], ys[i], BlendMode::SRC_OVER);
        }
    } else {
        int32_t pen_x = x;
        int32_t row = 0;
        for (size_t i = 0; i < n; i++) {
            bool line_break = pen_x + bmps[i].get_w() >= w || str[i] == '\n';
            bool print = str[i] >= 32;
            if (line_break) {
                pen_x = x;
                row++;
            }
            if (print) {
                dst.composite(bmps[i], pen_x, row*line_height + ys[i], BlendMode::SRC_OVER);
                pen_x += advs[i];
            }
        }
    }
}
