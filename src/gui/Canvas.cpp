#include "Canvas.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

namespace aerend {

Canvas::Canvas(Client& client, int32_t cvs_w, int32_t cvs_h) : Widget(client), cvs_w(cvs_w), cvs_h(cvs_h), bmp(SimpleBitmap{cvs_w, cvs_h}) {}

SimpleBitmap& Canvas::get_bmp() noexcept {
    return bmp;
}

void Canvas::set_pos(const int32_t x, const int32_t y) noexcept {
    this->x = x;
    this->y = y;
}

void Canvas::set_size(const int32_t w, const int32_t h) {
    this->w = w;
    this->h = h;
}

int32_t Canvas::get_cvs_w() const noexcept {
    return bmp.get_w();
}

int32_t Canvas::get_cvs_h() const noexcept {
    return bmp.get_h();
}

void Canvas::paint(Bitmap& dst) {
    dst.composite(bmp, x, y, 0, 0, std::min(w, bmp.get_w()), std::min(h, bmp.get_h()), BlendMode::SRC_OVER);
}

void Canvas::draw(Shape& shape) {
    shape.paint(bmp);
    autorepaint();
}

void Canvas::clear() {
    bmp.clear();
    autorepaint();
}

void Canvas::fill(Colour colour) {
    bmp.fill(colour);
    autorepaint();
}

}
