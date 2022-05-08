#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "Window.h"
#include "Cursor.h"
#include "Cursors.h"
#include "style/Colour.h"
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
    int32_t get_cursor_x();
    int32_t get_cursor_y();
    void update_cursor(Cursor* cursor);
    void update_cursor(int32_t dx, int32_t dy);
    void update_cursor(Cursor* cursor, int32_t dx, int32_t dy);
    void set_mouse_sensitivity(float mouse_sensitivity);
    void set_scroll_sensitivity(float scroll_sensitivity);
    float get_mouse_sensitivity();
    float get_scroll_sensitivity();
    void focus_on(std::vector<Widget*> widget);
    void unfocus();
    void grab(std::vector<Widget*> widget);
    void drop();
    void open_window(Window* window);
    void close_window(Window* window);
    void bump_window(Window* window);
    Window* get_window_at(int32_t x, int32_t y);
    std::vector<Widget*> get_widgets(Event* event);
    void push_update(std::function<void()> update);

    static const Colour def_bg_colour;
    std::unique_ptr<MergedUpdates> merged_updates;
private:
    std::vector<std::function<void()>> pop_updates();
    void run();
    std::thread thread;
    DRMCard card;
    FreeTypeLib ft_lib;
    Cursor* cursor;
    int32_t cursor_x = 0, cursor_y = 0;
    float mouse_sensitivity = 0.5, scroll_sensitivity = 1;
    std::vector<Window*> window_stack;
    std::vector<Widget*> focused, grabbed;
    std::queue<std::function<void()>> update_queue;
    std::mutex upq_mtx;
    std::condition_variable upq_cond;
    std::atomic<bool> running = true;
public:
    Cursors cursors;
};

}

#endif // DISPLAY_MANAGER_H

