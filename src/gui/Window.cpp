#include "Window.h"
#include "DisplayServer.h"
#include "WindowLayout.h"
#include <iostream>

namespace aerend {

Window::Window(int32_t x, int32_t y, int32_t w, int32_t h, std::string title) : bmp(SimpleBitmap{w, h}), title(title) {
    set_pos(x, y);
    set_size(w, h);
    lm = std::make_shared<WindowLayout>();
    root = this;
    parent = nullptr;
    children = std::vector<std::shared_ptr<Widget>>{};
    std::shared_ptr<Panel> title_bar = std::make_shared<Panel>(std::make_shared<GridLayout>(2, 1), Colour::grey());
    std::shared_ptr<Panel> frame = std::make_shared<Panel>();
    add(title_bar);
    add(frame);
}

void Window::set_title(std::string title) noexcept {
    this->title = title;
}

SimpleBitmap& Window::get_bmp() noexcept {
    return bmp;
}

std::shared_ptr<Panel> Window::get_frame() noexcept {
    return std::static_pointer_cast<Panel>(children[1]);
}

void Window::repaint() {
    for (const auto& child: children) {
        child->repaint();
    }

    auto bmps {DisplayServer::the().get_bmps()};
    for (auto& bmp: bmps) {
        paint(bmp);
    }
    DisplayServer::the().repaint();
}

void Window::paint(Bitmap& dst) {
    dst.composite(bmp, x, y);
}

}
