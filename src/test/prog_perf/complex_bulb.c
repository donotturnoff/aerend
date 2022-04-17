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
#include <stdlib.h>
#include <math.h>

//#define PRECACHING 1
//#define SERVER_SIDE 1
#define DEFAULT_PAGE 2

#define EVENT_BUF_SIZE 10
#define BULB_ON_LBL_MSG "Lightbulb on"
#define BULB_ON_LBL_MSG_LEN 12
#define BULB_OFF_LBL_MSG "Lightbulb off"
#define BULB_OFF_LBL_MSG_LEN 13
#define BULB_ON_BTN_MSG "Turn off"
#define BULB_ON_BTN_MSG_LEN 8
#define BULB_OFF_BTN_MSG "Turn on"
#define BULB_OFF_BTN_MSG_LEN 7

#define WHEEL_W 300
#define WHEEL_H 225
AeColour wheel_pix[WHEEL_H][WHEEL_W];

AeId make_win(AeCtx *ctx) {
    AeWindow win = {
        .x = 100, .y = 100,
        .w = 300, .h = 300,
        .title = "Advanced smart lightbulb", .title_len = 24
    };
    return ae_make_window(ctx, 0xFF, &win).id;
}

AeId make_page(AeCtx *ctx) {
    uint8_t cols = 1;
    uint8_t rows[2] = {3, 1};
    AePanel page = {
        .layout = {
            .type = AE_IRREGULAR_GRID_LAYOUT,
            .value = {.igl = {.col_count = 1, .row_count = 2, .cols=&cols, .rows=rows}}
        }
    };
    return ae_make_panel(ctx, 0x01, &page).id;
}

void make_page1_panels(AeCtx *ctx, AeId pnl_ids[2]) {
    // Top
    AePanel panel = {
        .layout = {
            .type = AE_GRID_LAYOUT,
            .value = {.gl = {.cols = 3, .rows = 3}}
        }
    };
    pnl_ids[0] = ae_make_panel(ctx, 0x01, &panel).id;

    // Bottom
    panel = (AePanel) {
        .layout = {
            .type = AE_GRID_LAYOUT,
            .value = {.gl = {.cols = 1, .rows = 1}}
        }
    };
    pnl_ids[1] = ae_make_panel(ctx, 0x01, &panel).id;
}

void add_page_panels(AeCtx *ctx, AeId page_id, AeId pnl_ids[2]) {
    ae_add_widget(ctx, page_id, pnl_ids[0]);
    ae_add_widget(ctx, page_id, pnl_ids[1]);
}

void make_page1_buttons(AeCtx *ctx, AeId btn_ids[10], AeId win_id, AeId page1_id, AeId page2_id) {
    const AeColour btn_colours[9] = {
        ae_colour_rgb(0, 0, 0),
        ae_colour_rgb(127, 127, 127),
        ae_colour_rgb(255, 255, 255),
        ae_colour_rgb(255, 0, 0),
        ae_colour_rgb(0, 255, 0),
        ae_colour_rgb(0, 0, 255),
        ae_colour_rgb(255, 0, 255),
        ae_colour_rgb(255, 255, 0),
        ae_colour_rgb(0, 255, 255),
    };
    for (int i = 0; i < 9; i++) {
        AeButton button = {
            .str = "    ", .str_len = 4,
            .bg_colour = btn_colours[i],
            .border = {.colour = ae_colour_rgb(0,0,0), .t = 2}
        };
        btn_ids[i] = ae_make_button(ctx, 0x19, &button).id;
        AeEventHandler colour_btn_handler = {
            .type = AE_ACTION, .wid = btn_ids[i],
            .action_type = AE_EVENT_NOTIFY_CLIENT
        };
        ae_add_event_handler(ctx, &colour_btn_handler);
    }
    AeButton next_btn = {
        .str = "Advanced", .str_len = 8
    };
    btn_ids[9] = ae_make_button(ctx, 0x01, &next_btn).id;
#ifndef SERVER_SIDE
    AeEventHandler next_page_handler = {
        .type = AE_ACTION, .wid = btn_ids[9],
        .action_type = AE_EVENT_NOTIFY_CLIENT
    };
    ae_add_event_handler(ctx, &next_page_handler);
#else
    AeEventHandler rm_page_handler = {
        .type = AE_ACTION, .wid = btn_ids[9],
        .action_type = AE_EVENT_RM_WIDGET,
        .action = {.rw = {
            .wid = page1_id
        }}
    };
    ae_add_event_handler(ctx, &rm_page_handler);
    AeEventHandler add_page_handler = {
        .type = AE_ACTION, .wid = btn_ids[9],
        .action_type = AE_EVENT_ADD_WIDGET,
        .action = {.aw = {
            .p_wid = win_id,
            .c_wid = page2_id
        }}
    };
    ae_add_event_handler(ctx, &add_page_handler);
#endif // SERVER_SIDE
}

