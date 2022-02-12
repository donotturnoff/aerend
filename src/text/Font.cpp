#include "Font.h"
#include "TextException.h"
#include <cassert>
#include <iostream>

namespace aerend {

FT_Library Font::lib {nullptr};

void Font::set_lib(FT_Library& lib) {
    Font::lib = lib;
}

Font::Font() : face(nullptr) {}

Font::Font(const std::string path, int index) {
    assert(lib != nullptr);

    auto err {FT_New_Face(lib, path.c_str(), index, &face)};
    if (err == FT_Err_Unknown_File_Format) {
        throw TextException{"unsupported font format"};
    } else if (err) {
        throw TextException{"failed to read font file", err};
    }
}

Font::Font(const Font& font) : face(font.face) {}

Font::Font(Font&& font) : Font() {
    swap(*this, font);
}

Font& Font::operator=(Font font) {
    swap(*this, font);
    return *this;
}

Font::~Font() {
    FT_Done_Face(face);
}

FT_Face& Font::get_face() noexcept {
    return face;
}

}
