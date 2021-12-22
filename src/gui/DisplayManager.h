#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "Window.h"
#include "Cursor.h"
#include "bitmap/SimpleBitmap.h"
#include "drm/DRMBitmap.h"
#include "drm/DRMCard.h"
#include "event/Event.h"
#include "text/FreeTypeLib.h"
#include <memory>
#include <vector>

namespace aerend {

class DisplayManager {
public:
    DisplayManager();
    void repaint();
    SimpleBitmap& get_bmp(Window* root);
    void set_cursor(std::shared_ptr<Cursor> cursor);
    void move_cursor(int32_t dx, int32_t dy);
    void add_win(Window* win);
    void rm_win(Window* win);
    void bump_win(Window* win);
    std::vector<Widget*> get_widgets(std::shared_ptr<Event> event);
    void handle_event(std::shared_ptr<Event> event);
    std::shared_ptr<Cursor> ARROW_CURSOR;
private:
    static uint32_t ARROW_MAP[];
    DRMCard card;
    FreeTypeLib ft_lib;
    std::shared_ptr<Cursor> cursor;
    int32_t cursor_x, cursor_y;
    std::vector<Window*> windows;
};

}

#endif // DISPLAY_MANAGER_H

