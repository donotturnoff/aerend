#ifndef AEREND_SERVER_H
#define AEREND_SERVER_H

#include "event/EventDispatcher.h"
#include "input/InputHandler.h"
#include "input/DeviceDetector.h"
#include "gui/DisplayManager.h"
#include "net/ConnectionListener.h"

namespace aerend {

class AerendServer {
public:
    static AerendServer& the();
    EventDispatcher& get_event_dispatcher();
    InputHandler& get_input_handler();
    DisplayManager& get_display_manager();
private:
    AerendServer() = default;
    AerendServer(const AerendServer&) = delete;
    void operator=(const AerendServer&) = delete;
    EventDispatcher ed;
    InputHandler ih;
    DisplayManager dm;
    DeviceDetector dd;
    ConnectionListener cl{5000};
};

}

#endif // AEREND_SERVER_H

