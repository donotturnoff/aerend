#include "Widget.h"
#include "DisplayServer.h"
#include <iostream>

namespace aerend {

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
    set_pos(w, h);
}

void Widget::set_h(const int32_t h) {
    set_pos(w, h);
}

void Widget::set_size(const int32_t w, const int32_t h) {
    if (w < 0 || h < 0) {
        throw GUIException("widget size cannot be negative");
    }
    this->w = w;
    this->h = h;
}

void Widget::set_root(Window* root) noexcept {
    this->root = root;
}

void Widget::set_parent(Container* parent) noexcept {
    this->parent = parent;
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

Window* Widget::get_root() const noexcept {
    return root;
}

Container* Widget::get_parent() const noexcept {
    return parent;
}

void Widget::repaint() {
    SimpleBitmap& bmp = DisplayServer::the().get_bmp(root);
    paint(bmp);
}

void Widget::layout() {

}

}
