#include "AerendServer.h"
#include <iostream>

namespace aerend {

AerendServer& AerendServer::the() {
    static AerendServer instance;
    return instance;
}

EventDispatcher& AerendServer::ed() {
    return ed_;
}

InputHandler& AerendServer::ih() {
    return ih_;
}

DisplayManager& AerendServer::dm() {
    return dm_;
}

ConnectionListener& AerendServer::cl() {
    return cl_;
}

}
