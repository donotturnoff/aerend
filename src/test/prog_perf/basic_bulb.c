#include "libaerend.h"
#include "../perf.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

//#define SERVER_SIDE 1

#define EVENT_BUF_SIZE 1
#define BULB_ON_LBL_MSG "Lightbulb on"
#define BULB_ON_LBL_MSG_LEN 12
#define BULB_OFF_LBL_MSG "Lightbulb off"
#define BULB_OFF_LBL_MSG_LEN 13
#define BULB_ON_BTN_MSG "Turn off"
#define BULB_ON_BTN_MSG_LEN 8
#define BULB_OFF_BTN_MSG "Turn on"
#define BULB_OFF_BTN_MSG_LEN 7

int main() {
    struct perf_event_attr pe;
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_INSTRUCTIONS;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;

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
            fprintf(stderr, "Failed to add notify event handler: error %d\n", status);
            return 1;
        }
#ifdef SERVER_SIDE
        toggle_handler = (AeEventHandler) {
            .type = AE_ACTION, .wid = btn_id,
            .action_type = AE_EVENT_SET_STR,
            .action = {.ss = {
                .wid = lbl_id,
                .str = BULB_ON_LBL_MSG,
                .str_len = BULB_ON_LBL_MSG_LEN
            }}
        };
        status = ae_add_event_handler(&ctx, &toggle_handler);
        if (status > 0) {
            fprintf(stderr, "Failed to add label str event handler: error %d\n", status);
            return 1;
        }
        toggle_handler = (AeEventHandler) {
            .type = AE_ACTION, .wid = btn_id,
            .action_type = AE_EVENT_SET_STR,
            .action = {.ss = {
                .wid = btn_id,
                .str = BULB_ON_BTN_MSG,
                .str_len = BULB_ON_BTN_MSG_LEN
            }}
        };
        status = ae_add_event_handler(&ctx, &toggle_handler);
        if (status > 0) {
            fprintf(stderr, "Failed to add button str event handler: error %d\n", status);
            return 1;
        }
#endif // SERVER_SIDE
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

    int fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd < 0) {
        fprintf(stderr, "Failed to start event logging\n");
        return 1;
    }

    while (true) {
        ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
        AeEvent *event = ae_recv_event(&ctx);
        if (ctx.err) {
            fprintf(stderr, "Failed to receive event: error %d\n", ctx.err);
            break;
        }
        if (event) {
            bulb_on = !bulb_on;
#ifndef SERVER_SIDE
            char *lbl_msg = bulb_on ? BULB_ON_LBL_MSG: BULB_OFF_LBL_MSG;
            char *btn_msg = bulb_on ? BULB_ON_BTN_MSG: BULB_OFF_BTN_MSG;
            uint16_t lbl_msg_len = bulb_on ? BULB_ON_LBL_MSG_LEN: BULB_OFF_LBL_MSG_LEN;
            uint16_t btn_msg_len = bulb_on ? BULB_ON_BTN_MSG_LEN: BULB_OFF_BTN_MSG_LEN;
            status = ae_set_str(&ctx, lbl_id, lbl_msg, lbl_msg_len);
            status = ae_set_str(&ctx, btn_id, btn_msg, btn_msg_len);
#endif // SERVER_SIDE
            ae_pop_event(&ctx);
        }
        ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
        long long int cost;
        read(fd, &cost, sizeof(long long int));
        printf("basic_bulb %lld\n", cost);
        fflush(stdout);
    }

    return 0;
}

