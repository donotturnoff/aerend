#ifndef FONT_H
#define FONT_H

#include "FreeTypeLib.h"
#include <ft2build.h>
#include FT_FREETYPE_H

class Font {
public:
    static void set_lib(FT_Library& lib);
    Font(const char* file, const int index = 0);
    ~Font();
    FT_Face& get_face() noexcept;
private:
    static FT_Library lib;
    FT_Face face;
};

#endif // FONT_H
