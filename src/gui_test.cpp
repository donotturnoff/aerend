#include "gui/DisplayServer.h"
#include "gui/Window.h"
#include "gui/Panel.h"
#include "gui/Label.h"
#include "gui/Button.h"
#include "gui/Canvas.h"
#include "shape/Ellipse.h"
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

        Font os {"/usr/share/fonts/TTF/OpenSans-Regular.ttf"};

        Window win1{400, 200, 600, 400, "Test"};
        Window win2{800, 400, 600, 400, "Test"};
        win1.set_title("Window 1");
        win2.set_title("Window 2");

        std::shared_ptr<Panel> pnl1 = std::make_shared<Panel>(std::make_shared<GridLayout>(std::vector<int32_t>{2, 1}, std::vector<int32_t>{2, 1, 2}));
        std::shared_ptr<Panel> pnl2 = std::make_shared<Panel>();
        pnl2->set_bg_colour(Colour::blue());
        pnl2->set_border(Border{Colour::red(), 30});
        pnl2->set_margin(Margin{10});
        std::shared_ptr<Label> lbl1 = std::make_shared<Label>("Lorem ipsum dolor sit amet, consectetuer adipiscing elit.", os, 24, Colour::red(), Colour{200, 200, 200});
        lbl1->set_margin(Margin{20});

        std::shared_ptr<Button> btn1 = std::make_shared<Button>("Click me!", os, 24);
        btn1->set_preferred_size(150, 50);

        std::shared_ptr<Canvas> cvs1 = std::make_shared<Canvas>();
        Ellipse e1{10, 10, 100, 50, Colour::yellow()};
        win2.get_frame()->add(cvs1);

        pnl1->add(lbl1);
        win1.get_frame()->add(pnl1);
        pnl1->add(pnl2);
        pnl1->add(btn1);
        win1.open();
        wait();
        win2.open();
        cvs1->get_bmp().fill(Colour{200, 200, 200});
        e1.paint(cvs1->get_bmp());
        win2.repaint();
        wait();
        win1.bump();
        wait();
        pnl1->rm(pnl2);
        wait();
        win2.set_size(100, 100);
        win2.set_pos(1100, 700);
        wait();
    } catch (const std::exception& e) {
        std::cerr << "drm_test: " << e.what() << std::endl;
    }

    return 0;
}
