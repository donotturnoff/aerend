#include "Button.h"
#include "AerendServer.h"
#include <iostream>
#include <cstdio>

namespace aerend {

Button::Button(std::string str, Font font, int32_t size, Colour colour, Colour bg_colour, Border border, Margin margin) : rect(Rectangle{0, 0, 0, 0, bg_colour, border}), text(Text{str, font, size, colour, 0, 0, -1}), bmp(SimpleBitmap{}) {
    this->margin = margin;

    std::function<void(Event*)> on_enter = [this] (Event*) {
        auto& dm = AerendServer::the().get_display_manager();
        dm.set_cursor(dm.POINTER_CURSOR);
        this->set_bg_colour(Colour{224, 224, 224});
    };
    add_event_handler(EventType::MOUSE_ENTER, on_enter);

    std::function<void(Event*)> on_exit = [this] (Event*) {
        auto& dm = AerendServer::the().get_display_manager();
        dm.set_cursor(dm.ARROW_CURSOR);
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
    this->bg_colour = bg_colour;
    rect.set_colour(bg_colour);
    autorepaint();
}

void Button::set_border(Border border) noexcept {
    this->border = border;
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