void add_page1_buttons(AeCtx *ctx, AeId pnl_ids[2], AeId btn_ids[10]) {
    for (int i = 0; i < 9; i++) {
        ae_add_widget(ctx, pnl_ids[0], btn_ids[i]);
    }
    ae_add_widget(ctx, pnl_ids[1], btn_ids[9]);
}

AeId make_page2_button(AeCtx *ctx, AeId win_id, AeId page1_id, AeId page2_id) {
    AeButton prev_btn = {
        .str = "Basic", .str_len = 5
    };
    AeStatusId sid = ae_make_button(ctx, 0x01, &prev_btn);
#ifndef SERVER_SIDE
    AeEventHandler prev_page_handler = {
        .type = AE_ACTION, .wid = sid.id,
        .action_type = AE_EVENT_NOTIFY_CLIENT
    };
    ae_add_event_handler(ctx, &prev_page_handler);
#else
    AeEventHandler rm_page_handler = {
        .type = AE_ACTION, .wid = sid.id,
        .action_type = AE_EVENT_RM_WIDGET,
        .action = {.rw = {
            .wid = page2_id
        }}
    };
    ae_add_event_handler(ctx, &rm_page_handler);
    AeEventHandler add_page_handler = {
        .type = AE_ACTION, .wid = sid.id,
        .action_type = AE_EVENT_ADD_WIDGET,
        asdfasdf
        .action = {.aw = {
            .p_wid = win_id,
            .c_wid = page1_id
        }}
    };
    ae_add_event_handler(ctx, &add_page_handler);
#endif // SERVER_SIDE
    return sid.id;
}

AeId make_page2_wheel(AeCtx *ctx) {
    AePicture pic = {
        .w = WHEEL_W, .h = WHEEL_H,
        .pix = (uint8_t *)wheel_pix
    };
    AeStatusId sid = ae_make_picture(ctx, 0x01, &pic);
    AeEventHandler wheel_handler = {
        .type = AE_MOUSE_CLICK, .wid = sid.id,
        .action_type = AE_EVENT_NOTIFY_CLIENT
    };
    ae_add_event_handler(ctx, &wheel_handler);
    return sid.id;
}

