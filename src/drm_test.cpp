#include "drm/DRMCard.h"
#include "bitmap/SimpleBitmap.h"
#include "text/Font.h"
#include "text/TextException.h"
#include "text/Text.h"
#include "shape/Rectangle.h"
#include "shape/Ellipse.h"
#include "shape/Line.h"
#include "utils/Colour.h"
#include <atomic>
#include <csignal>
#include <iostream>
#include <cstdio>
#include <ctime>

std::atomic<bool> quit(false);

void handle_signal(int signum) {
    quit.store(true);
}

void test0(std::vector<std::shared_ptr<DRMConn>>& conns) {
    SimpleBitmap bmp{1920, 1080};
    bmp.fill(Colour::blue());
    for (int i = 0; !quit.load() && i < 100; i++) {
        for (auto conn = conns.begin(); conn != conns.end(); conn++) {
            auto buf = (*conn)->get_back_buf();
            buf.clear();
            std::clock_t start = std::clock();
            buf.composite(bmp, 0, 0, BlendMode::SRC);
            double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
            std::cerr << duration << std::endl;
            (*conn)->repaint();
        }
    }
    std::cerr << std::endl;
}

void test1(std::vector<std::shared_ptr<DRMConn>>& conns) {
    SimpleBitmap bmp{1920, 1080};
    bmp.fill(Colour::blue());
    for (int i = 0; !quit.load() && i < 100; i++) {
        for (auto conn = conns.begin(); conn != conns.end(); conn++) {
            auto buf = (*conn)->get_back_buf();
            buf.clear();
            std::clock_t start = std::clock();
            buf.composite(bmp, 0, 0, BlendMode::SRC_OVER);
            double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
            std::cerr << duration << std::endl;
            (*conn)->repaint();
        }
    }
    std::cerr << std::endl;
}

void test2(std::vector<std::shared_ptr<DRMConn>>& conns) {
    SimpleBitmap bmp{1920, 1080};
    bmp.fill(Colour::blue(127));
    for (int i = 0; !quit.load() && i < 100; i++) {
        for (auto conn = conns.begin(); conn != conns.end(); conn++) {
            auto buf = (*conn)->get_back_buf();
            buf.clear();
            std::clock_t start = std::clock();
            buf.composite(bmp, 0, 0, BlendMode::SRC_OVER);
            double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
            std::cerr << duration << std::endl;
            (*conn)->repaint();
        }
    }
    std::cerr << std::endl;
}

void test3(std::vector<std::shared_ptr<DRMConn>>& conns) {
    SimpleBitmap bmp{800, 500};
    SimpleBitmap bmp2{700, 400};
    bmp.fill(Colour::blue());
    bmp2.fill(Colour::red());
    for (int i = 0; !quit.load() && i < 100; i++) {
        for (auto conn = conns.begin(); conn != conns.end(); conn++) {
            auto buf = (*conn)->get_back_buf();
            buf.clear();
            std::clock_t start = std::clock();
            buf.composite(bmp, 200, 200, BlendMode::SRC_OVER);
            buf.composite(bmp2, 400, 400, BlendMode::SRC_OVER);
            double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
            std::cerr << duration << std::endl;
            (*conn)->repaint();
        }
    }
    std::cerr << std::endl;
}

void test4(std::vector<std::shared_ptr<DRMConn>>& conns) {
    SimpleBitmap bmp{800, 500};
    SimpleBitmap bmp2{700, 400};
    bmp.fill(Colour::blue(127));
    bmp2.fill(Colour::red(127));
    for (int i = 0; !quit.load() && i < 100; i++) {
        for (auto conn = conns.begin(); conn != conns.end(); conn++) {
            auto buf = (*conn)->get_back_buf();
            buf.clear();
            std::clock_t start = std::clock();
            buf.composite(bmp, 200, 200, BlendMode::SRC_OVER);
            buf.composite(bmp2, 400, 400, BlendMode::SRC_OVER);
            double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
            std::cerr << duration << std::endl;
            (*conn)->repaint();
        }
    }
    std::cerr << std::endl;
}

