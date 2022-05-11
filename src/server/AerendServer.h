#ifndef AEREND_SERVER_H
#define AEREND_SERVER_H

#define TEMPERATURE_DEBUG 1

#include "event/EventDispatcher.h"
#include "input/InputHandler.h"
#include "input/DeviceDetector.h"
#include "gui/DisplayManager.h"
#include "net/ConnectionListener.h"
#include <chrono>

namespace aerend {

class AerendServer {
public:
    static AerendServer& the();
    EventDispatcher& ed();
    InputHandler& ih();
    DisplayManager& dm();
    ConnectionListener& cl();
    void start_timer();
    std::chrono::duration<double> stop_timer();
private:
    AerendServer() = default;
    AerendServer(const AerendServer&) = delete;
    void operator=(const AerendServer&) = delete;
    DeviceDetector dd_;
    EventDispatcher ed_;
    DisplayManager dm_;
    ConnectionListener cl_{5000};
    InputHandler ih_;
    std::chrono::steady_clock::time_point timer;
    bool timer_running{false};
};

}

#endif // AEREND_SERVER_H

