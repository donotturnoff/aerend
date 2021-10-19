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
        Text txt{"Hello world!amsfsldjkfnlskjdgnskldjbglsjblskdfjbgksldfhbgljsdbg;ljehvfiubvs;dlvihbs lrdkjv ;elrkgfn'wo3ih;2pi83gr8YHO*&({*IH@JPOJHPIUFVYEOTF:", os, 16, Colour{255, 255, 255}, 50, 100};
        Text txt2{"abcdefghijklmnopqrstuvwxyz", tm, 24, Colour{200, 200, 200}, 100, 200};
        Text txt3{"Lorem ipsum dolor sit amet", nsi, 20, Colour{255, 255, 200}, 100, 400};
        Text txt4{"Hello", cb, 100, Colour{200, 50, 50}, 400, 700};

        DRMCard card{"/dev/dri/card0"};
        auto conns = card.get_conns();
        SimpleBitmap bmp{150, 150};
        SimpleBitmap bmp2{200, 200};
        SimpleBitmap bmp3{50, 100};
        bmp.fill(Colour{255, 0, 0, 128});
        bmp2.fill(Colour{0, 0, 255, 255});
        bmp3.fill(Colour{0, 255, 0, 100});
        for (int i = 0; !quit.load() && i < 300; i++) {
            for (auto conn = conns.begin(); conn < conns.end(); conn++) {
                auto buf = (*conn)->get_back_buf();
                buf->clear();
                buf->composite(bmp2, i, 0, OVER_BLEND);
                buf->composite(bmp, 300-i, 75, OVER_BLEND);
                buf->composite(bmp3, 200, 50, OVER_BLEND);
                txt.paint(*buf);
                txt2.paint(*buf);
                txt3.paint(*buf);
                txt4.paint(*buf);
                (*conn)->repaint();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "drm_test: " << e.what() << std::endl;
    }

    return 0;
}
