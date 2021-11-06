#ifndef DRM_EXCEPTION_H
#define DRM_EXCEPTION_H

#include <string>
#include <stdexcept>

class DRMException : public std::runtime_error {
public:
    DRMException(std::string msg) noexcept;
    DRMException(std::string msg, const int errnum);
    DRMException(std::string msg, const std::exception& e);
};

#endif // DRM_EXCEPTION_H
