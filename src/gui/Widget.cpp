#include "Widget.h"
#include "AerendServer.h"
#include "DisplayManager.h"
#include <iostream>

namespace aerend {

Widget::Widget() : should_autorepaint(true), should_autolayout(true), root(nullptr), parent(nullptr), preferred_w(-1), preferred_h(-1), x(0), y(0), w(-1), h(-1), full_w(-1), full_h(-1) {};

void Widget::set_x(const int32_t x) noexcept {
    set_pos(x, y);
}

void Widget::set_y(const int32_t y) noexcept {
    set_pos(x, y);
}

void Widget::set_pos(const int32_t x, const int32_t y) noexcept {
    this->x = x;
    this->y = y;
}

void Widget::set_w(const int32_t w) {
    set_size(w, h);
}

void Widget::set_h(const int32_t h) {
    set_size(w, h);
}

void Widget::set_size(const int32_t w, const int32_t h) {
    if (w < 0 || h < 0) {
        throw GUIException("widget size cannot be negative");
    }
    this->w = w;
    this->h = h;
}

void Widget::set_preferred_w(const int32_t preferred_w) {
    set_preferred_size(preferred_w, preferred_h);
}

void Widget::set_preferred_h(const int32_t preferred_h) {
    set_preferred_size(preferred_w, preferred_h);
}

void Widget::set_preferred_size(const int32_t preferred_w, const int32_t preferred_h) {
    this->preferred_w = preferred_w;
    this->preferred_h = preferred_h;
}

void Widget::set_full_size(const int32_t full_w, const int32_t full_h) {
    int32_t extra = (border.t + margin.t) * 2;
    if (full_w < extra || full_h < extra ) {
        throw GUIException("full widget size cannot make interior size negative");
    }
    set_size(full_w - extra, full_h - extra);
}

void Widget::set_root(Window* root) noexcept {
    this->root = root;
}

void Widget::set_parent(Container* parent) noexcept {
    this->parent = parent;
}

void Widget::set_autorepaint(bool should_autorepaint) noexcept {
    this->should_autorepaint = should_autorepaint;
}

void Widget::set_autolayout(bool should_autolayout) noexcept {
    this->should_autolayout = should_autolayout;
}

void Widget::set_bg_colour(Colour bg_colour) noexcept {
    this->bg_colour = bg_colour;
    autorepaint();
}

void Widget::set_padding(Padding padding) noexcept {
    this->padding = padding;
    autolayout();
    autorepaint();
}

void Widget::set_border(Border border) noexcept {
    this->border = border;
    if (parent) {
        parent->autolayout();
        parent->autorepaint();
    }
}

void Widget::set_margin(Margin margin) noexcept {
    this->margin = margin;
    if (parent) {
        parent->autolayout();
        parent->autorepaint();
    }
}

int32_t Widget::get_x() const noexcept {
    return x;
}

int32_t Widget::get_y() const noexcept {
    return y;
}

int32_t Widget::get_w() const noexcept {
    return w;
}

int32_t Widget::get_h() const noexcept {
    return h;
}

int32_t Widget::get_preferred_w() const noexcept {
    return preferred_w;
}

int32_t Widget::get_preferred_h() const noexcept {
    return preferred_h;
}

int32_t Widget::get_full_w() const noexcept {
    int32_t extra = (border.t + margin.t) * 2;
    return w + extra;
}

int32_t Widget::get_full_h() const noexcept {
    int32_t extra = (border.t + margin.t) * 2;
    return h + extra;
}

int32_t Widget::get_preferred_full_w() const noexcept {
    int32_t extra = (border.t + margin.t) * 2;
    return (preferred_w < 0) ? preferred_w : preferred_w + extra;
}

int32_t Widget::get_preferred_full_h() const noexcept {
    int32_t extra = (border.t + margin.t) * 2;
    return (preferred_h < 0) ? preferred_h : preferred_h + extra;
}

Window* Widget::get_root() const noexcept {
    return root;
}

Container* Widget::get_parent() const noexcept {
    return parent;
}

Border Widget::get_border() const noexcept {
    return border;
}

Margin Widget::get_margin() const noexcept {
    return margin;
}

Padding Widget::get_padding() const noexcept {
    return padding;
}

void Widget::autorepaint() {
    if (should_autorepaint && root && parent) {
        repaint();
    }
}

void Widget::autolayout() {
    if (should_autolayout && root && parent) {
        layout();
    }
}

void Widget::repaint() {
    repaint(true);
}

void Widget::repaint(bool direct) {
    SimpleBitmap& bmp = AerendServer::the().get_display_manager().get_bmp(root);
    paint(bmp);
    if (direct) {
        AerendServer::the().get_display_manager().repaint();
    }
}

void Widget::layout() {

}

void Widget::add_event_handler(std::shared_ptr<EventHandler> handler) {
    event_handlers[(int) (handler->get_type())].insert(handler);
}

void Widget::rm_event_handler(std::shared_ptr<EventHandler> handler) {
    auto handlers = event_handlers[(int) (handler->get_type())];
    handlers.erase(handlers.find(handler));
}

std::set<std::shared_ptr<EventHandler>> Widget::get_event_handlers(EventType type) {
    return event_handlers[(int) type];
}

}
