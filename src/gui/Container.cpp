#include "Container.h"
#include "AerendServer.h"
#include "DisplayManager.h"
#include <algorithm>
#include <iostream>

namespace aerend {

Container::Container(Client& client) : Widget(client) {}

void Container::set_lm(std::shared_ptr<LayoutManager> lm) noexcept {
    this->lm = lm;
}

void Container::set_root(Window* root) noexcept {
    this->root = root;
    for (auto const& child: children) {
        child->set_root(root);
    }
}

void Container::add(Widget* child) {
    auto cur_parent = child->get_parent();
    if (cur_parent) {
        cur_parent->rm(child);
    }
    children.push_back(child);
    child->set_parent(this);
    child->set_root(root);
    autolayout();
    autorepaint();
}

void Container::rm(Widget* child) {
    auto i = std::find(children.begin(), children.end(), child);
    if (i != children.end()) {
        children.erase(i);
        child->set_root(nullptr);
        child->set_parent(nullptr);
    }
    autolayout();
    autorepaint();
}

void Container::repaint() {
    repaint(true);
}

void Container::repaint(bool direct) {
    SimpleBitmap& bmp = AerendServer::the().get_display_manager().get_bmp(root);
    paint(bmp);
    for (const auto& child: children) {
        child->repaint(false);
    }
    if (direct) {
        AerendServer::the().get_display_manager().repaint();
    }
}

void Container::layout() {
    root->map_widget(this);
    lm->reset();
    // TODO: change more loops to this syntax
    for (const auto& child: children) {
        lm->place(*this, *child);
        child->layout();
    }
}

void Container::paint(Bitmap& bmp) {

}

}
