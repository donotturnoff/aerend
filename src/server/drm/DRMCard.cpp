#include "DRMCard.h"
#include "DRMConn.h"
#include "DRMException.h"
#include <cerrno>
#include <cstdint>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

namespace aerend {

DRMCard::DRMCard() {}

DRMCard::DRMCard(std::string card_path) {
    open_card(card_path);

    drmModeRes* res{drmModeGetResources(fd)};
    if (!res) {
        auto err{errno};
        close(fd);
        throw DRMException{"cannot fetch resources for card", err};
    }

    /* Iterate over available connectors and, for every connected one, create a DRMConn instance */
    for (int32_t i{0}; i < res->count_connectors; i++) {
        drmModeConnector* c{drmModeGetConnector(fd, res->connectors[i])};
        if (!c) {
            auto err{errno};
            close(fd);
            throw DRMException{"cannot fetch connector for card", err};
        }

        if (c->connection != DRM_MODE_CONNECTED) {
            drmModeFreeConnector(c);
            continue;
        }

        try {
            conns.emplace_back(fd, conns, res, c);
        } catch (const DRMException& e) {
            std::cerr << "DRMCard: failed to set up connector: " << e.what() << std::endl;
        }

        drmModeFreeConnector(c);
    }

    if (drmSetClientCap(fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1) < 0) {
        auto err{errno};
        close(fd);
        throw DRMException{"cannot set DRM universal planes capability", err};
    }

    drmModeFreeResources(res);
}

DRMCard::~DRMCard() {
    close(fd);
}

void DRMCard::open_card(std::string card_path) {
    fd = open(card_path.c_str(), O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        throw DRMException{"cannot open card", errno};
    }

    /* Check for dumb buffer support. These are basic general-purpose framebuffers, not hardware-specific */
    uint64_t dumb_bufs;
    if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &dumb_bufs) < 0 || !dumb_bufs) {
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
