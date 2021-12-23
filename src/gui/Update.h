#ifndef UPDATE_H
#define UPDATE_H

namespace aerend {

enum class UpdateType {
    HALT, CURSOR_MOVE
};

class Update {
public:
    Update(UpdateType type);
    UpdateType get_type() const noexcept;
protected:
    UpdateType type;
};

}

#endif // UPDATE_H

