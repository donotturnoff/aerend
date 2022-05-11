#include "SimpleBitmap.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <stdexcept>

namespace aerend {

SimpleBitmap::SimpleBitmap(const int32_t w, const int32_t h) {
    this->map = nullptr;
    set_size(w, h);
}

SimpleBitmap::SimpleBitmap(const SimpleBitmap& bmp) {
    w = bmp.w;
    h = bmp.h;
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
    if (w < 0) throw std::invalid_argument("SimpleBitmap::set_size: width cannot be negative");
    if (h < 0) throw std::invalid_argument("SimpleBitmap::set_size: height cannot be negative");

    this->w = w;
    this->h = h;

    /* Allocate a buffer for the requested size */
    size = w*h*4;
    map = (uint32_t *) realloc(map, size);
    if (!map) {
        throw std::bad_alloc();
    }
}

}
