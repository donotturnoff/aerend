#ifndef BORDER_H
#define BORDER_H

#include "Colour.h"
#include <cstdint>

struct Border {
    Border(Colour c = Colour::black(), int32_t t = 0);

    Colour c;
    int32_t t;
};

#endif // BORDER_H

