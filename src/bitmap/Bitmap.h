#ifndef BITMAP_H
#define BITMAP_H

#include "utils/Colour.h"
#include <cstdint>

enum BlendMode {
    OPAQUE_BLEND, OVER_BLEND
};

class Bitmap {
public:
    virtual ~Bitmap() = default;

    void set_w(const int32_t w);
    void set_h(const int32_t h);
    virtual void set_size(const int32_t w, const int32_t h) = 0;

    uint8_t* get_map() const noexcept;
    int32_t get_w() const noexcept;
    int32_t get_h() const noexcept;
    int32_t get_size() const noexcept;

    void set_pixel(const int32_t x, const int32_t y, const Colour c) noexcept;
    Colour get_pixel(const int32_t x, const int32_t y) const noexcept;

    void clear() noexcept;
    void fill(const Colour c) noexcept;

    void composite(const Bitmap& bmp, const int32_t x, const int32_t y) noexcept;
    void composite(const Bitmap& bmp, const int32_t x, const int32_t y, const BlendMode mode) noexcept;
    void composite(const Bitmap& bmp, const int32_t x, const int32_t y, const int32_t src_x, const int32_t src_y, const int32_t src_w, const int32_t src_h, const BlendMode mode) noexcept;
    void composite(const uint8_t* src_map, const int32_t src_map_w, const int32_t src_map_h, const int32_t x, const int32_t y, const int32_t src_x, const int32_t src_y, const int32_t src_w, const int32_t src_h, const BlendMode mode) noexcept;
protected:
    int32_t w, h, size;
    uint8_t* map;
    virtual void opaque_blend(const uint8_t* src_map, const int32_t src_map_w, const int32_t x, const int32_t y, const int32_t src_x, const int32_t src_y, const int32_t src_w, const int32_t src_h) noexcept;
    virtual void over_blend(const uint8_t* src_map, const int32_t src_map_w, const int32_t x, const int32_t y, const int32_t src_x, const int32_t src_y, const int32_t src_w, const int32_t src_h) noexcept;
};

#endif // BITMAP_H
