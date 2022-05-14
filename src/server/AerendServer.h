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
    EventDispatcher& ed();
    InputHandler& ih();
    DisplayManager& dm();
    ConnectionListener& cl();
private:
    AerendServer() = default;
    AerendServer(const AerendServer&) = delete;
    void operator=(const AerendServer&) = delete;
    DeviceDetector dd_;
    EventDispatcher ed_;
    DisplayManager dm_;
    ConnectionListener cl_{5000};
    InputHandler ih_;
};

}

#endif // AEREND_SERVER_H

