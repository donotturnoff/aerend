#include "AerendServer.h"
#include "gui/Window.h"
#include "gui/Panel.h"
#include "gui/Label.h"
#include "gui/Button.h"
#include "gui/Canvas.h"
#include "gui/Picture.h"
#include "shape/Ellipse.h"
#include "input/InputHandler.h"
#include "input/Keyboard.h"
#include "input/Mouse.h"
#include "event/EventDispatcher.h"
#include "event/Event.h"
#include <iostream>
#include <csignal>
#include <linux/kd.h>

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

bool halt{false};
std::condition_variable interrupt_cv;
std::mutex interrupt_mtx;

void interrupt(int signum) {
    std::lock_guard<std::mutex> lock{interrupt_mtx};
    halt = true;
    interrupt_cv.notify_one();
}

int main() {
    std::signal(SIGINT, interrupt);

    try {
        aerend::AerendServer::the();
        std::unique_lock<std::mutex> lock{interrupt_mtx};
        interrupt_cv.wait(lock, []{ return halt; });
    } catch (const std::exception& e) {
        std::cerr << "aerend: " << e.what() << std::endl;
    }

    return 0;
}
