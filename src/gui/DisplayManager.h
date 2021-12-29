#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "Window.h"
#include "Cursor.h"
#include "Update.h"
#include "CursorUpdate.h"
#include "bitmap/SimpleBitmap.h"
#include "drm/DRMBitmap.h"
#include "drm/DRMCard.h"
#include "event/Event.h"
#include "text/FreeTypeLib.h"
#include <memory>
#include <vector>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <thread>

namespace aerend {

class DisplayManager {
public:
    DisplayManager();
    ~DisplayManager();
    void repaint();
    SimpleBitmap& get_bmp(Window* root);
    void set_cursor(std::shared_ptr<Cursor> cursor);
    void move_cursor(int32_t dx, int32_t dy);
    void register_widget(Widget* widget);
    void add_win(Window* win);
    void rm_win(Window* win);
    void bump_win(Window* win);
    std::vector<Widget*> get_widgets(std::shared_ptr<Event> event);
    void push_update(std::shared_ptr<Update> update);
    std::shared_ptr<Cursor> ARROW_CURSOR;
    Widget* focused;
private:
    std::vector<std::shared_ptr<Update>> pop_updates();
    void run();
    static uint32_t ARROW_MAP[];
    std::thread thread;
    DRMCard card;
    FreeTypeLib ft_lib;
    std::shared_ptr<Cursor> cursor;
    int32_t cursor_x, cursor_y;
    std::vector<Window*> windows;
    std::vector<Widget*> widget_map;
    std::queue<std::shared_ptr<Update>> update_queue;
    std::mutex upq_mtx, upq_cond_mtx;
    std::condition_variable upq_cond;
    std::atomic<bool> running;
};

}

#endif // DISPLAY_MANAGER_H

