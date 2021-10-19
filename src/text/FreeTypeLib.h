#ifndef FREE_TYPE_LIB_H
#define FREE_TYPE_LIB_H

#include <ft2build.h>
#include FT_FREETYPE_H

struct FreeTypeLib {
    FT_Library lib;
    FreeTypeLib();
    ~FreeTypeLib();
};

#endif // FREE_TYPE_LIB_H
