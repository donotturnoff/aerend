#include "drm/DRMCard.h"
#include "bitmap/SimpleBitmap.h"
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

    DRMCard card("/dev/dri/card0");
    auto conns = card.get_conns();
    SimpleBitmap bmp = SimpleBitmap(150, 150);
    bmp.fill(Colour{255, 0, 0, 128});
    for (auto conn = conns.begin(); conn < conns.end(); conn++) {
        DRMBitmap& buf = (*conn)->get_back_buf();
        for (int32_t i = 0; i < 150; i++) {
            for (int32_t j = 0; j < 150; j++) {
                buf.set_pixel(i, j, Colour{0, 0, 255, 255});
            }
        }
        buf.composite(bmp, 75, 75, OVER_BLEND);
        (*conn)->repaint();
    }

    while (!quit.load());

    return 0;
}
