//#define AE_STACK_DEBUG 1

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

//#define PRECACHING 1

#define EVENT_BUF_SIZE 1

size_t ae_stack_base = 0;

AeId make_page2(AeCtx *ctx) {
    uint8_t cols = 1;
    uint8_t rows[2] = {3, 1};
    AePanel page = {
        .layout = {
            .type = AE_IRREGULAR_GRID_LAYOUT,
            .value = {.igl = {.col_count = 1, .row_count = 2, .cols=&cols, .rows=rows}}
        }
    };
    AeId page_id = ae_make_panel(ctx, 0x01, &page).id;
    AeButton cur_btn = {
        .str = "Current", .str_len = 7,
    };
    AeId btn_id = ae_make_button(ctx, 0x01, &cur_btn).id;
    AeEventHandler switch_page_handler = {
        .type = AE_ACTION, .wid = btn_id,
        .action_type = AE_EVENT_NOTIFY_CLIENT
    };
    ae_add_event_handler(ctx, &switch_page_handler);
    AeCanvas cvs = {.w = 600, .h = 225};
    AeId cvs_id = ae_make_canvas(ctx, &cvs).id;
    ae_add_widget(ctx, page_id, cvs_id);
    ae_add_widget(ctx, page_id, btn_id);
    AeLine line = {.x0 = 0, .y0 = 100, .x1 = 100, .y1 = 150, .colour=ae_colour_rgb(128, 0, 0)};
    AeId line_id = ae_make_line(ctx, &line).id;
    ae_draw_shape(ctx, cvs_id, line_id);
    line = (AeLine){.x0 = 100, .y0 = 150, .x1 = 200, .y1 = 50, .colour=ae_colour_rgb(128, 0, 0)};
    line_id = ae_make_line(ctx, &line).id;
    ae_draw_shape(ctx, cvs_id, line_id);
    line = (AeLine){.x0 = 200, .y0 = 50, .x1 = 300, .y1 = 80, .colour=ae_colour_rgb(128, 0, 0)};
    line_id = ae_make_line(ctx, &line).id;
    ae_draw_shape(ctx, cvs_id, line_id);
    line = (AeLine){.x0 = 300, .y0 = 80, .x1 = 400, .y1 = 120, .colour=ae_colour_rgb(128, 0, 0)};
    line_id = ae_make_line(ctx, &line).id;
    ae_draw_shape(ctx, cvs_id, line_id);
    line = (AeLine){.x0 = 400, .y0 = 120, .x1 = 500, .y1 = 130, .colour=ae_colour_rgb(128, 0, 0)};
    line_id = ae_make_line(ctx, &line).id;
    ae_draw_shape(ctx, cvs_id, line_id);
    line = (AeLine){.x0 = 500, .y0 = 130, .x1 = 600, .y1 = 130, .colour=ae_colour_rgb(128, 0, 0)};
    line_id = ae_make_line(ctx, &line).id;
    ae_draw_shape(ctx, cvs_id, line_id);
    return page_id;
}

int main() {
#ifndef AE_STACK_DEBUG
    struct perf_event_attr pe;
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_INSTRUCTIONS;
    pe.disabled = 1;
    //pe.exclude_kernel = 1;
    pe.exclude_hv = 1;
#endif

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

    AeId win_id, page_id[2], lbl_id, btn_id;

    {
        AeWindow win = {
            .x = 100, .y = 100,
            .w = 600, .h = 300,
            .title = "Temperature sensor", .title_len = 18
        };
        win_id = ae_make_window(&ctx, 0xFF, &win).id;
    }

    {
        uint8_t cols = 1;
        uint8_t rows[2] = {3, 1};
        AePanel page = {
            .layout = {
                .type = AE_IRREGULAR_GRID_LAYOUT,
                .value = {.igl = {.col_count = 1, .row_count = 2, .cols=&cols, .rows=rows}}
            }
        };
        page_id[0] = ae_make_panel(&ctx, 0x01, &page).id;
    }

    {
        AeLabel label = {
            .str = "0C", .str_len = 2,
        };
        lbl_id = ae_make_label(&ctx, 0x01, &label).id;
    }

    {
        AeButton graph_btn = {
            .str = "Graph", .str_len = 5,
        };
        btn_id = ae_make_button(&ctx, 0x01, &graph_btn).id;
        AeEventHandler switch_page_handler = {
            .type = AE_ACTION, .wid = btn_id,
            .action_type = AE_EVENT_NOTIFY_CLIENT
        };
        ae_add_event_handler(&ctx, &switch_page_handler);
    }

    ae_add_widget(&ctx, page_id[0], lbl_id);
    ae_add_widget(&ctx, page_id[0], btn_id);
    ae_add_widget(&ctx, win_id, page_id[0]);
    ae_open_window(&ctx, win_id);

#ifdef PRECACHING
    page_id[1] = make_page2(&ctx);
#endif


#ifndef AE_STACK_DEBUG
    int fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd < 0) {
        fprintf(stderr, "Failed to start event logging\n");
        return 1;
    }
#endif

    while (true) {
#ifndef AE_STACK_DEBUG
        ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
#else
        int x = 0;
        ae_stack_base = (size_t) &x;
        ae_track_stack(true);
#endif



        AeEvent *event = ae_recv_event(&ctx);
        if (ctx.err) break;
        if (event) {
            ae_pop_event(&ctx);
            if (event->a.wid == btn_id) { // Switching to graph
#ifndef PRECACHING
                page_id[1] = make_page2(&ctx);
#endif
                ae_rm_widget(&ctx, page_id[0]);
                ae_add_widget(&ctx, win_id, page_id[1]);
            } else {
                ae_rm_widget(&ctx, page_id[1]);
                ae_add_widget(&ctx, win_id, page_id[0]);
            }
        }



#ifndef AE_STACK_DEBUG
        ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
        long long int cost;
        read(fd, &cost, sizeof(long long int));
        printf("%lld\n", cost);
        fflush(stdout);
#else
        size_t cost = ae_stack_base-ae_max_stack;
        ae_track_stack(false);

        printf("%lu\n", cost);
        fflush(stdout);
#endif
    }

    return 0;
}

