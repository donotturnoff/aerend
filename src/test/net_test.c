#include "libaerend.h"
#include <stdio.h>
#include <math.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5000);
    inet_pton(AF_INET, "169.254.1.1", &addr.sin_addr.s_addr);
    int err = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (err < 0) {
        printf("Failed to connect\n");
        return 1;
    }
    AeEvent buf[10];
    AeCtx ctx = ae_init(sock, buf, 10);

    AeId win_id, pnl_id, btn_id, lbl_id, cvs_id, pic_id, rect_id, ellp_id, line_id;

    {
        AeWindow win = (AeWindow) {
            .x = 100, .y = 100,
            .w = 600, .h = 400,
            .title = "lorem ipsum", .title_len = 11
        };

        AeStatusId s_wid = ae_make_window(&ctx, 0xFF, &win);
        win_id = s_wid.id;
        printf("[%d] %d\n", s_wid.status, win_id);
    }

    {
        AeButton btn = (AeButton) {
            .str = "click me", .str_len = 8,
            .colour = ae_colour_rgb(200, 200, 255),
            .bg_colour = ae_colour_rgb(100, 130, 160),
        };

        AeStatusId s_wid = ae_make_button(&ctx, 0x0D, &btn);
        btn_id = s_wid.id;
        printf("[%d] %d\n", s_wid.status, btn_id);
    }

    {
        AePanel pnl = (AePanel) {
            .layout = (AeLayout) {
                .type = AE_GRID_LAYOUT,
                .value = {.gl = {.cols = 2, .rows = 2}}
            },
            .bg_colour = ae_colour_rgb(50, 50, 50)
        };
        AeStatusId s_wid = ae_make_panel(&ctx, 0x03, &pnl);
        pnl_id = s_wid.id;
        printf("[%d] %d\n", s_wid.status, pnl_id);
    }

    {
        AeLabel lbl = (AeLabel) {
            .str = "lorem ipsum dolor sit amet, consectetuer adipiscing elit, non quam hacet", .str_len = 72,
            .colour = ae_colour_rgb(255, 0, 0),
            .bg_colour = ae_colour_rgba(0, 0, 0, 0)
        };

        AeStatusId s_wid = ae_make_label(&ctx, 0x0D, &lbl);
        lbl_id = s_wid.id;
        printf("[%d] %d\n", s_wid.status, lbl_id);
    }

    {
        AeCanvas cvs = {200, 200};
        AeStatusId s_wid = ae_make_canvas(&ctx, &cvs);
        cvs_id = s_wid.id;
        printf("[%d] %d\n", s_wid.status, cvs_id);
    }

    {
        AeColour pix[40000];
        for (unsigned int y = 0; y < 200; y++) {
            for (unsigned int x = 0; x < 200; x++) {
                uint8_t r = cos((double)y/200)*255;
                uint8_t g = cos((double)x/200)*255;
                uint8_t b = cos(((double)y*y+(double)x*x)/(200*200*2))*255;
                pix[y*200+x] = ae_colour_rgb(r, g, b);
            }
        }
        AePicture pic = {200, 200, (uint8_t *)pix};
        AeStatusId s_wid = ae_make_picture(&ctx, 0x01, &pic);
        pic_id = s_wid.id;
        printf("[%d] %d\n", s_wid.status, pic_id);
    }

    {
        AeRectangle rect = {
            .x = 20, .y = 30, .w = 40, .h = 50,
            .colour = ae_colour_rgb(255, 0, 0),
            .border = {.colour = ae_colour_rgb(0, 0, 255), .t = 5}
        };
        AeStatusId s_wid = ae_make_rectangle(&ctx, 0x01, &rect);
        rect_id = s_wid.id;
        printf("[%d] %d\n", s_wid.status, rect_id);
    }

    {
        AeEllipse ellp = {
            .x = 20, .y = 30, .w = 40, .h = 50,
            .colour = ae_colour_rgba(0, 255, 0, 100),
            .border = {.colour = ae_colour_rgba(0, 0, 255, 100), .t = 5}
        };
        AeStatusId s_wid = ae_make_ellipse(&ctx, 0x01, &ellp);
        ellp_id = s_wid.id;
        printf("[%d] %d\n", s_wid.status, ellp_id);
    }

    {
        AeLine line = {
            .x0 = 50, .y0 = 50, .x1 = 100, .y1 = 100,
            .colour = ae_colour_rgba(0, 255, 255, 200),
        };
        AeStatusId s_wid = ae_make_line(&ctx, &line);
        line_id = s_wid.id;
        printf("[%d] %d\n", s_wid.status, line_id);
    }

    AeEventHandler mouse_press_handler = (AeEventHandler){
        .type=AE_MOUSE_PRESS, .wid=win_id, .action_type=AE_EVENT_NOTIFY_CLIENT
    };
    AeEventHandler action_handler = (AeEventHandler){
        .type=AE_ACTION, .wid=btn_id, .action_type=AE_EVENT_NOTIFY_CLIENT
    };

    ae_add_event_handler(&ctx, &mouse_press_handler);
    ae_add_event_handler(&ctx, &action_handler);
    ae_add_widget(&ctx, win_id, pnl_id);
    ae_add_widget(&ctx, pnl_id, btn_id);
    ae_add_widget(&ctx, pnl_id, lbl_id);
    ae_add_widget(&ctx, pnl_id, cvs_id);
    ae_add_widget(&ctx, pnl_id, pic_id);
    ae_fill_canvas(&ctx, cvs_id, ae_colour_rgba(0, 50, 50, 100));
    ae_draw_shape(&ctx, cvs_id, rect_id);
    ae_draw_shape(&ctx, cvs_id, ellp_id);
    ae_draw_shape(&ctx, cvs_id, line_id);
    ae_open_window(&ctx, win_id);

    while (!ctx.err) {
        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf("\n%s", asctime(timeinfo));
        ae_recv_event(&ctx);
        AeEvent *event = ae_peek_event(&ctx);
        if (event) {
            printf("type: %d   wid: %d\n", event->mp.type, event->mp.wid);
            ae_pop_event(&ctx);
        }
    }
    return 0;
}

