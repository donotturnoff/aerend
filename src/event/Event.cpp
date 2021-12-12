#include "Event.h"

namespace aerend {

EventType Event::get_type() const noexcept {
    return type;
}

int Event::get_value() const noexcept {
    return value;
}

}
