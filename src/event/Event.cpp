#include "Event.h"

namespace aerend {

Event::Event(EventType type) : type(type) {}

EventType Event::get_type() const noexcept {
    return type;
}

}
