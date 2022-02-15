#ifndef COLOUR_H
#define COLOUR_H

#include <cstdint>
#include <algorithm>

namespace aerend {

struct Colour {
    uint8_t r, g, b, a;
    constexpr Colour(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255) : r(r*a/255), g(g*a/255), b(b*a/255), a(a) {};
    constexpr Colour(uint32_t v) : r(v>>16), g(v>>8), b(v), a(v>>24) {};

    static constexpr Colour black(uint8_t a = 255) { return Colour(0, 0, 0, a); }
    static constexpr Colour blue(uint8_t a = 255) { return Colour(0, 0, 255, a); }
    static constexpr Colour green(uint8_t a = 255) { return Colour(0, 255, 0, a); }
    static constexpr Colour cyan(uint8_t a = 255) { return Colour(0, 255, 255, a); }
    static constexpr Colour red(uint8_t a = 255) { return Colour(255, 0, 0, a); }
    static constexpr Colour magenta(uint8_t a = 255) { return Colour(255, 0, 255, a); }
    static constexpr Colour yellow(uint8_t a = 255) { return Colour(255, 255, 0, a); }
    static constexpr Colour white(uint8_t a = 255) { return Colour(255, 255, 255, a); }

    static constexpr Colour grey(uint8_t a = 255) { return Colour(128, 128, 128, a); }
    static constexpr Colour clear() { return Colour(0, 0, 0, 0); }

    inline uint32_t to_int() const;

    inline Colour lighten(uint8_t shift) const;

    static inline uint32_t src_over(uint32_t dst_v, uint32_t src_v);
};

uint32_t Colour::to_int() const {
    return (a << 24) | (r << 16) | (g << 8) | b;
}

Colour Colour::lighten(uint8_t shift) const {
    uint8_t new_r = std::min(255, (int) (r + shift));
    uint8_t new_g = std::min(255, (int) (g + shift));
    uint8_t new_b = std::min(255, (int) (b + shift));
    return Colour{new_r, new_g, new_b, a};
}

uint32_t Colour::src_over(uint32_t dst_v, uint32_t src_v) {
    uint32_t src_a = src_v >> 24;
    uint32_t src_b = src_v & 0xFF;
    uint32_t src_g = (src_v >> 8) & 0xFF;
    uint32_t src_r = (src_v >> 16) & 0xFF;

    uint32_t dst_a = dst_v >> 24;
    uint32_t dst_b = dst_v & 0xFF;
    uint32_t dst_g = (dst_v >> 8) & 0xFF;
    uint32_t dst_r = (dst_v >> 16) & 0xFF;

    int32_t p = (255 - src_a);
    int32_t a = src_a + ((dst_a*p)>>8);
    int32_t r = src_r + ((dst_r*p)>>8);
    int32_t g = src_g + ((dst_g*p)>>8);
    int32_t b = src_b + ((dst_b*p)>>8);

    return (a << 24 | r << 16 | g << 8 | b);
}

}

#endif // COLOUR_H

