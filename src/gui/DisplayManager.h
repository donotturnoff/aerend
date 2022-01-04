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

class MergedUpdates;

class DisplayManager {
public:
    DisplayManager();
    ~DisplayManager();
    void repaint();
    void remap();
    SimpleBitmap& get_bmp(Window* root);
    void set_cursor(std::shared_ptr<Cursor> cursor);
    void update_cursor_displacement(int32_t dx, int32_t dy);
    void move_cursor(int32_t dx, int32_t dy);
    void focus_on(Widget* widget);
    void unfocus();
    void add_win(Window* win);
    void rm_win(Window* win);
    void bump_win(Window* win);
    Window* get_window_at(int32_t x, int32_t y);
    std::vector<Widget*> get_widgets(std::shared_ptr<Event> event);
    void push_update(std::function<void()> update);
    std::shared_ptr<Cursor> ARROW_CURSOR;
    std::unique_ptr<MergedUpdates> merged_updates;
private:
    std::vector<std::function<void()>> pop_updates();
    void run();
    static uint32_t ARROW_MAP[];
    std::thread thread;
    DRMCard card;
    FreeTypeLib ft_lib;
    std::shared_ptr<Cursor> cursor;
    int32_t cursor_x, cursor_y, cursor_dx, cursor_dy;
    std::vector<Window*> windows;
    WidgetMap wmp;
    Widget* focused;
    std::queue<std::function<void()>> update_queue;
    std::mutex upq_mtx, upq_cond_mtx;
    std::condition_variable upq_cond;
    std::atomic<bool> running;
};

}

#endif // DISPLAY_MANAGER_H

