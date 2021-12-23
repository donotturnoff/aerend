#ifndef AEREND_SERVER_H
#define AEREND_SERVER_H

#include "event/EventDispatcher.h"
#include "input/InputHandler.h"
#include "gui/DisplayManager.h"
#include <thread>

namespace aerend {

class AerendServer {
public:
    static AerendServer& the();
    EventDispatcher& get_event_dispatcher();
    InputHandler& get_input_handler();
    DisplayManager& get_display_manager();
    void run();
    void stop();
private:
    AerendServer();
    AerendServer(const AerendServer&) = delete;
    void operator=(const AerendServer&) = delete;
    EventDispatcher ed;
    InputHandler ih;
    DisplayManager dm;
    std::thread ed_thread, ih_thread, dm_thread;
};

}

#endif // AEREND_SERVER_H

