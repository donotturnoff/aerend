#ifndef FONT_H
#define FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

class Font {
public:
    static void init_lib();
    Font(const char* file);
    Font(const char* file, const int index);
    FT_Face& get_face() noexcept;
private:
    static FT_Library lib;
    FT_Face face;
};

#endif // FONT_H
