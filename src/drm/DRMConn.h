#ifndef DRM_CONN_H
#define DRM_CONN_H

#include "bitmap/DRMBitmap.h"
#include "gui/Cursor.h"
#include <xf86drmMode.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <vector>

namespace aerend {

class DRMConn {
public:
    DRMConn();
    DRMConn(const int fd, const std::vector<std::shared_ptr<DRMConn>> conns, const drmModeRes* res, const drmModeConnector* conn);
    DRMConn(DRMConn& conn);
    DRMConn(DRMConn&& conn) noexcept;
    DRMConn& operator=(DRMConn conn);
    virtual ~DRMConn() noexcept;
    friend void swap(DRMConn& conn1, DRMConn& conn2) noexcept {
        using std::swap;
        swap(conn1.fd, conn2.fd);
        swap(conn1.mode, conn2.mode);
        swap(conn1.id, conn2.id);
        swap(conn1.crtc, conn2.crtc);
        swap(conn1.w, conn2.w);
        swap(conn1.h, conn2.h);
        swap(conn1.saved_crtc, conn2.saved_crtc);
        swap(conn1.front_buf, conn2.front_buf);
        swap(conn1.bufs, conn2.bufs);
        swap(conn1.refs, conn2.refs);
    }
    DRMBitmap get_back_buf() noexcept;
    void set_cursor(std::shared_ptr<Cursor> cursor, int32_t x, int32_t y);
    void repaint();
    void clear() noexcept;
private:
    bool use_crtc_if_free(const uint32_t try_crtc, const std::vector<std::shared_ptr<DRMConn>> conns, drmModeEncoder* enc);
    void find_crtc(const int fd, const std::vector<std::shared_ptr<DRMConn>> conns, const drmModeRes* res, const drmModeConnector* conn);
	int fd;
	drmModeModeInfo mode;
	uint32_t id, crtc, w, h, cursor_plane;
	drmModeCrtc* saved_crtc;
    uint8_t front_buf;
    std::array<DRMBitmap, 2> bufs;
    std::shared_ptr<int> refs;
};

}

#endif // DRM_CONN_H

