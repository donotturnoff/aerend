#include "libaerend.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define EVENT_BUF_SIZE 10
#define BULB_ON_LBL_MSG "Lightbulb on"
#define BULB_ON_LBL_MSG_LEN 12
#define BULB_OFF_LBL_MSG "Lightbulb off"
#define BULB_OFF_LBL_MSG_LEN 13
#define BULB_ON_BTN_MSG "Turn off"
#define BULB_ON_BTN_MSG_LEN 8
#define BULB_OFF_BTN_MSG "Turn on"
#define BULB_OFF_BTN_MSG_LEN 7

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5000);
    inet_pton(AF_INET, "169.254.1.1", &addr.sin_addr.s_addr);
    int err = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (err < 0) {
        fprintf(stderr, "Failed to connect\n");
        return 1;
    }
    AeEvent buf[EVENT_BUF_SIZE];
    AeCtx ctx = ae_init(sock, buf, EVENT_BUF_SIZE);

    bool bulb_on = false;

    AeId win_id, pnl_id, lbl_id, btn_id;

    {
        AeWindow win = {
            .x = 100, .y = 100,
            .w = 200, .h = 200,
            .title = "Basic smart lightbulb", .title_len = 21
        };
        AeStatusId win_sid = ae_make_window(&ctx, 0xFF, &win);
        if (win_sid.status > 0) {
            fprintf(stderr, "Failed to create window: error %d\n", win_sid.status);
            return 1;
        }
        win_id = win_sid.id;
    }

    {
        AePanel pnl = {
            .layout = {
                .type = AE_GRID_LAYOUT,
                .value = {.gl = {.cols = 1, .rows = 2}}
            }
        };
        AeStatusId pnl_sid = ae_make_panel(&ctx, 0x01, &pnl);
        if (pnl_sid.status > 0) {
            fprintf(stderr, "Failed to create panel: error %d\n", pnl_sid.status);
            return 1;
        }
        pnl_id = pnl_sid.id;
    }

    {
        AeLabel lbl = {
            .str = bulb_on ? BULB_ON_LBL_MSG: BULB_OFF_LBL_MSG,
            .str_len = bulb_on ? BULB_ON_LBL_MSG_LEN: BULB_OFF_LBL_MSG_LEN,
        };
        AeStatusId lbl_sid = ae_make_label(&ctx, 0x01, &lbl);
        if (lbl_sid.status > 0) {
            fprintf(stderr, "Failed to create label: error %d\n", lbl_sid.status);
            return 1;
        }
        lbl_id = lbl_sid.id;
    }

    {
        AeButton btn = {
            .str = bulb_on ? BULB_ON_BTN_MSG: BULB_OFF_BTN_MSG,
            .str_len = bulb_on ? BULB_ON_BTN_MSG_LEN: BULB_OFF_BTN_MSG_LEN,
        };
        AeStatusId btn_sid = ae_make_button(&ctx, 0x01, &btn);
        if (btn_sid.status > 0) {
            fprintf(stderr, "Failed to create button: error %d\n", btn_sid.status);
            return 1;
        }
        btn_id = btn_sid.id;

        AeEventHandler toggle_handler = {
            .type = AE_ACTION, .wid = btn_id,
            .action_type = AE_EVENT_NOTIFY_CLIENT
        };
        AeStatus status = ae_add_event_handler(&ctx, &toggle_handler);
        if (status > 0) {
            fprintf(stderr, "Failed to add event handler: error %d\n", btn_sid.status);
            return 1;
        }
    }

    AeStatus status = ae_add_widget(&ctx, win_id, pnl_id);
    if (status > 0) {
        fprintf(stderr, "Failed to add panel to window: error %d\n", status);
        return 1;
    }
    status = ae_add_widget(&ctx, pnl_id, lbl_id);
    if (status > 0) {
        fprintf(stderr, "Failed to add label to panel: error %d\n", status);
        return 1;
    }
    status = ae_add_widget(&ctx, pnl_id, btn_id);
    if (status > 0) {
        fprintf(stderr, "Failed to add button to panel: error %d\n", status);
        return 1;
    }
    status = ae_open_window(&ctx, win_id);
    if (status > 0) {
        fprintf(stderr, "Failed to open window: error %d\n", status);
        return 1;
    }

    while (true) {
        AeEvent *event = ae_recv_event(&ctx);
        if (ctx.err) {
            fprintf(stderr, "Failed to receive event: error %d\n", ctx.err);
            break;
        }
        if (event) {
            bulb_on = !bulb_on;
            char *lbl_msg = bulb_on ? BULB_ON_LBL_MSG: BULB_OFF_LBL_MSG;
            char *btn_msg = bulb_on ? BULB_ON_BTN_MSG: BULB_OFF_BTN_MSG;
            uint16_t lbl_msg_len = bulb_on ? BULB_ON_LBL_MSG_LEN: BULB_OFF_LBL_MSG_LEN;
            uint16_t btn_msg_len = bulb_on ? BULB_ON_BTN_MSG_LEN: BULB_OFF_BTN_MSG_LEN;
            printf("%s\n", lbl_msg);
            status = ae_set_str(&ctx, lbl_id, lbl_msg, lbl_msg_len);
            status = ae_set_str(&ctx, btn_id, btn_msg, btn_msg_len);
            ae_pop_event(&ctx);
        }
    }

    return 0;
}

