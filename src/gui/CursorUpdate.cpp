#include "CursorUpdate.h"
#include <iostream>

namespace aerend {

CursorUpdate::CursorUpdate(UpdateType type, int32_t dx, int32_t dy) : Update(type), dx(dx), dy(dy) {
    // TODO: ensure this is one of the appropriate types
}

int32_t CursorUpdate::get_dx() const noexcept {
    return dx;
}

int32_t CursorUpdate::get_dy() const noexcept {
    return dy;
}

}
