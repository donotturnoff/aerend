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
}

std::shared_ptr<Widget> Container::get_child(const int32_t i) const noexcept {
    return children[i];
}

void Container::repaint() {
    SimpleBitmap& bmp = DisplayServer::the().get_bmp(root);
    paint(bmp);
    for (auto const& child: children) {
        child->repaint();
    }
}

void Container::layout() {
    // TODO: const auto&?
    // TODO: change more loops to this syntax
    for (auto const& child: children) {
        lm->place(*this, *child);
        child->layout();
    }
}

void Container::paint(Bitmap& bmp) {

}

}
