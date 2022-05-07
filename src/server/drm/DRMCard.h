#ifndef DRM_CARD_H
#define DRM_CARD_H

#include "DRMConn.h"
#include <vector>
#include <string>
#include <memory>

namespace aerend {

class DRMCard {
public:
    DRMCard();
    ~DRMCard();
    DRMCard(std::string card_path);
    int get_fd() const noexcept;
    std::vector<DRMConn>& get_conns() noexcept;
private:
    void open_card(std::string card_path);
    int fd = -1;
    std::vector<DRMConn> conns;
};

}

#endif // DRM_CARD_H

