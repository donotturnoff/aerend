#ifndef DRM_CARD_H
#define DRM_CARD_H

#include "DRMConn.h"
#include <vector>
#include <memory>

namespace aerend {

class DRMCard {
public:
    DRMCard();
    DRMCard(const char* card_path);
    int get_fd() const noexcept;
    std::vector<std::shared_ptr<DRMConn>> get_conns() const noexcept;
private:
    void open_card(const char* card_path);
    int fd = -1;
    std::vector<std::shared_ptr<DRMConn>> conns;
};

}

#endif // DRM_CARD_H

