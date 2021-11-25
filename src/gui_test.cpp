#include "gui/DisplayServer.h"
#include "gui/Window.h"
#include "gui/Panel.h"
#include "gui/Label.h"
#include <atomic>
#include <csignal>
#include <iostream>
#include <cstdio>
#include <ctime>

using namespace aerend;

std::atomic<bool> next(false);

void handle_signal(int signum) {
    next.store(true);
}

void wait() {
    while (!next.load()) ;
    next.store(false);
}

int main() {
    std::signal(SIGINT, handle_signal);

    try {
        DisplayServer::the();

        Window win1{400, 200, 600, 400, "Test"};
        Window win2{800, 400, 600, 400, "Test"};
        win1.set_title("Window 1");
        win2.set_title("Window 2");
        std::shared_ptr<Panel> pnl1 = std::make_shared<Panel>(std::make_shared<GridLayout>(2, 3), Colour::green());
        std::shared_ptr<Panel> pnl2 = std::make_shared<Panel>();
        pnl2->set_colour(Colour::blue());
        std::shared_ptr<Label> lbl1 = std::make_shared<Label>("Lorem ipsum dolor sit amet, consectetuer adipiscing elit.", Font{"/usr/share/fonts/TTF/OpenSans-Regular.ttf"}, 24);
        pnl1->add(lbl1);
        win1.get_frame()->add(pnl1);
        pnl1->add(pnl2);
        win1.open();
        wait();
        win2.open();
        wait();
        win1.bump();
        wait();
        pnl1->rm(pnl2);
        wait();
    } catch (const std::exception& e) {
        std::cerr << "drm_test: " << e.what() << std::endl;
    }

    return 0;
}
