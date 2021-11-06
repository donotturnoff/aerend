#include "TextException.h"
#include <cstring>

// TODO: store error code

TextException::TextException(std::string msg) noexcept : std::runtime_error(msg) {}

TextException::TextException(std::string msg, const FT_Error err) : std::runtime_error(msg + ": FreeType error " + std::to_string(err)) {}
