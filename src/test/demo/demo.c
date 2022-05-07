#include "libaerend.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define EVENT_BUF_SIZE 1

AeId id_or_die(AeStatusId sid) {
    if (sid.status > 0) {
        fprintf(stderr, "Failed to create widget %d\n", sid.id);
        exit(sid.status);
    } else {
        return sid.id;
    }
}

AeId make_lbls_pnl(AeCtx *ctx) {
    AePanel pnl = {
        .layout = {
            .type = AE_GRID_LAYOUT,
            .value = {.gl = {
                .cols = 3, .rows = 3,
            }}
        }
    };
    AeId lbls_pnl = id_or_die(ae_make_panel(ctx, 0x01, &pnl));
    for (int i = 0; i < 9; i++) {
        AeLabel lbl = {
            .str = "Test", .str_len = 4,
            .margin = 5,
        };
        AeId test_lbl = ae_make_label(ctx, 0x21, &lbl).id;
        ae_add_widget(ctx, lbls_pnl, test_lbl);
    }
    return lbls_pnl;
}

AeId make_cvs(AeCtx *ctx) {
    AeCanvas cvs = {.w = 300, .h = 300};
    AeId lines_cvs = id_or_die(ae_make_canvas(ctx, &cvs));
    ae_fill_canvas(ctx, lines_cvs, ae_colour_rgb(150, 150, 200));
    for (int i = 0; i < 300; i += 30) {
        AeLine line = {
            .x0 = i, .y0 = 0, .x1 = 300, .y1 = i,
            .colour = ae_colour_rgb(0, 0, 0)
        };
        AeId l = ae_make_line(ctx, &line).id;
        ae_draw_shape(ctx, lines_cvs, l);
    }
    return lines_cvs;
}

AeId make_pic(AeCtx *ctx) {
    AeColour data[300][300];
    for (int y = 0; y < 300; y++) {
        for (int x = 0; x < 300; x++) {
            data[y][x] = ae_colour_rgb(x%255, 0, y%255);
        }
    }
    AePicture pic = {.w = 300, .h = 300, .pix = (uint8_t *)&data};
    AeId colour_pic = id_or_die(ae_make_picture(ctx, 0x01, &pic));
    return colour_pic;
}

