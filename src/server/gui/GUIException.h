#ifndef GUI_EXCEPTION_H
#define GUI_EXCEPTION_H

#include <string>
#include <stdexcept>

namespace aerend {

class GUIException : public std::runtime_error {
public:
    GUIException(std::string msg) noexcept;
};

}

#endif // GUI_EXCEPTION_H

