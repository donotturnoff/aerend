#include "InputException.h"
#include <cstring>

namespace aerend {

InputException::InputException(std::string msg) noexcept : std::runtime_error(msg) {}

InputException::InputException(std::string msg, const int errnum) : std::runtime_error(msg + ": " + std::strerror(errnum)) {}

}