int main(int argc, const char *argv[]) {
    bool pc = false, ss = false;
    if (argc > 1) {
        char type = argv[1][0];
        pc = (type == '1');
        ss = (type == '2');
    }

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
    AeCtx ctx_ = ae_init(sock, buf, EVENT_BUF_SIZE);
    AeCtx *ctx = &ctx_;

    AeId main_win, main_pnl, btns_pnl, lbls_btn, lines_btn, pic_btn, close_btn,
         lbls_pnl, cvs, pic, cur_pnl = 0;

    {
        AeWindow win = {
            .x = 200, .y = 200,
            .w = 300, .h = 600,
            .title = "Demo", .title_len = 4,
        };
        main_win = id_or_die(ae_make_window(ctx, 0xFF, &win));

        AePanel pnl = {
            .layout = {
                .type = AE_GRID_LAYOUT,
                .value = {.gl = {
                    .cols = 1, .rows = 2,
                }}
            }
        };
        main_pnl = id_or_die(ae_make_panel(ctx, 0x01, &pnl));

        pnl = (AePanel){
            .layout = {
                .type = AE_GRID_LAYOUT,
                .value = {.gl = {
                    .cols = 2, .rows = 2,
                }}
            }
        };
        btns_pnl = id_or_die(ae_make_panel(ctx, 0x01, &pnl));

        AeButton btn = {
            .str = "Lots of labels", .str_len = 14,
            .colour = ae_colour_rgb(200, 0, 0),
        };
        lbls_btn = id_or_die(ae_make_button(ctx, 0x05, &btn));

        btn = (AeButton){
            .str = "Lots of lines", .str_len = 13,
            .colour = ae_colour_rgb(0, 200, 0),
        };
        lines_btn = id_or_die(ae_make_button(ctx, 0x05, &btn));

        btn = (AeButton){
            .str = "A picture", .str_len = 9,
            .colour = ae_colour_rgb(0, 0, 200),
        };
        pic_btn = id_or_die(ae_make_button(ctx, 0x05, &btn));

        btn = (AeButton){
            .str = "Close", .str_len = 5,
            .colour = ae_colour_rgb(200, 200, 200),
            .bg_colour = ae_colour_rgb(200, 0, 0),
        };
        close_btn = id_or_die(ae_make_button(ctx, 0x0D, &btn));

        if (ss) {
            AeEventHandler close_eh = {
                .type = AE_ACTION,
                .wid = close_btn,
                .action_type = AE_EVENT_CLOSE_WINDOW,
                .action = {.cw = {
                    .wid = main_win
                }}
            };
            ae_add_event_handler(ctx, &close_eh);
        }
        AeEventHandler close_eh = {
            .type = AE_ACTION,
            .wid = close_btn,
            .action_type = AE_EVENT_NOTIFY_CLIENT,
        };
        ae_add_event_handler(ctx, &close_eh);

        if (pc || ss) {
            lbls_pnl = make_lbls_pnl(ctx);
            cvs = make_cvs(ctx);
            pic = make_pic(ctx);
        }

        if (!ss) {
            AeEventHandler lbls_eh = {
                .type = AE_ACTION,
                .wid = lbls_btn,
                .action_type = AE_EVENT_NOTIFY_CLIENT,
            };
            ae_add_event_handler(ctx, &lbls_eh);
            AeEventHandler lines_eh = {
                .type = AE_ACTION,
                .wid = lines_btn,
                .action_type = AE_EVENT_NOTIFY_CLIENT,
            };
            ae_add_event_handler(ctx, &lines_eh);
            AeEventHandler pic_eh = {
                .type = AE_ACTION,
                .wid = pic_btn,
                .action_type = AE_EVENT_NOTIFY_CLIENT,
            };
            ae_add_event_handler(ctx, &pic_eh);
        } else {
            AeEventHandler rm_page_eh = {
                .type = AE_ACTION,
                .wid = lbls_btn,
                .action_type = AE_EVENT_RM_CHILD,
                .action = {.rc = {.p_wid = main_pnl, .c_index = 1}},
            };
            ae_add_event_handler(ctx, &rm_page_eh);
            AeEventHandler add_page_eh = {
                .type = AE_ACTION,
                .wid = lbls_btn,
                .action_type = AE_EVENT_ADD_WIDGET,
                .action = {.aw = {.p_wid = main_pnl, .c_wid = lbls_pnl}},
            };
            ae_add_event_handler(ctx, &add_page_eh);
            rm_page_eh = (AeEventHandler){
                .type = AE_ACTION,
                .wid = lines_btn,
                .action_type = AE_EVENT_RM_CHILD,
                .action = {.rc = {.p_wid = main_pnl, .c_index = 1}},
            };
            ae_add_event_handler(ctx, &rm_page_eh);
            add_page_eh = (AeEventHandler){
                .type = AE_ACTION,
                .wid = lines_btn,
                .action_type = AE_EVENT_ADD_WIDGET,
                .action = {.aw = {.p_wid = main_pnl, .c_wid = cvs}},
            };
            ae_add_event_handler(ctx, &add_page_eh);
            rm_page_eh = (AeEventHandler){
                .type = AE_ACTION,
                .wid = pic_btn,
                .action_type = AE_EVENT_RM_CHILD,
                .action = {.rc = {.p_wid = main_pnl, .c_index = 1}},
            };
            ae_add_event_handler(ctx, &rm_page_eh);
            add_page_eh = (AeEventHandler){
                .type = AE_ACTION,
                .wid = pic_btn,
                .action_type = AE_EVENT_ADD_WIDGET,
                .action = {.aw = {.p_wid = main_pnl, .c_wid = pic}},
            };
            ae_add_event_handler(ctx, &add_page_eh);
        }
    }

    ae_add_widget(ctx, btns_pnl, lbls_btn);
    ae_add_widget(ctx, btns_pnl, lines_btn);
    ae_add_widget(ctx, btns_pnl, pic_btn);
    ae_add_widget(ctx, btns_pnl, close_btn);
    ae_add_widget(ctx, main_pnl, btns_pnl);
    ae_add_widget(ctx, main_win, main_pnl);
    ae_open_window(ctx, main_win);

    while (true) {
        AeEvent *event = ae_recv_event(ctx);
        if (ctx->err) {
            fprintf(stderr, "Error in event receive\n");
            return ctx->err;
        }
        if (event) {
            AeId wid = event->a.wid;
            if (wid == close_btn) {
                break;
            } else if (wid == lbls_btn) {
                if (!pc) {
                    lbls_pnl = make_lbls_pnl(ctx);
                }
                if (cur_pnl) {
                    ae_rm_widget(ctx, cur_pnl);
                }
                ae_add_widget(ctx, main_pnl, lbls_pnl);
                cur_pnl = lbls_pnl;
            } else if (wid == lines_btn) {
                if (!pc) {
                    cvs = make_cvs(ctx);
                }
                if (cur_pnl) {
                    ae_rm_widget(ctx, cur_pnl);
                }
                ae_add_widget(ctx, main_pnl, cvs);
                cur_pnl = cvs;
            } else if (wid == pic_btn) {
                if (!pc) {
                    pic = make_pic(ctx);
                }
                if (cur_pnl) {
                    ae_rm_widget(ctx, cur_pnl);
                }
                ae_add_widget(ctx, main_pnl, pic);
                cur_pnl = pic;
            }

            ae_pop_event(ctx);
        }
    }
    return 0;
}

