#ifndef CURSORS_H
#define CURSORS_H

#include "Cursor.h"
#include <memory>

namespace aerend {

enum class CursorType {
    ARROW, POINTER, MAX_NUM=POINTER
};

class Cursors {
public:
    Cursors(int fd);
    Cursor* get_cursor(CursorType type);
private:
    static const uint32_t* MAPS[(int) CursorType::MAX_NUM+1];
    static const int32_t WS[(int) CursorType::MAX_NUM+1];
    static const int32_t HS[(int) CursorType::MAX_NUM+1];
    static const int32_t OFF_XS[(int) CursorType::MAX_NUM+1];
    static const int32_t OFF_YS[(int) CursorType::MAX_NUM+1];
    static const uint32_t ARROW_MAP[], POINTER_MAP[];
    std::unordered_map<CursorType, std::unique_ptr<Cursor>> cursors;
};

}

#endif // CURSORS_H

