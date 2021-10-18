#ifndef TEXT_EXCEPTION_H
#define TEXT_EXCEPTION_H

#include <exception>
#include <ft2build.h>
#include FT_FREETYPE_H

class TextException : public std::exception {
public:
    TextException(const char* msg) noexcept;
    TextException(const char* msg, const FT_Error err);
    ~TextException();
    const char* what() const noexcept;
private:
    char* msg;
};

#endif // TEXT_EXCEPTION_H
