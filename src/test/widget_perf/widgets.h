#include "libaerend.h"

extern size_t stack_base;

extern inline AeId widget_test_init(AeCtx ctx) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    AeWindow win = (AeWindow) {
        .x = 100, .y = 100,
        .w = 800, .h = 600,
        .title = "Test window", .title_len = 11
    };
    AeId win_id = ae_make_window(&ctx, 0x1F, &win).id;
    ae_open_window(&ctx, win_id);
    return win_id;
}

extern inline void widget_test_cleanup(AeCtx ctx, AeId win_id) {
    ae_destroy_widget(&ctx, win_id);
}

extern inline void panel_widget_test(AeCtx ctx, AeId win_id) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    AePanel pnl = (AePanel) {
        .bg_colour = ae_colour_rgb(100, 130, 160),
        .border = (AeBorder) {.colour = ae_colour_rgb(0, 20, 60), .t = 5}
    };
    AeId pnl_id = ae_make_panel(&ctx, 0x0D, &pnl).id;
    ae_add_widget(&ctx, win_id, pnl_id);
}

extern inline void button_widget_test_1(AeCtx ctx, AeId win_id) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    AeId btn_id = ae_make_button(&ctx, 0x00, NULL).id;
    ae_add_widget(&ctx, win_id, btn_id);
}

extern inline void button_widget_test_2(AeCtx ctx, AeId win_id) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    AeButton btn = (AeButton) {
        .str = "Click me", .str_len = 8,
    };
    AeId btn_id = ae_make_button(&ctx, 0x01, &btn).id;
    ae_add_widget(&ctx, win_id, btn_id);
}

extern inline void button_widget_test_3(AeCtx ctx, AeId win_id) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    AeButton btn = (AeButton) {
        .str = "Click me", .str_len = 8,
        .colour = ae_colour_rgb(200, 200, 255),
        .bg_colour = ae_colour_rgb(100, 130, 160),
        .border = (AeBorder) {.colour = ae_colour_rgb(0, 20, 60), .t = 5}
    };
    AeId btn_id = ae_make_button(&ctx, 0x1D, &btn).id;
    ae_add_widget(&ctx, win_id, btn_id);
}

extern inline void button_widget_test_4(AeCtx ctx, AeId win_id) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    AeButton btn = (AeButton) {
        .str = "Click me", .str_len = 8,
        .font_path = "res/lmroman12-regular.otf", .font_path_len = 25,
        .font_size = 20,
        .colour = ae_colour_rgb(200, 200, 255),
        .bg_colour = ae_colour_rgb(100, 130, 160),
        .border = (AeBorder) {.colour = ae_colour_rgb(0, 20, 60), .t = 5},
        .margin = 5,
        .padding = 5,
        .wrap = 0
    };
    AeId btn_id = ae_make_button(&ctx, 0xFF, &btn).id;
    ae_add_widget(&ctx, win_id, btn_id);
}

extern inline void label_widget_test_1(AeCtx ctx, AeId win_id) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    AeId lbl_id = ae_make_label(&ctx, 0x00, NULL).id;
    ae_add_widget(&ctx, win_id, lbl_id);
}

extern inline void label_widget_test_2(AeCtx ctx, AeId win_id) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    AeLabel lbl = (AeLabel) {
        .str = "Lorem ipsum", .str_len = 11,
    };
    AeId lbl_id = ae_make_label(&ctx, 0x01, &lbl).id;
    ae_add_widget(&ctx, win_id, lbl_id);
}

extern inline void label_widget_test_3(AeCtx ctx, AeId win_id) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    AeLabel lbl = (AeLabel) {
        .str = "Lorem ipsum", .str_len = 11,
        .colour = ae_colour_rgb(200, 200, 255),
        .bg_colour = ae_colour_rgb(100, 130, 160),
        .border = (AeBorder) {.colour = ae_colour_rgb(0, 20, 60), .t = 5}
    };
    AeId lbl_id = ae_make_label(&ctx, 0x1D, &lbl).id;
    ae_add_widget(&ctx, win_id, lbl_id);
}

extern inline void label_widget_test_4(AeCtx ctx, AeId win_id) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    AeLabel lbl = (AeLabel) {
        .str = "Lorem ipsum", .str_len = 11,
        .font_path = "res/lmroman12-regular.otf", .font_path_len = 25,
        .font_size = 20,
        .colour = ae_colour_rgb(200, 200, 255),
        .bg_colour = ae_colour_rgb(100, 130, 160),
        .border = (AeBorder) {.colour = ae_colour_rgb(0, 20, 60), .t = 5},
        .margin = 5,
        .padding = 5,
        .wrap = 0
    };
    AeId lbl_id = ae_make_label(&ctx, 0xFF, &lbl).id;
    ae_add_widget(&ctx, win_id, lbl_id);
}

extern inline void label_widget_test_5(AeCtx ctx, AeId win_id) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    AeLabel lbl = (AeLabel) {
        .str = "Lorem ipsum", .str_len = 11,
        .font_path = "res/lmroman12-regular.otf", .font_path_len = 25,
        .font_size = 20,
        .colour = ae_colour_rgb(200, 200, 255),
        .bg_colour = ae_colour_rgba(0, 0, 0, 0),
        .margin = 5,
        .padding = 5,
        .wrap = 0
    };
    AeId lbl_id = ae_make_label(&ctx, 0xEF, &lbl).id;
    ae_add_widget(&ctx, win_id, lbl_id);
}

