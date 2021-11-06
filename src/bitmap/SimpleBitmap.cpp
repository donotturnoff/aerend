#include "SimpleBitmap.h"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>

SimpleBitmap::SimpleBitmap(const int32_t w, const int32_t h) {
    map = nullptr;
    set_size(w, h);
}

SimpleBitmap::SimpleBitmap(SimpleBitmap& bmp) {
    size = bmp.size;
    map = (uint32_t*) malloc(size);
    memcpy(map, bmp.map, bmp.size);
}

SimpleBitmap::SimpleBitmap(SimpleBitmap&& bmp) : SimpleBitmap() {
    swap(*this, bmp);
}

SimpleBitmap& SimpleBitmap::operator=(SimpleBitmap bmp) {
    swap(*this, bmp);
    return *this;
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
    map = (uint32_t *) realloc(map, size);
    if (!map) {
        throw std::bad_alloc(); // TODO: throw better exception
    }
}
