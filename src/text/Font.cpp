#include "Font.h"
#include "TextException.h"

FT_Library Font::lib {nullptr};

void Font::init_lib() {
    auto err {FT_Init_FreeType(&Font::lib)};
    if (err) {
        throw TextException{"failed to initialise FreeType", err};
    }
}

Font::Font(const char* path) : Font(path, 0) {}

Font::Font(const char* path, int index) {
    auto err {FT_New_Face(lib, path, index, &face)};
    if (err == FT_Err_Unknown_File_Format) {
        throw TextException{"unsupported font format"};
    } else if (err) {
        throw TextException{"failed to read font file", err};
    }
}

FT_Face& Font::get_face() noexcept {
    return face;
}
