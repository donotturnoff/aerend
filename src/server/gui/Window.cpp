#include "Window.h"
#include "AerendServer.h"
#include "DisplayManager.h"
#include "GridLayout.h"
#include "WindowLayout.h"
#include "Label.h"
#include "MergedUpdates.h"
#include "event/Event.h"
#include <iostream>
#include <cstdio>

namespace aerend {

const int32_t Window::def_x{0};
const int32_t Window::def_y{0};
const int32_t Window::def_w{600};
const int32_t Window::def_h{400};
const Colour Window::def_bg_colour{Colour::grey()};
const Padding Window::def_padding{2};
const Border Window::def_border{Colour::black(), 1};
const std::string Window::def_title{"Untitled window"};
const std::string Window::def_title_font_path{"res/lmsans12-regular.otf"};
const int32_t Window::def_title_font_size{12};
const int32_t Window::def_title_bar_height{24};

Window::Window(Client& client, int32_t x, int32_t y, int32_t w, int32_t h, std::string title) : Container(client, std::make_unique<WindowLayout>(), def_bg_colour, def_border, Margin{}, def_padding), bmp(SimpleBitmap{w, h}), title_bar(std::make_unique<Panel>(client, std::make_unique<GridLayout>(1, 1), Colour::grey())), frame(std::make_unique<Panel>(client)), title_label(std::make_unique<Label>(client, title, def_title_font_path, def_title_font_size, Colour::black(), def_bg_colour)), title(title), draggable(false) {

    set_pos(x, y);
    set_size(w, h);

    Rectangle rect{client, border.t, border.t, w-border.t*2, h-border.t*2, bg_colour, border};
    rect.paint(bmp);

    root = this;
    parent = this;

    std::function<void(Event*)> drag_window = [this] (Event* e) {
        if (e->is_left_down() && this->draggable) {
            auto& dm{AerendServer::the().dm()};
            dm.grab({title_bar.get(), this});
            dm.push_update([this] () {
                AerendServer::the().dm().merged_updates->follow_mouse(this);
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
        AerendServer::the().dm().drop();
    };
    title_bar->add_event_handler(EventType::MOUSE_RELEASE, stop_drag);

    std::function<void(Event*)> bump = [this] (Event*) {
        AerendServer::the().dm().push_update([this] () {
            this->bump(); // TODO: make bump use merged_updates
        });
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
    AerendServer::the().dm().repaint();
}

void Window::set_size(const int32_t w, const int32_t h) {
    if (w < 0 || h < 0) {
        throw GUIException("widget size cannot be negative");
    }
    this->w = w;
    this->h = h;
    bmp.set_size(w, h);
    autolayout();
    autorepaint();
}

std::string Window::get_title() const noexcept {
    return title;
}

SimpleBitmap& Window::get_bmp() noexcept {
    return bmp;
}

void Window::open() {
    AerendServer::the().dm().open_window(this);
    // TODO: necessary?
    autolayout();
    autorepaint();
}

void Window::close() {
    AerendServer::the().dm().close_window(this);
    AerendServer::the().dm().repaint();
}

void Window::bump() {
    AerendServer::the().dm().bump_window(this);
    AerendServer::the().dm().repaint();
}

void Window::add(Widget* child) {
#if defined(COMPLEX_BULB_DEBUG) || defined(TEMPERATURE_DEBUG)
    auto duration{AerendServer::the().stop_timer().count()};
    if (duration > 0) {
        std::cerr << duration << std::endl;
    }
#endif
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

    AerendServer::the().dm().repaint();
}

void Window::paint(Bitmap& dst) {
    dst.composite(bmp, x, y);
}

//TODO: reduce duplication with Container
void Window::get_widgets_at(std::vector<Widget*>& widgets, int32_t x, int32_t y) noexcept {
    if (x >= 0 && y >= 0 && x < this->x + w && y < this->y + h) {
        int32_t index{lm->index_from_position(*this, x, y)};
        if (index >= 0 && index < children.size()) { // Possibly child at position
            children[index]->get_widgets_at(widgets, x, y);
        }
        widgets.push_back(this);
    }
}

}
