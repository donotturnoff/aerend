#include "gui/DisplayServer.h"
#include "gui/Window.h"
#include "gui/Panel.h"
#include <atomic>
#include <csignal>
#include <iostream>
#include <cstdio>
#include <ctime>

using namespace aerend;

std::atomic<bool> quit(false);

void handle_signal(int signum) {
    quit.store(true);
}


// TODO: drmSetMaster
int main() {
    std::signal(SIGINT, handle_signal);

    try {
        Window win1;
        std::shared_ptr<Panel> pnl1 = std::make_shared<Panel>();
        win1.get_frame()->add(pnl1);
        win1.layout();
        win1.repaint();

        while (!quit.load()) ;

    } catch (const std::exception& e) {
        std::cerr << "drm_test: " << e.what() << std::endl;
    }

    return 0;
}
