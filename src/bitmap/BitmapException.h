#ifndef BITMAP_EXCEPTION_H
#define BITMAP_EXCEPTION_H

#include <string>
#include <stdexcept>

namespace aerend {

class BitmapException : public std::runtime_error {
public:
    BitmapException(std::string msg) noexcept;
    BitmapException(std::string msg, const int errnum);
};

}

#endif // BITMAP_EXCEPTION_H

