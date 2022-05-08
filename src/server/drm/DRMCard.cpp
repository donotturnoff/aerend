#include "DRMCard.h"
#include "DRMConn.h"
#include "DRMException.h"
#include <cerrno>
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

namespace aerend {

DRMCard::DRMCard() {}

DRMCard::DRMCard(std::string card_path) {
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
            conns.emplace_back(fd, conns, res, c);
        } catch (const DRMException& e) {
            throw DRMException{"cannot setup connector", e};
        }

        drmModeFreeConnector(c);
    }

    if (drmSetClientCap(fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1) < 0) {
        throw DRMException{"cannot set DRM universal planes capability", errno};
    }

    drmModeFreeResources(res);
}

DRMCard::~DRMCard() {
    drmClose(fd);
}

void DRMCard::open_card(std::string card_path) {
    fd = open(card_path.c_str(), O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        throw DRMException{"cannot open card", errno};
    }

    uint64_t has_dumb;
    if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
        close(fd);
        throw DRMException{"card does not support dumb buffers"};
    }
}

int DRMCard::get_fd() const noexcept {
    return fd;
}

std::vector<DRMConn>& DRMCard::get_conns() noexcept {
    return conns;
}

}
