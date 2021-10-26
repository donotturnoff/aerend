#ifndef DRM_BITMAP_H
#define DRM_BITMAP_H

#include "bitmap/Bitmap.h"
#include <cstdint>

class DRMBitmap : public Bitmap {
public:
    DRMBitmap(const int fd, const int32_t w, const int32_t h);
    virtual ~DRMBitmap();
    void set_size(const int32_t w, const int32_t h);
    uint32_t get_fb() const noexcept;
protected:
    void over_blend(const uint32_t* src_map, const int32_t src_map_w, const int32_t x, const int32_t y, const int32_t src_x, const int32_t src_y, const int32_t src_w, const int32_t src_h) noexcept;
private:
    const int fd;
	uint32_t handle, fb, stride;
};

#endif // DRM_BITMAP_H
