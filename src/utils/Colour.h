#ifndef COLOUR_H
#define COLOUR_H

#include <cstdint>

enum BlendMode {
    OPAQUE_BLEND, OVER_BLEND, MULTIPLY_BLEND, SCREEN_BLEND,
    OVERLAY_BLEND, DIVIDE_BLEND, ADD_BLEND, SUBTRACT_BLEND,
    DIFFERENCE_BLEND, MAX_BLEND, MIN_BLEND
};

struct Colour {
    uint8_t r, g, b, a;
};

#endif // COLOUR_H
