#include "DRMBitmap.h"
#include "drm/DRMException.h"
#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <sys/mman.h>
#include <xf86drmMode.h>
#include <xf86drm.h>
#include <iostream>
#include <drm_fourcc.h>
#include <stdexcept>

namespace aerend {

DRMBitmap::DRMBitmap() : fd(-1), handle(0), fb(0), stride(0) {
    map = nullptr;
    w = 0;
    h = 0;
    size = 0;
}

DRMBitmap::DRMBitmap(const int fd, const int32_t w, const int32_t h) : fd(fd) {
    set_size(w, h);
}

DRMBitmap::DRMBitmap(DRMBitmap&& bmp) noexcept : DRMBitmap() {
    swap(*this, bmp);
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
    if (w < 0) throw std::invalid_argument("DRMBitmap::set_size: width cannot be negative");
    if (w < 0) throw std::invalid_argument("DRMBitmap::set_size: height cannot be negative");

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

    uint32_t handles[4] = {handle, 0, 0, 0};
    uint32_t pitches[4] = {stride, 0, 0, 0};
    uint32_t offsets[4] = {0, 0, 0, 0};

    if (drmModeAddFB2(fd, w, h, DRM_FORMAT_ARGB8888, handles, pitches, offsets, &fb, 0) < 0) {
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

    map = (uint32_t*) mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mreq.offset);
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

// TODO: rename to get_id?
uint32_t DRMBitmap::get_fb() const noexcept {
    return fb;
}

void DRMBitmap::src_over_blend(const uint32_t* src_map, const int32_t src_map_w, const int32_t x, const int32_t y, const int32_t src_x, const int32_t src_y, const int32_t src_w, const int32_t src_h) noexcept {
    for (int32_t i = 0; i < src_h; i++) {
        int32_t src_off_base = (src_y+i)*src_map_w + src_x;
        int32_t dst_off_base = (y+i)*w + x;
        for (int32_t j = 0; j < src_w; j++) {
            int32_t src_off = src_off_base + j;
            uint32_t src_v = src_map[src_off];
            if (src_v <= 0xFFFFFF) {
                continue;
            }

            int32_t dst_off = dst_off_base + j;
            if (src_v >= 0xFF000000) {
                map[dst_off] = src_v;
                continue;
            }

            uint32_t dst_v = map[dst_off];
            map[dst_off] = Colour::src_over(dst_v, src_v);
        }
    }
}

}
