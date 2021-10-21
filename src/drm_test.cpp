#include "drm/DRMCard.h"
#include "bitmap/SimpleBitmap.h"
#include "text/Font.h"
#include "text/TextException.h"
#include "text/Text.h"
#include "utils/Colour.h"
#include <atomic>
#include <csignal>
#include <iostream>
#include <cstdio>

std::atomic<bool> quit(false);

void handle_signal(int signum) {
    quit.store(true);
}

// TODO: drmSetMaster
int main() {
    std::signal(SIGINT, handle_signal);

    try {
        FreeTypeLib ft;

        Font os{"/usr/share/fonts/truetype/open-sans/OpenSans-Regular.ttf"};
        Font nsi{"/usr/share/fonts/truetype/noto/NotoSerif-Italic.ttf"};
        Font tm{"/usr/share/fonts/opentype/terminus/terminus-normal.otb"};
        Font cb{"/usr/local/share/fonts/cloisterblack.ttf"};
        Text txt{"Hello world!amsfsldjkfnlskjdgnskldjbglsjblskdfjbgksldfhbgljsdbg;ljehvfiubvs;dlvihbs lrdkjv ;elrkgfn'wo3ih;2pi83gr8YHO*&({*IH@JPOJHPIUFVYEOTF:", os, 16, Colour::white(), 50, 100};
        Text txt2{"abcdefghijklmnopqrstuvwxyz", tm, 24, Colour::green(), 100, 50};
        Text txt3{"Lorem ipsum dolor sit amet", nsi, 20, Colour::grey(), 100, 400};
        Text txt4{"Hello", cb, 20, Colour::white(), 10, 50};

        DRMCard card{"/dev/dri/card0"};
        auto conns = card.get_conns();
        SimpleBitmap bmp{600, 150};
        SimpleBitmap bmp2{200, 200};
        SimpleBitmap bmp3{50, 100};
        bmp2.fill(Colour::blue());
        bmp3.fill(Colour::green(100));
        for (int i = 0; !quit.load() && i < 300; i++) {
            for (auto conn = conns.begin(); conn < conns.end(); conn++) {
                bmp.fill(Colour::red(0));
                auto buf = (*conn)->get_back_buf();
                buf->clear();
                txt.paint(*buf);
                txt2.paint(bmp);
                txt3.paint(*buf);
                txt4.paint(bmp);
                bmp.composite(bmp3, 200, 50, BlendMode::SRC_OVER);
                buf->composite(bmp2, i, 0, BlendMode::SRC_OVER);
                buf->composite(bmp, 300-i, 75, BlendMode::SRC_OVER);
                buf->composite(bmp3, 500, 500, BlendMode::SRC_OVER);
                (*conn)->repaint();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "drm_test: " << e.what() << std::endl;
    }

    return 0;
}
