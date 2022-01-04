#include "DRMConn.h"
#include "DRMException.h"
#include <algorithm>
#include <cassert>
#include <cerrno>
#include <iostream>
#include <cstring>
#include <memory>

namespace aerend {

DRMConn::DRMConn() : fd(-1), id(0), crtc(0), w(0), h(0), saved_crtc(nullptr), front_buf(0), bufs(std::array<DRMBitmap, 2>{}), refs(std::make_shared<int>(0)) {}

DRMConn::DRMConn(const int fd, const std::vector<std::shared_ptr<DRMConn>> conns, const drmModeRes* res, const drmModeConnector* conn) : fd(fd), id(conn->connector_id), front_buf(0), refs(std::make_shared<int>(0)) {
    assert(fd >= 0);

    if (conn->count_modes == 0) {
        throw DRMException{"no valid mode"};
    }

    mode = conn->modes[0];

    w = mode.hdisplay;
    h = mode.vdisplay;

    bufs[0] = DRMBitmap(fd, w, h);
    bufs[1] = DRMBitmap(fd, w, h);

    find_crtc(fd, conns, res, conn);

    saved_crtc = drmModeGetCrtc(fd, crtc);
    if (!saved_crtc) {
        throw DRMException{"cannot save current CRTC", errno};
    }

    uint32_t fb {bufs[0].get_fb()};
    if (drmModeSetCrtc(fd, crtc, fb, 0, 0, &id, 1, &mode) < 0) {
        throw DRMException{"cannot set CRTC for connector", errno};
    }

    drmModePlaneRes* plane_res = drmModeGetPlaneResources(fd);
    // TODO: ensure this is a suitable plane
    cursor_plane = plane_res->planes[1];
    drmModeFreePlaneResources(plane_res);

}

DRMConn::DRMConn(DRMConn& conn) : fd(conn.fd), mode(conn.mode), id(conn.id), crtc(conn.crtc), w(conn.w), h(conn.h), saved_crtc(conn.saved_crtc), front_buf(conn.front_buf), bufs(conn.bufs), refs(conn.refs) {}

DRMConn::DRMConn(DRMConn&& conn) noexcept : DRMConn() {
    swap(*this, conn);
}

DRMConn& DRMConn::operator=(DRMConn conn) {
    swap(*this, conn);
    return *this;
}

DRMConn::~DRMConn() noexcept {
    assert(saved_crtc);

    if (!refs.unique()) {
        return;
    }

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

bool DRMConn::use_crtc_if_free(const uint32_t try_crtc, const std::vector<std::shared_ptr<DRMConn>> conns, drmModeEncoder* enc) {
    for (auto conn = conns.begin(); conn != conns.end(); conn++) {
        if ((*conn)->crtc == try_crtc) {
            return false;
        }
    }

    drmModeFreeEncoder(enc);
    crtc = try_crtc;
    return true;
}

void DRMConn::find_crtc(const int fd, const std::vector<std::shared_ptr<DRMConn>> conns, const drmModeRes* res, const drmModeConnector* conn) {
    drmModeEncoder* enc {conn->encoder_id ? drmModeGetEncoder(fd, conn->encoder_id) : nullptr};

    if (enc) {
        if (enc->crtc_id) {
            if (use_crtc_if_free(enc->crtc_id, conns, enc)) {
                return;
            }
        }
        drmModeFreeEncoder(enc);
    }

    for (int32_t i = 0; i < conn->count_encoders; i++) {
        enc = drmModeGetEncoder(fd, conn->encoders[i]);
        if (!enc) {
            continue;
        }

        for (int32_t j = 0; j < res->count_crtcs; j++) {
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

DRMBitmap DRMConn::get_back_buf() noexcept {
    return bufs[front_buf^1];
}

void DRMConn::set_cursor(std::shared_ptr<Cursor> cursor, int32_t x, int32_t y) {
    int32_t cw = cursor->get_w();
    int32_t ch = cursor->get_h();
    int32_t off_x = std::min(cw, (int)w-x);
    int32_t off_y = std::min(ch, (int)h-y);
    x = std::min(x, (int)w-cw);
    y = std::min(y, (int)h-ch);
    uint32_t fb = cursor->get_bmp().get_fb();
    int err = drmModeSetPlane(fd, cursor_plane, crtc, fb, 0, x, y, cw, ch, off_x << 16, off_y << 16, cw << 16, ch << 16);
    if (err) {
        throw DRMException("cannot set cursor", -err);
    }
}

void DRMConn::repaint() {
    uint32_t fb {bufs[front_buf^1].get_fb()};
    if (drmModeSetCrtc(fd, crtc, fb, 0, 0, &id, 1, &mode) < 0) {
        throw DRMException{"cannot flip CRTC", errno};
    }
    front_buf ^= 1;
}

void DRMConn::clear() noexcept {
    bufs[front_buf^1].clear();
}

}
