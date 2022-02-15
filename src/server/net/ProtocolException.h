#ifndef PROTOCOL_EXCEPTION_H
#define PROTOCOL_EXCEPTION_H

#include <string>
#include <stdexcept>
#include <cstdint>

namespace aerend {

class ProtocolException : public std::runtime_error {
public:
    ProtocolException(std::string msg) noexcept;
    ProtocolException(std::string msg, const uint8_t status);
    uint8_t get_status() const noexcept;
private:
    uint8_t status;
};

}

#endif // PROTOCOL_EXCEPTION_H

