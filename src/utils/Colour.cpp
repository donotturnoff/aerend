#include "Colour.h"

uint32_t Colour::to_int() {
    return (a << 24) | (r << 16) | (g << 8) | b;
}
