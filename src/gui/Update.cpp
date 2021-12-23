#include "Update.h"

namespace aerend {

Update::Update(UpdateType type) : type(type) {}

UpdateType Update::get_type() const noexcept {
    return type;
}

}
