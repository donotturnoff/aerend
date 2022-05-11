#include "Container.h"
#include "AerendServer.h"
#include "DisplayManager.h"
#include <algorithm>
#include <iostream>

namespace aerend {

Container::Container(Client& client, std::unique_ptr<LayoutManager> lm, Colour bg_colour, Border border, Margin margin, Padding padding) : Widget(client, bg_colour, border, margin, padding), lm(std::move(lm)) {}

Container::~Container() {
    for (const auto& child: children) {
        child->set_root(nullptr);
        child->set_parent(nullptr);
    }
}

void Container::set_lm(std::unique_ptr<LayoutManager> lm) noexcept {
    this->lm = std::move(lm);
}

void Container::set_root(Window* root) noexcept {
    this->root = root;
    for (const auto& child: children) {
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
    if (std::count(children.begin(), children.end(), child) > 0) {
        children.erase(std::remove(children.begin(), children.end(), child));
        child->set_root(nullptr);
        child->set_parent(nullptr);
        autolayout();
        autorepaint();
    }
}

void Container::rm(uint32_t index) {
    if (index < children.size()) {
        rm(children[index]);
    }
}

void Container::repaint() {
    repaint(true);
}

void Container::repaint(bool direct) {
    SimpleBitmap& bmp = root->get_bmp();
    paint(bmp);
    for (const auto& child: children) {
        child->repaint(false);
    }
    if (direct) {
        /* Only make the display manager repaint the window if the repaint
         * request was issued directly to this widget, not if an ancestor
         * was repainted. In that case, the ancestor will ask the dm to repaint,
         * rather than it happening once per descendant. */
        AerendServer::the().dm().repaint();
    }
}

void Container::layout() {
    lm->reset();
    for (const auto& child: children) {
        lm->place(*this, *child);
        child->layout();
    }
}

void Container::paint(Bitmap& bmp) {}

void Container::get_widgets_at(std::vector<Widget*>& widgets, int32_t x, int32_t y) noexcept {
    if (contains_point(x, y)) {
        /* Ask layout manager to predict the index of the child which contains the point */
        int32_t index{lm->index_from_position(*this, x, y)};
        if (index >= 0 && (uint32_t) index < children.size()) {
            /* Possibly child at position, so recurse */
            children[index]->get_widgets_at(widgets, x, y);
        }
        widgets.push_back(this);
    }
}

}
