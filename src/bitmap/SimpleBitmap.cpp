#include "SimpleBitmap.h"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <exception>

SimpleBitmap::SimpleBitmap() : SimpleBitmap(0, 0) {}

SimpleBitmap::SimpleBitmap(const int32_t w, const int32_t h) {
    map = nullptr;
    set_size(w, h);
}

SimpleBitmap::~SimpleBitmap() {
    free(map);
}

void SimpleBitmap::set_size(const int32_t w, const int32_t h) {
    assert(w >= 0);
    assert(h >= 0);

    this->w = w;
    this->h = h;

    size = w*h*4; // TODO: compute size better
    map = (uint8_t *) realloc(map, size);
    if (!map) {
        throw std::bad_alloc(); // TODO: throw better exception
    }
}
