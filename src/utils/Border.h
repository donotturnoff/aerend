#ifndef BORDER_H
#define BORDER_H

#include "Colour.h"
#include <cstdint>

struct Border {
    Border(Colour c = Colour::black(), int32_t t = 0, int32_t r = 0);

    Colour c;
    int32_t t, r;
};

#endif // BORDER_H

