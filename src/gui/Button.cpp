#include "Button.h"
#include "AerendServer.h"
#include <iostream>
#include <cstdio>

namespace aerend {

Button::Button(std::string str, Font font, int32_t size, Colour colour, Colour bg_colour, Border border, Margin margin) : rect(Rectangle{0, 0, 0, 0, bg_colour, border}), text(Text{str, font, size, colour, 0, 0, -1}), bmp(SimpleBitmap{}) {
    this->margin = margin;
    std::function<void(std::shared_ptr<Event>)> spawn_action = [this] (std::shared_ptr<Event> event) {
        auto action_event = std::make_shared<ActionEvent>(this);
        AerendServer::the().get_event_dispatcher().dispatch(action_event, event_handlers[(int) EventType::ACTION]);
    };
    add_event_handler(EventType::MOUSE_CLICK, spawn_action);
}

void Button::set_str(std::string str) {
    text.set_str(str);
    autorepaint();
}

void Button::set_colour(Colour colour) {
    text.set_colour(colour);
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
    bmp.fill(Colour::white(0));
}

void Button::paint(Bitmap& dst) {
    rect.paint(dst);
    text.paint(bmp);
    dst.composite(bmp, x, y);
}

}
