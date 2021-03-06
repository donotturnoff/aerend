#include "Text.h"
#include "TextException.h"
#include "bitmap/SimpleBitmap.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdexcept>
#include <cstdio>
#include <cctype>
#include <iostream>

namespace aerend {

const char Text::BREAKPOINTS[] = {' ', '-', '\n', ',', '.', ':', ';', '?', '!'};

Text::Text(Client& client, const std::string str, const std::string font_path, const int32_t font_size, const Colour colour, const int32_t x, const int32_t y, int32_t wrap) : Shape(client, colour), str(str), font(font_path), font_size(font_size), x(x), y(y), wrap(wrap) {
    if (font_size < 0) throw std::invalid_argument("Text::Text: font size cannot be negative");
    if (x < 0) throw std::invalid_argument("Text::Text: x cannot be negative");
    if (y < 0) throw std::invalid_argument("Text::Text: y cannot be negative");
    update();
}

void Text::update() {
    bmps.clear();
    advs.clear();
    xs.clear();
    ys.clear();
    segs.clear();
    seg_ws.clear();
    FT_Face& face{font.get_face()};

    auto err{FT_Set_Char_Size(face, 0, font_size*64, DPI, 0)};
    if (err) {
        throw TextException{"cannot set font size", err};
    }

    FT_GlyphSlot slot{face->glyph};

    int64_t y_off{face->size->metrics.ascender >> 6};

    size_t n{str.length()};
    int32_t seg_w {0};

    /* Iterate over characters in string */
    size_t i;
    for (i = 0; i < n; i++) {
        /* Load glyph image into slot */
        err = FT_Load_Char(face, str[i], FT_LOAD_RENDER);
        if (err) {
            continue;
        }

        /* Create new segment at breakpoints */
        if (std::find(std::begin(Text::BREAKPOINTS), std::end(Text::BREAKPOINTS), str[i]) != std::end(Text::BREAKPOINTS)) {
            segs.push_back(i+1);
            seg_ws.push_back(seg_w);
            seg_w = 0;
        }

        FT_Bitmap bmp{slot->bitmap};

        /* Create bitmap filled with text colour. Text will be rendered by varying the opacity */
        size_t w{bmp.width};
        size_t h{bmp.rows};
        SimpleBitmap src{(int32_t) w, (int32_t) h};
        uint32_t* map{src.get_map()};
        src.fill(colour);

        if (bmp.pixel_mode == FT_PIXEL_MODE_MONO) {
            /* Mono, 1 bit per pixel */
            for (size_t j{0}; j < w*h; j++) {
                uint8_t byte{bmp.buffer[j/8]};
                int index{1 << (7-j%8)};
                uint8_t grey{(byte&index) > 0};
                map[j] *= grey;
            }
        } else if (bmp.pixel_mode == FT_PIXEL_MODE_GRAY) {
            /* Greyscale, 4 bytes per pixel */
            for (size_t j{0}; j < w*h; j++) {
                uint8_t grey{bmp.buffer[j]};
                uint32_t v{map[j]};
                uint8_t a{(uint8_t)(((v >> 24) & 0xFF) * grey / 255)};
                uint8_t r{(uint8_t)(((v >> 16) & 0xFF) * grey / 255)};
                uint8_t g{(uint8_t)(((v >> 8) & 0xFF) * grey / 255)};
                uint8_t b{(uint8_t)((v & 0xFF) * grey / 255)};
                map[j] = a << 24 | r << 16 | g << 8 | b;
            }
        } else {
            throw TextException{"unsupported pixel mode"};
        }

        int64_t adv{slot->advance.x >> 6};

        bmps.push_back(std::move(src));
        advs.push_back(adv);
        xs.push_back(slot->bitmap_left);
        ys.push_back(y+y_off-slot->bitmap_top);

        seg_w += adv;
    }

    segs.push_back(i);
    seg_ws.push_back(seg_w);
}

void Text::set_str(std::string str) {
    this->str = str;
    update();
}

void Text::set_colour(Colour colour) {
    this->colour = colour;
    update();
}

void Text::set_font_size(const int32_t font_size) {
    this->font_size = font_size;
    update();
}

void Text::set_wrap(const int32_t wrap) {
    this->wrap = wrap;
    update();
}

int32_t Text::get_w() const noexcept {
    return width;
}

int32_t Text::get_h() const noexcept {
    return height;
}

std::string Text::get_str() const noexcept {
    return str;
}

void Text::paint(Bitmap& dst) {
    FT_Face& face{font.get_face()};
    int64_t line_height{face->size->metrics.height >> 6};
    size_t n{str.length()};
    int32_t w{wrap};
    if (w == -1) { /* Auto-wrap at width of label */
        w = dst.get_w();
    }
    int32_t pen_x{x};
    if (w == 0) { /* Do not wrap */
        for (size_t i{0}; i < n; i++) {
            dst.composite(bmps[i], pen_x+xs[i], ys[i], BlendMode::SRC_OVER);
            pen_x += advs[i];
        }
        height = line_height;
    } else {
        int32_t row{0};
        int32_t seg_i{0};
        int32_t seg_w{seg_ws[0]};
        for (size_t i{0}; i < n; i++) {
            char c{str[i]};
            bool line_break{c == '\n' || pen_x + advs[i] >= w}; /* Break at new line or if segment is longer than line */
            if (i >= segs[seg_i]) {
                /* Progressed into a new segment, line break if next segment will run past wrap limit */
                seg_i++;
                seg_w = seg_ws[seg_i];
                line_break = line_break || (pen_x + seg_w >= w && seg_w < w);
            }
            bool print{c >= ' '};
            if (line_break) {
                pen_x = x;
                row++;
                /* Don't print whitespace after a line break */
                if (std::isspace(c)) {
                    print = false;
                }
            }
            if (print) {
                dst.composite(bmps[i], pen_x+xs[i], row*line_height + ys[i], BlendMode::SRC_OVER);
                pen_x += advs[i];
            }
        }
        height = row * line_height;
    }
    width = pen_x+1;
}

}