void test5(std::vector<std::shared_ptr<DRMConn>>& conns) {
    Font os{"/usr/share/fonts/TTF/OpenSans-Regular.ttf"};
    SimpleBitmap bmp{800, 500};
    SimpleBitmap bmp2{700, 400};
    Text txt{"Hello world!", os, 16, Colour::white(), 50, 50};
    for (int i = 0; !quit.load() && i < 100; i++) {
        bmp.fill(Colour::blue());
        bmp2.fill(Colour::red());
        for (auto conn = conns.begin(); conn != conns.end(); conn++) {
            auto buf = (*conn)->get_back_buf();
            buf.clear();
            std::clock_t start = std::clock();
            txt.paint(bmp2);
            buf.composite(bmp, 200, 200, BlendMode::SRC_OVER);
            buf.composite(bmp2, 400, 400, BlendMode::SRC_OVER);
            double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
            std::cerr << duration << std::endl;
            (*conn)->repaint();
        }
    }
    std::cerr << std::endl;
}

void test6(std::vector<std::shared_ptr<DRMConn>>& conns) {
    Font os{"/usr/share/fonts/TTF/OpenSans-Regular.ttf"};
    SimpleBitmap bmp{800, 500};
    SimpleBitmap bmp2{700, 400};
    Text txt{"Hello world!", os, 16, Colour::white(), 50, 50};
    for (int i = 0; !quit.load() && i < 100; i++) {
        bmp.fill(Colour::blue(127));
        bmp2.fill(Colour::red(127));
        for (auto conn = conns.begin(); conn != conns.end(); conn++) {
            auto buf = (*conn)->get_back_buf();
            buf.clear();
            std::clock_t start = std::clock();
            txt.paint(bmp2);
            buf.composite(bmp, 200, 200, BlendMode::SRC_OVER);
            buf.composite(bmp2, 400, 400, BlendMode::SRC_OVER);
            double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
            std::cerr << duration << std::endl;
            (*conn)->repaint();
        }
    }
    std::cerr << std::endl;
}

void test7(std::vector<std::shared_ptr<DRMConn>>& conns) {
    Font os{"/usr/share/fonts/TTF/OpenSans-Regular.ttf"};
    SimpleBitmap bmp{800, 500};
    SimpleBitmap bmp2{700, 400};
    Text txt{"Hello world!", os, 16, Colour::white(), 50, 50};
    Text txt2{"Hello world!", os, 16, Colour::white(), 50, 50};
    for (int i = 0; !quit.load() && i < 100; i++) {
        bmp.fill(Colour::blue(127));
        bmp2.fill(Colour::red(127));
        for (auto conn = conns.begin(); conn != conns.end(); conn++) {
            auto buf = (*conn)->get_back_buf();
            buf.clear();
            std::clock_t start = std::clock();
            txt.paint(bmp2);
            txt2.paint(bmp);
            buf.composite(bmp, 200, 200, BlendMode::SRC_OVER);
            buf.composite(bmp2, 400, 400, BlendMode::SRC_OVER);
            double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
            std::cerr << duration << std::endl;
            (*conn)->repaint();
        }
    }
    std::cerr << std::endl;
}

void test8(std::vector<std::shared_ptr<DRMConn>>& conns) {
    Font os{"/usr/share/fonts/TTF/OpenSans-Regular.ttf"};
    SimpleBitmap bmp{800, 500};
    SimpleBitmap bmp2{700, 400};
    Text txt{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 50};
    Text txt2{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 100};
    Text txt3{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 150};
    Text txt4{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 200};
    Text txt5{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 250};
    Text txt6{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 300};
    Text txt7{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 350};
    for (int i = 0; !quit.load() && i < 100; i++) {
        bmp.fill(Colour::blue(127));
        bmp2.fill(Colour::red(127));
        for (auto conn = conns.begin(); conn != conns.end(); conn++) {
            auto buf = (*conn)->get_back_buf();
            buf.clear();
            std::clock_t start = std::clock();
            txt.paint(bmp2);
            txt2.paint(bmp2);
            txt3.paint(bmp2);
            txt4.paint(bmp2);
            txt5.paint(bmp2);
            txt6.paint(bmp2);
            txt7.paint(bmp2);
            buf.composite(bmp, 200, 200, BlendMode::SRC_OVER);
            buf.composite(bmp2, 400, 400, BlendMode::SRC_OVER);
            double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
            std::cerr << duration << std::endl;
            (*conn)->repaint();
        }
    }
    std::cerr << std::endl;
}

