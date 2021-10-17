#ifndef DRM_CONN_H
#define DRM_CONN_H

#include "DRMBitmap.h"
#include <cstdint>
#include <xf86drmMode.h>
#include <array>
#include <memory>
#include <vector>

class DRMConn {
public:
    DRMConn(int fd, std::vector<std::shared_ptr<DRMConn>> conns, drmModeRes* res, drmModeConnector* conn);
    virtual ~DRMConn();
    DRMBitmap& get_back_buf() noexcept;
    void repaint();
    void clear() noexcept;
private:
    bool use_crtc_if_free(uint32_t try_crtc, std::vector<std::shared_ptr<DRMConn>> conns, drmModeEncoder* enc);
    void find_crtc(int fd, std::vector<std::shared_ptr<DRMConn>> conns, drmModeRes* res, drmModeConnector* conn);
	int fd;
	drmModeModeInfo mode;
	uint32_t id, crtc, w, h;
	drmModeCrtc* saved_crtc;
    uint8_t front_buf;
    std::array<DRMBitmap, 2> bufs;
};

#endif // DRM_CONN_H