int main() {
    for (int y = 0; y < WHEEL_H; y++) {
        for (int x = 0; x < WHEEL_W; x++) {
            int red = fmax(255*(WHEEL_W-x)/WHEEL_W, 0);
            int green = fmax(255*(WHEEL_H-y)/WHEEL_H, 0);
            int blue = fmax(255*x/WHEEL_W, 0);
            wheel_pix[y][x] = ae_colour_rgb(red, green, blue);
        }
    }

    struct perf_event_attr pe;
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_INSTRUCTIONS;
    pe.disabled = 1;
    //pe.exclude_kernel = 1;
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

    AeId win_id, page_ids[2], pnl_ids[2], btn_ids1[10], btn_id2, whl_id;

    win_id = make_win(&ctx);

#if DEFAULT_PAGE == 1
    page_ids[0] = make_page(&ctx);
#ifdef PRECACHING
    page_ids[1] = make_page(&ctx);
#endif // PRECACHING
    make_page1_panels(&ctx, pnl_ids);
    make_page1_buttons(&ctx, btn_ids1, win_id, page_ids[0], page_ids[1]);

    ae_add_widget(&ctx, win_id, page_ids[0]);
    add_page_panels(&ctx, page_ids[0], pnl_ids);
    add_page1_buttons(&ctx, pnl_ids, btn_ids1);
#ifdef PRECACHING
    btn_id2 = make_page2_button(&ctx, win_id, page_ids[0], page_ids[1]);
    whl_id = make_page2_wheel(&ctx);
    ae_add_widget(&ctx, page_ids[1], whl_id);
    ae_add_widget(&ctx, page_ids[1], btn_id2);
#endif // PRECACHING
#else
    page_ids[1] = make_page(&ctx);
#ifdef PRECACHING
    page_ids[0] = make_page(&ctx);
#endif // PRECACHING
    btn_id2 = make_page2_button(&ctx, win_id, page_ids[0], page_ids[1]);
    whl_id = make_page2_wheel(&ctx);
    ae_add_widget(&ctx, page_ids[1], whl_id);
    ae_add_widget(&ctx, page_ids[1], btn_id2);
    ae_add_widget(&ctx, win_id, page_ids[1]);
#ifdef PRECACHING
    make_page1_panels(&ctx, pnl_ids);
    make_page1_buttons(&ctx, btn_ids1, win_id, page_ids[0], page_ids[1]);

    add_page_panels(&ctx, page_ids[0], pnl_ids);
    add_page1_buttons(&ctx, pnl_ids, btn_ids1);
#endif // PRECACHING
#endif // DEFAULT_PAGE == 1

    ae_open_window(&ctx, win_id);

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
            break;
        }
        if (event) {
            AeEventType type = event->h.type;
            AeId wid = (type == AE_ACTION) ? event->a.wid : event->mc.wid;
#ifndef SERVER_SIDE
            if (wid == btn_ids1[9]) { // "Advanced" button
#ifndef PRECACHING
#if DEFAULT_PAGE == 1
                page_ids[1] = make_page(&ctx);
                btn_id2 = make_page2_button(&ctx, win_id, page_ids[0], page_ids[1]);
                whl_id = make_page2_wheel(&ctx);
                ae_add_widget(&ctx, page_ids[1], whl_id);
                ae_add_widget(&ctx, page_ids[1], btn_id2);
#endif // DEFAULT_PAGE == 1
#endif // PRECACHING
                ae_rm_widget(&ctx, page_ids[0]);
                ae_add_widget(&ctx, win_id, page_ids[1]);
            } else if (wid == btn_id2) { // "Basic" button
#if DEFAULT_PAGE == 2
                page_ids[0] = make_page(&ctx);
                make_page1_panels(&ctx, pnl_ids);
                make_page1_buttons(&ctx, btn_ids1, win_id, page_ids[0], page_ids[1]);
                add_page_panels(&ctx, page_ids[0], pnl_ids);
                add_page1_buttons(&ctx, pnl_ids, btn_ids1);
#endif // DEFAULT_PAGE == 2
                ae_rm_widget(&ctx, page_ids[1]);
                ae_add_widget(&ctx, win_id, page_ids[0]);
            } else
#endif // SERVER_SIDE
            if (wid == whl_id) {
                printf("Selected colour from wheel\n");
            } else if (wid >= btn_ids1[0] && wid <= btn_ids1[8]) {
                printf("Selected colour %d\n", wid-btn_ids1[0]);
            }
            ae_pop_event(&ctx);
        }
        ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
        long long int cost;
        read(fd, &cost, sizeof(long long int));
        printf("complex_bulb %lld\n", cost);
        fflush(stdout);
    }

    return 0;
}

