#include "Window.h"
#include "AerendServer.h"
#include "DisplayManager.h"
#include "WindowLayout.h"
#include "Label.h"
#include "WidgetMap.h"
#include "MergedUpdates.h"
#include "event/Event.h"
#include <iostream>
#include <cstdio>

namespace aerend {

const char* Window::WIN_TITLE_FONT_PATH = "/usr/share/fonts/misc/ter-u16n.otb";
const int32_t Window::WIN_TITLE_FONT_SIZE = 12;

Window::Window(Client& client, int32_t x, int32_t y, int32_t w, int32_t h, std::string title) : Container(client), bmp(SimpleBitmap{w, h}), title(title), draggable(false) {
    set_pos(x, y);
    set_size(w, h);
    set_padding(2);

    bmp.fill(Colour::grey());

    lm = std::make_shared<WindowLayout>();
    root = this;
    parent = this;
    children = std::vector<std::shared_ptr<Widget>>{};

    std::shared_ptr<Panel> title_bar = std::make_shared<Panel>(client, std::make_shared<GridLayout>(2, 1), Colour::grey());
    std::shared_ptr<Label> title_label = std::make_shared<Label>(client, title, Font{WIN_TITLE_FONT_PATH}, WIN_TITLE_FONT_SIZE, Colour::black(), Colour::white(0));
    title_bar->add(title_label);

    std::function<void(Event*)> drag_window = [this] (Event* e) {
        if (e->is_left_down() && this->draggable) {
            AerendServer::the().get_display_manager().push_update([this] () {
                AerendServer::the().get_display_manager().merged_updates->follow_mouse(this);
            });
        }
    };
    title_bar->add_event_handler(EventType::MOUSE_MOVE, drag_window);

    std::function<void(Event*)> start_drag = [this] (Event* e) {
        if (e->is_left_down()) {
            this->draggable = true;
        }
    };
    title_bar->add_event_handler(EventType::MOUSE_PRESS, start_drag);

    std::function<void(Event*)> stop_drag = [this] (Event*) {
        this->draggable = false;
    };
    title_bar->add_event_handler(EventType::MOUSE_RELEASE, stop_drag);

    std::function<void(Event*)> bump = [this] (Event*) {
        this->bump();
    };
    add_event_handler(EventType::MOUSE_PRESS, bump);

    std::shared_ptr<Panel> frame = std::make_shared<Panel>(client);

    add(title_bar);
    add(frame);
}

void Window::set_title(std::string title) noexcept {
    this->title = title;
    // TODO: improve this
    std::static_pointer_cast<Label>(get_title_bar()->get_child(0))->set_str(title);
    autorepaint();
}

void Window::set_pos(const int32_t x, const int32_t y) noexcept {
    this->x = x;
    this->y = y;
    AerendServer::the().get_display_manager().remap();
    AerendServer::the().get_display_manager().repaint();
}

void Window::set_size(const int32_t w, const int32_t h) {
    if (w < 0 || h < 0) {
        throw GUIException("widget size cannot be negative");
    }
    this->w = w;
    this->h = h;
    bmp.set_size(w, h);
    wmp.set_size(w, h);
    AerendServer::the().get_display_manager().remap();
    autolayout();
    autorepaint();
}

std::string Window::get_title() const noexcept {
    return title;
}

SimpleBitmap& Window::get_bmp() noexcept {
    return bmp;
}

WidgetMap& Window::get_wmp() noexcept {
    return wmp;
}

std::shared_ptr<Panel> Window::get_title_bar() noexcept {
    return std::static_pointer_cast<Panel>(children[0]);
}

std::shared_ptr<Panel> Window::get_frame() noexcept {
    return std::static_pointer_cast<Panel>(children[1]);
}

Widget* Window::get_widget_at(int32_t x, int32_t y) {
    return wmp.get(x, y);
}

void Window::open() {
    AerendServer::the().get_display_manager().open_window(this);
    autolayout();
    autorepaint();
}

void Window::close() {
    AerendServer::the().get_display_manager().close_window(this);
    AerendServer::the().get_display_manager().repaint();
}

void Window::bump() {
    AerendServer::the().get_display_manager().bump_window(this);
    AerendServer::the().get_display_manager().repaint();
}

void Window::repaint() {
    repaint(true);
}

void Window::repaint(bool direct) {
    for (const auto& child: children) {
        child->repaint(false);
    }

    AerendServer::the().get_display_manager().repaint();
}

void Window::paint(Bitmap& dst) {
    dst.composite(bmp, x, y);
}

void Window::map_widget(Widget* widget) {
    wmp.add(widget);
}

}
