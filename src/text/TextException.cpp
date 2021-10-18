#include "TextException.h"
#include <cstring>
#include <string>
#include <exception>
#include <cstdlib>

// TODO: store error code
// TODO: improve memory management

TextException::TextException(const char* msg) noexcept {
    this->msg = strdup(msg);
}

TextException::TextException(const char* msg, const FT_Error err) {
    std::string buf {msg};
    buf.append(": ");
    const char* err_str {FT_Error_String(err)};
    if (!err_str) {
        buf.append("FreeType error ");
        buf.append(std::to_string(err));
        buf.append(" (FreeType must be compiled with `FT_CONFIG_OPTION_ERROR_STRINGS` or `FT_DEBUG_LEVEL_ERROR` to get meaningful descriptions. Refer to https://www.freetype.org/freetype2/docs/reference/ft2-error_code_values.html for details)");
    } else {
        buf.append(err_str);
    }
    this->msg = strdup(buf.c_str());
}

TextException::~TextException() {
    std::free(this->msg);
}

const char* TextException::what() const noexcept {
    return msg;
}
