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
#include <cstdio>
#include <ctime>
#include <fcntl.h>
#include <csignal>
#include <atomic>

std::atomic<bool> next(false);

void handle_signal(int signum) {
    next.store(true);
}

void wait() {
    while (!next.load()) ;
    next.store(false);
}

using namespace aerend;
int main() {
    std::signal(SIGINT, handle_signal);

    try {
        AerendServer::the();
        wait();
    } catch (const std::exception& e) {
        std::cerr << "gui_test: " << e.what() << std::endl;
    }

    return 0;
}
