#include "DRMBitmap.h"
#include "DRMException.h"
#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <sys/mman.h>
#include <xf86drmMode.h>
#include <xf86drm.h>

DRMBitmap::DRMBitmap(const int fd, const int32_t w, const int32_t h) : fd(fd) {
    set_size(w, h);
}

DRMBitmap::~DRMBitmap() {
    struct drm_mode_destroy_dumb dreq;

    munmap(map, size);

    drmModeRmFB(fd, fb);

    memset(&dreq, 0, sizeof(dreq));
    dreq.handle = handle;
    drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
}

// TODO: clean up when altering size
void DRMBitmap::set_size(const int32_t w, const int32_t h) {
    assert(w >= 0);
    assert(h >= 0);

    this->w = w;
    this->h = h;

    struct drm_mode_create_dumb creq;
    struct drm_mode_map_dumb mreq;
    struct drm_mode_destroy_dumb dreq;

    creq.width = w;
    creq.height = h;
    creq.bpp = 32;
    if (drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq) < 0) {
        throw DRMException("cannot create dumb buffer", errno);
    }

    stride = creq.pitch;
    size = creq.size;
    handle = creq.handle;
    dreq.handle = handle;

    if (drmModeAddFB(fd, w, h, 24, 32, stride, handle, &fb) < 0) {
        int err = errno;
        if (drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq) < 0) {
            throw DRMException("cannot create framebuffer (additionally failed to destroy dumb buffer during cleanup)", err);
        }
        throw DRMException("cannot create framebuffer", err);
    }

    memset(&mreq, 0, sizeof(struct drm_mode_map_dumb));
    mreq.handle = handle;
    if (drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq) < 0) {
        int err = errno;
        if (drmModeRmFB(fd, fb) < 0) {
            throw DRMException("cannot prepare dumb buffer for mapping (additionally failed to destroy framebuffer during cleanup)", err);
        }
        if (drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq) < 0) {
            throw DRMException("cannot prepare dumb buffer for mapping (additionally failed to destroy dumb buffer during cleanup)", err);
        }
        throw DRMException("cannot prepare dumb buffer for mapping", err);
    }

    map = (uint8_t*) mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mreq.offset);
    if (map == MAP_FAILED) {
        int err = errno;
        if (drmModeRmFB(fd, fb) < 0) {
            throw DRMException("cannot map dumb buffer (additionally failed to destroy framebuffer during cleanup)", err);
        }
        if (drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq) < 0) {
            throw DRMException("cannot map dumb buffer (additionally failed to destroy dumb buffer during cleanup)", err);
        }
        throw DRMException("cannot map dumb buffer", err);
    }
    memset(map, 0, size);
}

uint32_t DRMBitmap::get_fb() const noexcept {
    return fb;
}

void DRMBitmap::over_blend(const uint8_t* src_map, const int32_t src_map_w, const int32_t x, const int32_t y, const int32_t src_x, const int32_t src_y, const int32_t src_w, const int32_t src_h) noexcept {
    int32_t dst_a = 255;
    for (int32_t i = 0; i < src_h; i++) {
        for (int32_t j = 0; j < src_w; j++) {
            int32_t src_off = ((src_y+i)*src_map_w + src_x + j) * 4;
            int32_t dst_off = ((y+i)*w + x + j) * 4;

            int32_t src_b = src_map[src_off];
            int32_t src_g = src_map[src_off+1];
            int32_t src_r = src_map[src_off+2];
            int32_t src_a = src_map[src_off+3];
            int32_t dst_b = map[dst_off];
            int32_t dst_g = map[dst_off+1];
            int32_t dst_r = map[dst_off+2];

            // TODO: investigate replacing /255 with >>8
            // TODO: investigate replacing individual calculations with one big calculation
            int32_t p = dst_a*(255 - src_a)/255;
            int32_t a = src_a + p;
            int32_t r = (src_r*src_a + dst_r*p)/255;
            int32_t g = (src_g*src_a + dst_g*p)/255;
            int32_t b = (src_b*src_a + dst_b*p)/255;

            map[dst_off] = b;
            map[dst_off+1] = g;
            map[dst_off+2] = r;
            map[dst_off+3] = a;
        }
    }
}
