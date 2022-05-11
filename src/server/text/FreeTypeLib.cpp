#include "FreeTypeLib.h"
#include "Font.h"
#include "TextException.h"

namespace aerend {

FreeTypeLib::FreeTypeLib() {
    auto err{FT_Init_FreeType(&lib)};
    if (err) {
        throw TextException{"failed to initialise FreeType", err};
    }
    Font::set_lib(lib);
}

FreeTypeLib::~FreeTypeLib() {
    FT_Done_FreeType(lib);
}

}
