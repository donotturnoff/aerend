#include "DRMCard.h"
#include "DRMConn.h"
#include "DRMException.h"
#include <cerrno>
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

DRMCard::DRMCard(const char* card_path) {
    open_card(card_path);

    drmModeRes* res {drmModeGetResources(fd)};
    if (!res) {
        throw DRMException{"cannot fetch resources for card", errno};
    }

    for (int32_t i = 0; i < res->count_connectors; i++) {
        drmModeConnector* c {drmModeGetConnector(fd, res->connectors[i])};
        if (!c) {
            throw DRMException{"cannot fetch connector for card", errno};
        }

        if (c->connection != DRM_MODE_CONNECTED) {
            drmModeFreeConnector(c);
            continue;
        }

        try {
            conns.push_back(std::make_shared<DRMConn>(fd, conns, res, c));
        } catch (const DRMException& e) {
            throw DRMException{"cannot setup connector", e};
        }

        drmModeFreeConnector(c);
    }

    drmModeFreeResources(res);
}

void DRMCard::open_card(const char* card_path) {
    fd = open(card_path, O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        throw DRMException{"cannot open card", errno};
    }

    uint64_t has_dumb;
    if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
        close(fd);
        throw DRMException{"card does not support dumb buffers"};
    }
}

std::vector<std::shared_ptr<DRMConn>> DRMCard::get_conns() const noexcept {
    return conns;
}
