#include "Colour.h"

uint32_t Colour::to_int() {
    return (b << 24) | (g << 16) | (r << 8) | a;
}
