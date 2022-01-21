#include "Button.h"
#include "AerendServer.h"
#include "MergedUpdates.h"
#include <iostream>
#include <cstdio>

namespace aerend {

const std::string Button::str{"Button"};
const std::string Button::font_path{"/usr/share/fonts/misc/ter-u16n.otb"};
const int32_t Button::font_size{12};
const Colour Button::colour{32, 32, 32};
const Colour Button::bg_colour{223, 223, 223};
const Border Button::border{Colour::black(), 2};
const Margin Button::margin{5};
const Padding Button::padding{5};
const int32_t Button::wrap{0};

Button::Button(Client& client, const std::string str, const std::string font_path, const int32_t size, const Colour colour, const Colour bg_colour, const Border border, const Margin margin, const Padding padding, const int32_t wrap) : Widget(client, bg_colour, border, margin, padding), rect(Rectangle{0, 0, 0, 0, bg_colour, border}), text(Text{str, font_path, size, colour, 0, 0, wrap}) {
    std::function<void(Event*)> on_enter = [this] (Event*) {
        auto& dm = AerendServer::the().get_display_manager();
        dm.merged_updates->set_cursor(dm.cursors.get_cursor(CursorType::POINTER));
        this->set_bg_colour(Colour{224, 224, 224});
    };
    add_event_handler(EventType::MOUSE_ENTER, on_enter);

    std::function<void(Event*)> on_exit = [this] (Event*) {
        auto& dm = AerendServer::the().get_display_manager();
        dm.merged_updates->set_cursor(dm.cursors.get_cursor(CursorType::ARROW));
        this->set_bg_colour(Colour{192, 192, 192});
    };
    add_event_handler(EventType::MOUSE_EXIT, on_exit);
}

void Button::set_str(std::string str) {
    bmp.fill(Colour::clear());
    text.set_str(str);
    text.paint(bmp);
    autorepaint();
}

void Button::set_colour(Colour colour) {
    bmp.fill(Colour::clear());
    text.set_colour(colour);
    text.paint(bmp);
    autorepaint();
}

void Button::set_bg_colour(Colour bg_colour) noexcept {
    Widget::bg_colour = bg_colour;
    rect.set_colour(bg_colour);
    autorepaint();
}

void Button::set_border(Border border) noexcept {
    Widget::border = border;
    rect.set_border(border);
    if (parent) {
        parent->autolayout();
        parent->autorepaint();
    }
}

void Button::set_wrap(const int32_t wrap) {
    text.set_wrap(wrap);
    autorepaint();
}

void Button::set_pos(const int32_t x, const int32_t y) noexcept {
    this->x = x;
    this->y = y;
    rect.set_pos(x, y);
}

void Button::set_size(const int32_t w, const int32_t h) {
    this->w = w;
    this->h = h;
    rect.set_size(w, h);
    bmp.set_size(w, h);
    bmp.fill(Colour::clear());
    text.paint(bmp);
}

void Button::paint(Bitmap& dst) {
    rect.paint(dst);
    dst.composite(bmp, x, y);
}

}
