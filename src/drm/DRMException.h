#ifndef DRM_EXCEPTION_H
#define DRM_EXCEPTION_H

#include <exception>

class DRMException : public std::exception {
public:
    DRMException(const char* msg) noexcept;
    DRMException(const char* msg, int errnum);
    DRMException(const char* msg, const std::exception& e);
    const char* what() const noexcept;
private:
    const char* msg;
};

#endif // DRM_EXCEPTION_H
