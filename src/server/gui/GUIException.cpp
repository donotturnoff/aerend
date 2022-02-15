#include "GUIException.h"
#include <cstring>

namespace aerend {

GUIException::GUIException(std::string msg) noexcept : std::runtime_error(msg) {}

}
