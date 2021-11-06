#ifndef TEXT_EXCEPTION_H
#define TEXT_EXCEPTION_H

#include <string>
#include <stdexcept>
#include <ft2build.h>
#include FT_FREETYPE_H

class TextException : public std::runtime_error {
public:
    TextException(std::string msg) noexcept;
    TextException(std::string msg, const FT_Error err);
};

#endif // TEXT_EXCEPTION_H
