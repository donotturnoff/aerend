#ifndef NETWORK_EXCEPTION_H
#define NETWORK_EXCEPTION_H

#include <string>
#include <stdexcept>

namespace aerend {

class NetworkException : public std::runtime_error {
public:
    NetworkException(std::string msg) noexcept;
    NetworkException(std::string msg, const int errnum);
};

}

#endif // NETWORK_EXCEPTION_H

