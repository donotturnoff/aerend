#include "drm/DRMException.h"
#include <cstring>
#include <string>
#include <exception>

DRMException::DRMException(const char* msg) noexcept : msg(msg) {
    this->msg = msg;
}

DRMException::DRMException(const char* msg, int errnum) {
    std::string buf(msg);
    buf.append(": ");
    buf.append(std::strerror(errnum));
    this->msg = strdup(buf.c_str());
}

DRMException::DRMException(const char* msg, const std::exception& e) {
    std::string buf(msg);
    buf.append(": ");
    buf.append(e.what());
    this->msg = strdup(buf.c_str());
}

const char* DRMException::what() const noexcept {
    return msg;
}
