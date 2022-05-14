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

void DRMBitmap::set_size(const int32_t w, const int32_t h) {
    if (w < 0) throw std::invalid_argument("DRMBitmap::set_size: width cannot be negative");
    if (w < 0) throw std::invalid_argument("DRMBitmap::set_size: height cannot be negative");

    this->w = w;
    this->h = h;

    /* Create dumb buffer */
    struct drm_mode_create_dumb create_buf;
    create_buf.width = w;
    create_buf.height = h;
    create_buf.bpp = 32;
    if (drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &create_buf) < 0) {
        throw DRMException("cannot create dumb buffer", errno);
    }

    handle = create_buf.handle;
    size = create_buf.size;
    stride = create_buf.pitch;

    uint32_t handles[4] = {handle, 0, 0, 0};
    uint32_t pitches[4] = {stride, 0, 0, 0};
    uint32_t offsets[4] = {0, 0, 0, 0};

    /* Add dumb buffer as framebuffer */
    struct drm_mode_destroy_dumb destroy_buf;
    destroy_buf.handle = handle;
    if (drmModeAddFB2(fd, w, h, DRM_FORMAT_ARGB8888, handles, pitches, offsets, &fb, 0) < 0) {
        int err = errno;
        if (drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy_buf) < 0) {
            throw DRMException("cannot create framebuffer (additionally failed to destroy dumb buffer during cleanup)", err);
        }
        throw DRMException("cannot create framebuffer", err);
    }

    /* Prepare dumb buffer for mapping */
    struct drm_mode_map_dumb map_buf;
    memset(&map_buf, 0, sizeof(struct drm_mode_map_dumb));
    map_buf.handle = handle;
    if (drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &map_buf) < 0) {
        int err = errno;
        if (drmModeRmFB(fd, fb) < 0) {
            throw DRMException("cannot prepare dumb buffer for mapping (additionally failed to destroy framebuffer during cleanup)", err);
        }
        if (drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy_buf) < 0) {
            throw DRMException("cannot prepare dumb buffer for mapping (additionally failed to destroy dumb buffer during cleanup)", err);
        }
        throw DRMException("cannot prepare dumb buffer for mapping", err);
    }

    /* Map dumb buffer into process address space */
    map = (uint32_t*) mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, map_buf.offset);
    if (map == MAP_FAILED) {
        int err = errno;
        if (drmModeRmFB(fd, fb) < 0) {
            throw DRMException("cannot map dumb buffer (additionally failed to destroy framebuffer during cleanup)", err);
        }
        if (drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy_buf) < 0) {
            throw DRMException("cannot map dumb buffer (additionally failed to destroy dumb buffer during cleanup)", err);
        }
        throw DRMException("cannot map dumb buffer", err);
    }
    memset(map, 0x00, size);
}

uint32_t DRMBitmap::get_fb() const noexcept {
    return fb;
}

}
