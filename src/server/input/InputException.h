#ifndef INPUT_EXCEPTION_H
#define INPUT_EXCEPTION_H

#include <string>
#include <stdexcept>

namespace aerend {

class InputException : public std::runtime_error {
public:
    InputException(std::string msg) noexcept;
    InputException(std::string msg, const int errnum);
};

}

#endif // INPUT_EXCEPTION_H

