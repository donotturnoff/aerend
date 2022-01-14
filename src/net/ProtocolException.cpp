#include "ProtocolException.h"
#include <cstring>

namespace aerend {

ProtocolException::ProtocolException(std::string msg) noexcept : std::runtime_error(msg) {}

ProtocolException::ProtocolException(std::string msg, const uint8_t status) : std::runtime_error(msg), status(status) {}

uint8_t ProtocolException::get_status() const noexcept {
    return status;
}

}
