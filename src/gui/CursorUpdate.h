#ifndef CURSOR_UPDATE_H
#define CURSOR_UPDATE_H

#include "Update.h"
#include <cstdint>

namespace aerend {

class CursorUpdate : public Update {
public:
    CursorUpdate(UpdateType type, int32_t dx, int32_t dy);
    int32_t get_dx() const noexcept;
    int32_t get_dy() const noexcept;
private:
    int32_t dx, dy;
};

}

#endif // CURSOR_UPDATE_H

