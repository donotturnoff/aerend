#include "IEventHandler.h"
#include <iostream>

namespace aerend {

IEventHandler::IEventHandler(EventType type) : type(type) {}

void IEventHandler::handle(std::shared_ptr<Event> event) {}

}
