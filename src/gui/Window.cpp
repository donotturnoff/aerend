#include "Window.h"
#include "AerendServer.h"
#include "DisplayManager.h"
#include "GridLayout.h"
#include "WindowLayout.h"
#include "Label.h"
#include "WidgetMap.h"
#include "MergedUpdates.h"
#include "event/Event.h"
#include <iostream>
#include <cstdio>

namespace aerend {

const int32_t Window::def_x{0};
const int32_t Window::def_y{0};
const int32_t Window::def_w{100};
const int32_t Window::def_h{100};
const std::string Window::def_title{"Untitled window"};
const std::string Window::def_title_font_path{"res/lmsans12-regular.otf"};
const int32_t Window::def_title_font_size{12};
const int32_t Window::def_padding{2};
const int32_t Window::def_title_bar_height{24};

Window::Window(Client& client, int32_t x, int32_t y, int32_t w, int32_t h, std::string title) : Container(client, std::make_unique<WindowLayout>(), Colour::grey(), Border{}, Margin{}, Padding()), bmp(SimpleBitmap{w, h}), title_bar(std::make_unique<Panel>(client, std::make_unique<GridLayout>(2, 1), Colour::grey())), frame(std::make_unique<Panel>(client)), title_label(std::make_unique<Label>(client, title, def_title_font_path, def_title_font_size, Colour::black(), Colour::grey())), title(title), draggable(false) {

    set_pos(x, y);
    set_size(w, h);
    set_padding(def_padding);

    bmp.fill(Colour::grey());

    root = this;
    parent = this;

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

    title_bar->add(title_label.get());
    Container::add(title_bar.get());
    Container::add(frame.get());
}

Window::~Window() {
    close();
}

void Window::set_title(std::string title) noexcept {
    this->title = title;
    title_label->set_str(title);
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

void Window::add(Widget* child) {
    frame->add(child);
}

void Window::rm(Widget* child) {
    if (child == frame.get() || child == title_bar.get()) {
        Container::rm(child);
    } else {
        frame->rm(child);
    }
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
