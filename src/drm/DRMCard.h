#ifndef DRM_CARD_H
#define DRM_CARD_H

#include "DRMConn.h"
#include <vector>
#include <memory>

class DRMCard {
public:
    DRMCard(const char* card_path);
    std::vector<std::shared_ptr<DRMConn>> get_conns() const noexcept;
private:
    void open_card(const char* card_path);
    int fd;
    std::vector<std::shared_ptr<DRMConn>> conns;
};

#endif // DRM_CARD_H
