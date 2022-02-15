#include "BitmapException.h"
#include <cstring>

namespace aerend {

BitmapException::BitmapException(std::string msg) noexcept : std::runtime_error(msg) {}

BitmapException::BitmapException(std::string msg, const int errnum) : std::runtime_error(msg + ": " + std::strerror(errnum)) {}

}
