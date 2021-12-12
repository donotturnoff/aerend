#ifndef DISPLAY_SERVER_H
#define DISPLAY_SERVER_H

#include "Window.h"
#include "drm/DRMBitmap.h"
#include "drm/DRMCard.h"
#include "event/Event.h"
#include "text/FreeTypeLib.h"
#include <memory>
#include <vector>
#include <queue>
#include <mutex>

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
    void push_event(std::shared_ptr<Event> event);
    std::shared_ptr<Event> pop_event();
    void wait_for_mouse();
    void wait_for_keyboard();
    void wait_for_client();
    std::vector<Window*> windows;
    std::queue<std::shared_ptr<Event>> events;
    std::mutex events_mtx;
    DRMCard card;
    FreeTypeLib ft_lib;
};

}

#endif // DISPLAY_SERVER_H

