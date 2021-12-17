#include "AerendServer.h"
#include "input/Mouse.h"
#include "input/Keyboard.h"
#include <iostream>

namespace aerend {

AerendServer& AerendServer::the() {
    static AerendServer instance;
    return instance;
}

AerendServer::AerendServer() {
    ih.add_device(std::make_shared<Mouse>("/dev/input/event11"));
    ih.add_device(std::make_shared<Keyboard>("/dev/input/event3"));
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

void AerendServer::run() {
    ed_thread = std::thread(&EventDispatcher::run, &ed);
    ih_thread = std::thread(&InputHandler::run, &ih);
}

void AerendServer::stop() {
    ed.stop();
    ih.stop();
    //ed_thread.join();
    //ih_thread.join();
}

}
