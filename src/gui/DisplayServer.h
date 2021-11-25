#ifndef DISPLAY_SERVER_H
#define DISPLAY_SERVER_H

#include "Window.h"
#include "drm/DRMBitmap.h"
#include "drm/DRMCard.h"
#include "text/FreeTypeLib.h"
#include <memory>
#include <vector>

namespace aerend {

class DisplayServer {
public:
    static DisplayServer& the();
    DisplayServer(const DisplayServer&) = delete;
    void operator=(const DisplayServer&) = delete;
    void repaint();
    SimpleBitmap& get_bmp(Window* root);
    void add_win(Window* win);
    void rm_win(Window* win);
    void bump_win(Window* win);
private:
    DisplayServer();
    std::vector<Window*> windows;
    DRMCard card;
    FreeTypeLib ft_lib;
};

}

#endif // DISPLAY_SERVER_H

