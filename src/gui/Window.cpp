#include "Window.h"
#include "DisplayServer.h"
#include "WindowLayout.h"
#include "Label.h"
#include <iostream>

namespace aerend {

const char* Window::WIN_TITLE_FONT_PATH = "/usr/share/fonts/misc/ter-u16n.otb";
const int32_t Window::WIN_TITLE_FONT_SIZE = 12;

Window::Window(int32_t x, int32_t y, int32_t w, int32_t h, std::string title) : bmp(SimpleBitmap{w, h}), title(title) {
    set_pos(x, y);
    set_size(w, h);
    lm = std::make_shared<WindowLayout>();
    root = this;
    parent = this;
    children = std::vector<std::shared_ptr<Widget>>{};

    std::shared_ptr<Panel> title_bar = std::make_shared<Panel>(std::make_shared<GridLayout>(2, 1), Colour::grey());
    std::shared_ptr<Label> title_label = std::make_shared<Label>(title, Font{WIN_TITLE_FONT_PATH}, WIN_TITLE_FONT_SIZE, Colour::black(), Colour::white(0));
    title_bar->add(title_label);

    std::shared_ptr<Panel> frame = std::make_shared<Panel>();
    add(title_bar);
    add(frame);
}

void Window::set_title(std::string title) noexcept {
    this->title = title;
    // TODO: improve this
    std::static_pointer_cast<Label>(get_title_bar()->get_child(0))->set_str(title);
    autorepaint();
}

std::string Window::get_title() const noexcept {
    return title;
}

SimpleBitmap& Window::get_bmp() noexcept {
    return bmp;
}

std::shared_ptr<Panel> Window::get_title_bar() noexcept {
    return std::static_pointer_cast<Panel>(children[0]);
}

std::shared_ptr<Panel> Window::get_frame() noexcept {
    return std::static_pointer_cast<Panel>(children[1]);
}

void Window::open() {
    DisplayServer::the().add_win(this);
    autolayout();
    autorepaint();
}

void Window::close() {
    DisplayServer::the().rm_win(this);
    DisplayServer::the().repaint();
}

void Window::bump() {
    DisplayServer::the().bump_win(this);
    DisplayServer::the().repaint();
}

void Window::repaint() {
    for (const auto& child: children) {
        child->repaint();
    }

    DisplayServer::the().repaint();
}

void Window::paint(Bitmap& dst) {
    dst.composite(bmp, x, y);
}

}
