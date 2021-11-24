#include "DisplayServer.h"

namespace aerend {

DisplayServer::DisplayServer() : card(DRMCard{"/dev/dri/card0"}) {}

DisplayServer& DisplayServer::the() {
    static DisplayServer instance;
    return instance;
}

void DisplayServer::repaint() {
    for (const auto& conn: card.get_conns()) {
        conn->repaint();
    }
}

std::vector<DRMBitmap> DisplayServer::get_bmps() {
    std::vector<DRMBitmap> bmps;
    // TODO: this makes all displays the same: allow for different windows on different displays
    for (const auto& conn: card.get_conns()) {
        bmps.push_back(conn->get_back_buf());
    }
    return bmps;
}

SimpleBitmap& DisplayServer::get_bmp(Window* root) {
    return root->get_bmp();
}

}
