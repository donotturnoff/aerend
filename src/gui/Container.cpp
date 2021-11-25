#include "Container.h"
#include "DisplayServer.h"
#include <algorithm>
#include <iostream>

namespace aerend {

void Container::set_lm(std::shared_ptr<LayoutManager> lm) noexcept {
    this->lm = lm;
}

void Container::set_root(Window* root) noexcept {
    this->root = root;
    for (auto const& child: children) {
        child->set_root(root);
    }
}

void Container::add(std::shared_ptr<Widget> child) {
    auto cur_parent = child->get_parent();
    if (cur_parent) {
        cur_parent->rm(child);
    }
    children.push_back(child);
    child->set_parent(this);
    child->set_root(root);
}

void Container::rm(std::shared_ptr<Widget> child) {
    auto i = std::find(children.begin(), children.end(), child);
    if (i != children.end()) {
        children.erase(i);
        child->set_root(nullptr);
        child->set_parent(nullptr);
    }
    autolayout();
    autorepaint();
}

std::shared_ptr<Widget> Container::get_child(const int32_t i) const noexcept {
    return children[i];
}

void Container::repaint() {
    SimpleBitmap& bmp = DisplayServer::the().get_bmp(root);
    paint(bmp);
    for (const auto& child: children) {
        child->repaint();
    }
    DisplayServer::the().repaint();
}

void Container::layout() {
    // TODO: change more loops to this syntax
    lm->reset();
    for (const auto& child: children) {
        lm->place(*this, *child);
        child->layout();
    }
}

void Container::paint(Bitmap& bmp) {

}

}
