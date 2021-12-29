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
        AerendServer::the().run();

        Font os {"/usr/share/fonts/TTF/OpenSans-Regular.ttf"};

        Window win1{400, 200, 600, 400, "Test"};
        Window win2{800, 400, 600, 400, "Test"};
        win1.set_title("Window 1");
        win2.set_title("Window 2");

        auto pnl1 = std::make_shared<Panel>(std::make_shared<GridLayout>(std::vector<int32_t>{2, 1}, std::vector<int32_t>{2, 1, 2}));
        auto pnl2 = std::make_shared<Panel>();
        pnl2->set_bg_colour(Colour::blue());
        pnl2->set_border(Border{Colour::red(), 10});
        pnl2->set_margin(Margin{10});
        auto lbl1 = std::make_shared<Label>("Lorem ipsum dolor sit amet, consectetuer adipiscing elit.", os, 24, Colour::red(), Colour{200, 200, 200});
//        lbl1->set_margin(Margin{20});

        auto btn1 = std::make_shared<Button>("Click me!", os, 24);
        btn1->add_event_handler(std::make_shared<EventHandler>(EventType::MOUSE_MOVE));
        btn1->set_preferred_size(150, 50);

        auto cvs1 = std::make_shared<Canvas>(800, 800);
        Ellipse e1{10, 10, 100, 50, Colour::yellow()};
        win2.get_frame()->add(cvs1);

        pnl1->set_padding(Padding{20});

        pnl1->add(lbl1);
        win1.get_frame()->add(pnl1);
        pnl1->add(pnl2);
        pnl1->add(btn1);
        win1.open();
        wait();

        win2.open();
        cvs1->get_bmp().fill(Colour{50, 50, 50});
        e1.paint(cvs1->get_bmp());
        win2.repaint();
        uint32_t* pic_map = cvs1->get_bmp().get_map();
        auto pic1 = std::make_shared<Picture>(cvs1->get_cvs_w(), cvs1->get_cvs_h(), pic_map);
        pnl1->add(pic1);
        wait();

        win1.bump();
        wait();

        pnl1->rm(pnl2);
        win2.set_size(100, 100);
        win2.set_pos(1100, 700);
        wait();

        AerendServer::the().stop();

    } catch (const std::exception& e) {
        std::cerr << "drm_test: " << e.what() << std::endl;
    }

    return 0;
}
