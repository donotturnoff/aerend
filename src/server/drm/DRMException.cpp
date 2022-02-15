#include "drm/DRMException.h"
#include <cstring>

namespace aerend {

DRMException::DRMException(std::string msg) noexcept : std::runtime_error(msg) {}

DRMException::DRMException(std::string msg, const int errnum) : std::runtime_error(msg + ": " + std::strerror(errnum)) {}

DRMException::DRMException(std::string msg, const std::exception& e) : std::runtime_error(msg + ": " + e.what()) {}

}
