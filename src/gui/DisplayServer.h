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
    std::vector<DRMBitmap> get_bmps();
    SimpleBitmap& get_bmp(Window* root);
private:
    DisplayServer();
    FreeTypeLib ft;
    DRMCard card;
};

}

#endif // DISPLAY_SERVER_H

