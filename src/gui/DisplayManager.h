#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "Window.h"
#include "Cursor.h"
#include "bitmap/SimpleBitmap.h"
#include "bitmap/DRMBitmap.h"
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
    void move_cursor(int32_t dx, int32_t dy);
    void set_mouse_sensitivity(float mouse_sensitivity);
    float get_mouse_sensitivity();
    void focus_on(Widget* widget);
    void unfocus();
    void add_win(Window* win);
    void rm_win(Window* win);
    void bump_win(Window* win);
    Window* get_window_at(int32_t x, int32_t y);
    std::vector<Widget*> get_widgets(std::shared_ptr<Event> event);
    void push_update(std::function<void()> update);
    std::unique_ptr<MergedUpdates> merged_updates;
private:
    std::vector<std::function<void()>> pop_updates();
    void run();
    std::thread thread;
    DRMCard card;
    FreeTypeLib ft_lib;
    std::shared_ptr<Cursor> cursor;
    int32_t cursor_x, cursor_y;
    float mouse_sensitivity;
    std::vector<Window*> windows;
    WidgetMap wmp;
    Widget* focused;
    std::queue<std::function<void()>> update_queue;
    std::mutex upq_mtx;
    std::condition_variable upq_cond;
    std::atomic<bool> running;
public:
    const std::shared_ptr<Cursor> ARROW_CURSOR, POINTER_CURSOR;
};

}

#endif // DISPLAY_MANAGER_H

