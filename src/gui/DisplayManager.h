#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "Window.h"
#include "drm/DRMBitmap.h"
#include "drm/DRMCard.h"
#include "text/FreeTypeLib.h"
#include <memory>
#include <vector>

namespace aerend {

class DisplayManager {
public:
    DisplayManager();
    void repaint();
    SimpleBitmap& get_bmp(Window* root);
    void add_win(Window* win);
    void rm_win(Window* win);
    void bump_win(Window* win);
private:
    std::vector<Window*> windows;
    DRMCard card;
    FreeTypeLib ft_lib;
};

}

#endif // DISPLAY_MANAGER_H

