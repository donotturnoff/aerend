#include "DisplayManager.h"
#include <iostream>

namespace aerend {

DisplayManager::DisplayManager() : card(DRMCard{"/dev/dri/card0"}) {}

void DisplayManager::repaint() {
    for (const auto& conn: card.get_conns()) {
        conn->get_back_buf().clear();
        for (const auto& win: windows) {
            conn->get_back_buf().composite(win->get_bmp(), win->get_x(), win->get_y());
        }
        conn->repaint();
    }
}

void DisplayManager::add_win(Window* win) {
    windows.push_back(win);
}

void DisplayManager::rm_win(Window* win) {
    auto i = std::find(windows.begin(), windows.end(), win);
    if (i != windows.end()) {
        windows.erase(i);
    }
}

void DisplayManager::bump_win(Window* win) {
    rm_win(win);
    add_win(win);
}

SimpleBitmap& DisplayManager::get_bmp(Window* window) {
    return window->get_bmp();
}

}