void test9(std::vector<std::shared_ptr<DRMConn>>& conns) {
    Font os{"/usr/share/fonts/TTF/OpenSans-Regular.ttf"};
    SimpleBitmap bmp{800, 500};
    SimpleBitmap bmp2{700, 400};
    Text txt{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 50};
    Text txt2{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 100};
    Text txt3{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 150};
    Text txt4{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 200};
    Text txt5{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 250};
    Text txt6{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 300};
    Text txt7{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 350};
    Text txta{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 50};
    Text txt2a{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 100};
    Text txt3a{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 150};
    Text txt4a{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 200};
    Text txt5a{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 250};
    Text txt6a{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 300};
    Text txt7a{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 350};
    Text txt8a{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 400};
    Text txt9a{"Lorem ipsum dolor sit amet, consectetuer adipiscing elit", os, 16, Colour::white(), 50, 450};
    for (int i = 0; !quit.load() && i < 100; i++) {
        bmp.fill(Colour::blue(127));
        bmp2.fill(Colour::red(127));
        for (auto conn = conns.begin(); conn != conns.end(); conn++) {
            auto buf = (*conn)->get_back_buf();
            buf.clear();
            std::clock_t start = std::clock();
            txt.paint(bmp2);
            txt2.paint(bmp2);
            txt3.paint(bmp2);
            txt4.paint(bmp2);
            txt5.paint(bmp2);
            txt6.paint(bmp2);
            txt7.paint(bmp2);
            txta.paint(bmp);
            txt2a.paint(bmp);
            txt3a.paint(bmp);
            txt4a.paint(bmp);
            txt5a.paint(bmp);
            txt6a.paint(bmp);
            txt7a.paint(bmp);
            txt8a.paint(bmp);
            txt9a.paint(bmp);
            buf.composite(bmp, 200, 200, BlendMode::SRC_OVER);
            buf.composite(bmp2, 400, 400, BlendMode::SRC_OVER);
            double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
            std::cerr << duration << std::endl;
            (*conn)->repaint();
        }
    }
}

// TODO: drmSetMaster
int main() {
    std::signal(SIGINT, handle_signal);

    try {
        FreeTypeLib ft;

        DRMCard card{"/dev/dri/card0"};
        auto conns = card.get_conns();

        /*test0(conns);
        test1(conns);
        test2(conns);
        test3(conns);
        test4(conns);
        test5(conns);
        test6(conns);
        test7(conns);
        test8(conns);
        test9(conns);*/
        Rectangle r1{-50, -50, 400, 600, Colour::grey(), Border{Colour::red(), 5}};
        Rectangle r2{100, 100, 400, 600, Colour{0, 0, 255, 100}, Border{Colour::green(100), 10}};
        Rectangle r3{800, 800, 100, 100, Colour{255, 255, 0, 100}, Border{}};
        Ellipse e1{-100, -100, 400, 600, Colour::red(100), Border{Colour::cyan(100), 10}};
        Line l1{-8000, 4000, 100, 100, Colour::black(100)};
        for (auto conn = conns.begin(); conn != conns.end(); conn++) {
            auto buf = (*conn)->get_back_buf();
            buf.fill(Colour::white());
            r1.paint(buf);
            r2.paint(buf);
            r3.paint(buf);
            e1.paint(buf);
            l1.paint(buf);
            (*conn)->repaint();
        }

        while (!quit.load()) ;

    } catch (const std::exception& e) {
        std::cerr << "drm_test: " << e.what() << std::endl;
    }

    return 0;
}
