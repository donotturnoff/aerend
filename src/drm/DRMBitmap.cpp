#include "DRMBitmap.h"
#include "DRMException.h"
#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <sys/mman.h>
#include <xf86drmMode.h>
#include <xf86drm.h>
#include <iostream>

DRMBitmap::DRMBitmap() : fd(-1), handle(0), fb(0), stride(0), refs(std::make_shared<int>(0)) {
    map = nullptr;
    w = 0;
    h = 0;
    size = 0;
}

DRMBitmap::DRMBitmap(const int fd, const int32_t w, const int32_t h) : fd(fd), refs(std::make_shared<int>(0)) {
    set_size(w, h);
}

DRMBitmap::DRMBitmap(DRMBitmap& bmp) : fd(bmp.fd), handle(bmp.handle), fb(bmp.fb), stride(bmp.stride), refs(bmp.refs) {
    w = bmp.w;
    h = bmp.h;
    map = bmp.map;
    size = bmp.size;
}

DRMBitmap::DRMBitmap(DRMBitmap&& bmp) noexcept : DRMBitmap() {
    swap(*this, bmp);
}

DRMBitmap& DRMBitmap::operator=(DRMBitmap bmp) {
    swap(*this, bmp);
    return *this;
}

DRMBitmap::~DRMBitmap() {
    if (!refs.unique()) {
        return;
    }

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
