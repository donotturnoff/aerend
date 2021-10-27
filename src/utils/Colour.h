#ifndef COLOUR_H
#define COLOUR_H

#include <cstdint>

struct Colour {
    uint8_t r, g, b, a;
    constexpr Colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r*a/255), g(g*a/255), b(b*a/255), a(a) {};

    static constexpr Colour black(uint8_t a = 255) { return Colour(0, 0, 0, a); }
    static constexpr Colour blue(uint8_t a = 255) { return Colour(0, 0, 255, a); }
    static constexpr Colour green(uint8_t a = 255) { return Colour(0, 255, 0, a); }
    static constexpr Colour cyan(uint8_t a = 255) { return Colour(0, 255, 255, a); }
    static constexpr Colour red(uint8_t a = 255) { return Colour(255, 0, 0, a); }
    static constexpr Colour magenta(uint8_t a = 255) { return Colour(255, 0, 255, a); }
    static constexpr Colour yellow(uint8_t a = 255) { return Colour(255, 255, 0, a); }
    static constexpr Colour white(uint8_t a = 255) { return Colour(255, 255, 255, a); }

    static constexpr Colour grey(uint8_t a = 255) { return Colour(128, 128, 128, a); }
};

#endif // COLOUR_H
