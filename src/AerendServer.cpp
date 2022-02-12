#include "AerendServer.h"
#include <iostream>

namespace aerend {

AerendServer& AerendServer::the() {
    static AerendServer instance;
    return instance;
}

EventDispatcher& AerendServer::get_event_dispatcher() {
    return ed;
}

InputHandler& AerendServer::get_input_handler() {
    return ih;
}

DisplayManager& AerendServer::get_display_manager() {
    return dm;
}

ConnectionListener& AerendServer::get_connection_listener() {
    return cl;
}

}
