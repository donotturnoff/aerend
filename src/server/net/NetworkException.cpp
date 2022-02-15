#include "NetworkException.h"
#include <cstring>

namespace aerend {

NetworkException::NetworkException(std::string msg) noexcept : std::runtime_error(msg) {}

NetworkException::NetworkException(std::string msg, const int errnum) : std::runtime_error(msg + ": " + std::strerror(errnum)) {}

}
