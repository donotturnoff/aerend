#include "DRMConn.h"
#include "DRMException.h"
#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cstring>

// TODO: remove bufs from list
DRMConn::DRMConn(int fd, std::vector<std::shared_ptr<DRMConn>> conns, drmModeRes* res, drmModeConnector* conn) : fd(fd), id(conn->connector_id), front_buf(0), bufs({DRMBitmap(fd), DRMBitmap(fd)}) {
    assert(fd >= 0);

    if (conn->count_modes == 0) {
        throw DRMException("no valid mode");
    }

    memcpy(&mode, &conn->modes[0], sizeof(mode));

    w = mode.hdisplay;
    h = mode.vdisplay;

    bufs[0].set_size(w, h);
    bufs[1].set_size(w, h);

    find_crtc(fd, conns, res, conn);

    saved_crtc = drmModeGetCrtc(fd, crtc);
    if (!saved_crtc) {
        throw DRMException("cannot save current CRTC", errno);
    }

    uint32_t fb = bufs[0].get_fb();
    if (drmModeSetCrtc(fd, crtc, fb, 0, 0, &id, 1, &mode) < 0) {
        throw DRMException("cannot set CRTC for connector", errno);
    }
}

DRMConn::~DRMConn() {
    assert(saved_crtc);

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

bool DRMConn::use_crtc_if_free(uint32_t try_crtc, std::vector<std::shared_ptr<DRMConn>> conns, drmModeEncoder* enc) {
    for (auto conn = conns.begin(); conn != conns.end(); conn++) {
        if ((*conn)->crtc == try_crtc) {
            try_crtc = -1;
            break;
        }
    }

    if (try_crtc >= 0) {
        drmModeFreeEncoder(enc);
        crtc = try_crtc;
        return true;
    }
    return false;
}

void DRMConn::find_crtc(int fd, std::vector<std::shared_ptr<DRMConn>> conns, drmModeRes* res, drmModeConnector* conn) {
    drmModeEncoder *enc = conn->encoder_id ? drmModeGetEncoder(fd, conn->encoder_id) : nullptr;

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

    throw DRMException("cannot find suitable CRTC");
}

DRMBitmap& DRMConn::get_back_buf() noexcept {
    return bufs[front_buf ^ 1]; 
}

void DRMConn::repaint() {
    DRMBitmap& bb = bufs[front_buf ^ 1];
    if (drmModeSetCrtc(fd, crtc, bb.get_fb(), 0, 0, &id, 1, &mode) < 0) {
        throw DRMException("cannot flip CRTC", errno);
    }
    front_buf ^= 1;
}

void DRMConn::clear() noexcept {
    DRMBitmap& bb = bufs[front_buf ^ 1];
    bb.clear();
}
