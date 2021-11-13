#ifndef FONT_H
#define FONT_H

#include "FreeTypeLib.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <algorithm>
#include <memory>

namespace aerend {

class Font {
public:
    static void set_lib(FT_Library& lib);
    Font();
    Font(const char* file, const int index = 0);
    Font(const Font& font);
    Font(Font&& font);
    Font& operator=(Font font);
    ~Font();
    friend void swap(Font& font1, Font& font2) {
        using std::swap;
        swap(font1.face, font2.face);
        swap(font1.refs, font2.refs);
    }
    FT_Face& get_face() noexcept;
private:
    static FT_Library lib;
    FT_Face face;
    std::shared_ptr<int> refs;
};

}

#endif // FONT_H

