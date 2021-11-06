#ifndef FREE_TYPE_LIB_H
#define FREE_TYPE_LIB_H

#include <ft2build.h>
#include FT_FREETYPE_H

struct FreeTypeLib {
    FT_Library lib;
    FreeTypeLib();
    FreeTypeLib(const FreeTypeLib& lib) = delete;
    FreeTypeLib(FreeTypeLib&& lib) = delete;
    FreeTypeLib& operator=(FreeTypeLib& lib) = delete;
    ~FreeTypeLib();
};

#endif // FREE_TYPE_LIB_H
