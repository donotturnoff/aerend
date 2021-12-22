#ifndef DRM_BITMAP_H
#define DRM_BITMAP_H

#include "bitmap/Bitmap.h"
#include "bitmap/SimpleBitmap.h"
#include <cstdint>
#include <algorithm>
#include <memory>

namespace aerend {

class DRMBitmap : public Bitmap {
public:
    DRMBitmap();
    DRMBitmap(const int fd, const int32_t w, const int32_t h);
    DRMBitmap(DRMBitmap& bmp);
    DRMBitmap(DRMBitmap&& bmp) noexcept;
    DRMBitmap& operator=(DRMBitmap bmp);
    virtual ~DRMBitmap() noexcept;
    friend void swap(DRMBitmap& bmp1, DRMBitmap& bmp2) noexcept {
        using std::swap;
        swap(bmp1.size, bmp2.size);
        swap(bmp1.w, bmp2.w);
        swap(bmp1.h, bmp2.h);
        swap(bmp1.map, bmp2.map);
        swap(bmp1.fd, bmp2.fd);
        swap(bmp1.handle, bmp2.handle);
        swap(bmp1.fb, bmp2.fb);
        swap(bmp1.stride, bmp2.stride);
        swap(bmp1.refs, bmp2.refs);
    }
    void set_size(const int32_t w, const int32_t h);
    uint32_t get_fb() const noexcept;
    SimpleBitmap get_region(const int32_t x, const int32_t y, const int32_t w, const int32_t h) const noexcept;

protected:
    void src_over_blend(const uint32_t* src_map, const int32_t src_map_w, const int32_t x, const int32_t y, const int32_t src_x, const int32_t src_y, const int32_t src_w, const int32_t src_h) noexcept;
private:
    int fd;
	uint32_t handle, fb, stride;
    std::shared_ptr<int> refs;
};

}

#endif // DRM_BITMAP_H

