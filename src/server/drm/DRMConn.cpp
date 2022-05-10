#include "DRMConn.h"
#include "DRMException.h"
#include <algorithm>
#include <cassert>
#include <cerrno>
#include <iostream>
#include <cstring>
#include <memory>

namespace aerend {

DRMConn::DRMConn() : fd(-1), id(0), crtc(0), w(0), h(0), saved_crtc(nullptr) {}

DRMConn::DRMConn(const int fd, const std::vector<DRMConn>& conns, const drmModeRes* res, const drmModeConnector* conn) : fd(fd), id(conn->connector_id) {
    assert(fd >= 0);

    if (conn->count_modes == 0) {
        throw DRMException{"no valid mode"};
    }

    mode = conn->modes[0];

    /* Display dimensions */
    w = mode.hdisplay;
    h = mode.vdisplay;

    /* Create double buffer */
    bufs[0] = std::make_unique<DRMBitmap>(fd, w, h);
    bufs[1] = std::make_unique<DRMBitmap>(fd, w, h);

    /* Save existing CRTC and set new one */
    find_crtc(fd, conns, res, conn);
    saved_crtc = drmModeGetCrtc(fd, crtc);
    if (!saved_crtc) {
        throw DRMException{"cannot save current CRTC", errno};
    }
    uint32_t fb{bufs[0]->get_fb()};
    int ret = drmModeSetCrtc(fd, crtc, fb, 0, 0, &id, 1, &mode);
    if (ret < 0) {
        if (ret == -1) {
            throw DRMException{"cannot set CRTC for connector: count is invalid or connectors are incompatible"};
        } else if (ret == -EINVAL) {
            throw DRMException{"cannot set CRTC for connector: CRTC ID is invalid"};
        }
        throw DRMException{"cannot set CRTC for connector", errno};
    }

    drmModePlaneRes* plane_res = drmModeGetPlaneResources(fd);
    if (!plane_res) {
        throw DRMException{"cannot get plane resources", errno};
    }
    cursor_plane = plane_res->planes[1];
    drmModeFreePlaneResources(plane_res);
}

DRMConn::DRMConn(DRMConn&& conn) noexcept : DRMConn() {
    swap(*this, conn);
}

DRMConn::~DRMConn() noexcept {
    assert(saved_crtc);

    /* Restore saved CRTC */
    drmModeSetCrtc(fd,
        saved_crtc->crtc_id,
        saved_crtc->buffer_id,
        saved_crtc->x,
        saved_crtc->y,
        &id,
        1,
        &saved_crtc->mode
    );

    drmModeFreeCrtc(saved_crtc);
}

bool DRMConn::use_crtc_if_free(const uint32_t try_crtc, const std::vector<DRMConn>& conns, drmModeEncoder* enc) {
    /* Check all other connectors to see if CRTC is in use */
    for (const auto& conn : conns) {
        if (conn.crtc == try_crtc) {
            return false;
        }
    }

    drmModeFreeEncoder(enc);
    crtc = try_crtc;
    return true;
}

void DRMConn::find_crtc(const int fd, const std::vector<DRMConn>& conns, const drmModeRes* res, const drmModeConnector* conn) {
    drmModeEncoder* enc {conn->encoder_id ? drmModeGetEncoder(fd, conn->encoder_id) : nullptr};

    /* Attempt to use the current CRTC */
    if (enc) {
        if (enc->crtc_id) {
            if (use_crtc_if_free(enc->crtc_id, conns, enc)) {
                return;
            }
        }
        drmModeFreeEncoder(enc);
    }

    /* Otherwise, check all other available CRTCs */
    for (int32_t i{0}; i < conn->count_encoders; i++) {
        enc = drmModeGetEncoder(fd, conn->encoders[i]);
        if (!enc) {
            continue;
        }

        for (int32_t j{0}; j < res->count_crtcs; j++) {
            if (!(enc->possible_crtcs & (1 << j))) {
                continue;
            }

            if (use_crtc_if_free(res->crtcs[j], conns, enc)) {
                return;
            }
        }
        drmModeFreeEncoder(enc);
    }

    throw DRMException{"cannot find suitable CRTC"};
}

void DRMConn::composite(Bitmap& bmp, int32_t x, int32_t y) {
    bufs[front_buf^1]->composite(bmp, x, y);
}

void DRMConn::set_cursor(Cursor* cursor, int32_t x, int32_t y) {
    int32_t cw {cursor->get_w()};
    int32_t ch {cursor->get_h()};

    /* Prevent cursor leaving screen */
    int32_t off_x {std::min(cw, (int)w-x)};
    int32_t off_y {std::min(ch, (int)h-y)};
    x = std::min(x, (int)w-cw);
    y = std::min(y, (int)h-ch);

    uint32_t fb {cursor->get_bmp().get_fb()};

    /* Update the cursor plane's framebuffer */
    int ret {drmModeSetPlane(fd, cursor_plane, crtc, fb, 0, x, y, cw, ch, off_x << 16, off_y << 16, cw << 16, ch << 16)};
    if (ret < 0) {
        if (ret == -EINVAL) {
            throw DRMException{"cannot set cursor: plane ID or CRTC id is invalid"};
        }
        throw DRMException{"cannot set cursor", errno};
    }
}

void DRMConn::repaint() {
    /* Update the CRTC's framebuffer to use the current back buffer */
    uint32_t fb {bufs[front_buf^1]->get_fb()};
    int ret {drmModeSetCrtc(fd, crtc, fb, 0, 0, &id, 1, &mode)};
    if (ret < 0) {
        if (ret == -1) {
            throw DRMException{"cannot flip CRTC: count is invalid or connectors are incompatible"};
        } else if (ret == -EINVAL) {
            throw DRMException{"cannot flip CRTC: CRTC ID is invalid"};
        }
        throw DRMException{"cannot flip CRTC", errno};
    }
    front_buf ^= 1;
}

void DRMConn::clear() noexcept {
    bufs[front_buf^1]->clear();
}

void DRMConn::fill(Colour colour) noexcept {
    bufs[front_buf^1]->fill(colour);
}

int32_t DRMConn::get_w() const noexcept {
    return w;
}

int32_t DRMConn::get_h() const noexcept {
    return h;
}

}
