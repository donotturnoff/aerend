#include "libaerend.h"

extern size_t stack_base;

extern inline AeId2 primitive_test_init(AeCtx ctx) {
    AeCanvas cvs = (AeCanvas) {.w = 800, .h = 600};
    AeId win_id = ae_make_window(&ctx, 0, NULL).id;
    AeId cvs_id = ae_make_canvas(&ctx, &cvs).id;
    ae_open_window(&ctx, win_id);
    ae_add_widget(&ctx, win_id, cvs_id);
    return (AeId2) {win_id, cvs_id};
}

extern inline void primitive_test_cleanup(AeCtx ctx, AeId win_id) {
    ae_destroy_widget(&ctx, win_id);
}

extern inline void panel_primitive_test(AeCtx ctx, AeId cvs_id) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    AeRectangle rect = (AeRectangle) {
        .x = 0, .y = 0,
        .w = 800, .h = 600,
        .colour = ae_colour_rgb(100, 130, 160),
        .border = (AeBorder) {.colour = ae_colour_rgb(0, 20, 60), .t = 5}
    };
    AeId rect_id = ae_make_rectangle(&ctx, 0x01, &rect).id;
    ae_draw_shape(&ctx, cvs_id, rect_id);
}

extern inline void button_primitive_test_1(AeCtx ctx, AeId cvs_id) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    AeRectangle rect = (AeRectangle) {
        .x = 0, .y = 0,
        .w = 200, .h = 60,
        .colour = ae_colour_rgb(100, 130, 160),
        .border = (AeBorder) {.colour = ae_colour_rgb(0, 20, 60), .t = 5}
    };
    AeText text = (AeText) {
        .str = "Click me", .str_len = 8,
        .font_path = "res/lmsans12-regular.otf", .font_path_len = 24,
        .font_size = 16,
        .colour = ae_colour_rgb(0, 0, 0),
        .x = 5, .y = 5,
        .wrap = 0
    };
    AeId rect_id = ae_make_rectangle(&ctx, 0x01, &rect).id;
    AeId text_id = ae_make_text(&ctx, &text).id;
    ae_draw_shape(&ctx, cvs_id, rect_id);
    ae_draw_shape(&ctx, cvs_id, text_id);
}

extern inline void button_primitive_test_2(AeCtx ctx, AeId cvs_id) {
    button_primitive_test_1(ctx, cvs_id);
}

extern inline void button_primitive_test_3(AeCtx ctx, AeId cvs_id) {
    button_primitive_test_1(ctx, cvs_id);
}

extern inline void button_primitive_test_4(AeCtx ctx, AeId cvs_id) {
    button_primitive_test_1(ctx, cvs_id);
}

extern inline void label_primitive_test_1(AeCtx ctx, AeId cvs_id) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    AeRectangle rect = (AeRectangle) {
        .x = 0, .y = 0,
        .w = 200, .h = 60,
        .colour = ae_colour_rgb(100, 130, 160),
    };
    AeText text = (AeText) {
        .str = " ", .str_len = 1,
        .font_path = "res/lmsans12-regular.otf", .font_path_len = 24,
        .font_size = 16,
        .colour = ae_colour_rgb(0, 0, 0),
        .x = 5, .y = 5,
        .wrap = 0
    };
    AeId rect_id = ae_make_rectangle(&ctx, 0x00, &rect).id;
    AeId text_id = ae_make_text(&ctx, &text).id;
    ae_draw_shape(&ctx, cvs_id, rect_id);
    ae_draw_shape(&ctx, cvs_id, text_id);
}

extern inline void label_primitive_test_2(AeCtx ctx, AeId cvs_id) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    AeRectangle rect = (AeRectangle) {
        .x = 0, .y = 0,
        .w = 200, .h = 60,
        .colour = ae_colour_rgb(100, 130, 160),
    };
    AeText text = (AeText) {
        .str = "Lorem ipsum", .str_len = 11,
        .font_path = "res/lmsans12-regular.otf", .font_path_len = 24,
        .font_size = 16,
        .colour = ae_colour_rgb(0, 0, 0),
        .x = 5, .y = 5,
        .wrap = 0
    };
    AeId rect_id = ae_make_rectangle(&ctx, 0x00, &rect).id;
    AeId text_id = ae_make_text(&ctx, &text).id;
    ae_draw_shape(&ctx, cvs_id, rect_id);
    ae_draw_shape(&ctx, cvs_id, text_id);
}

extern inline void label_primitive_test_3(AeCtx ctx, AeId cvs_id) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    AeRectangle rect = (AeRectangle) {
        .x = 0, .y = 0,
        .w = 200, .h = 60,
        .colour = ae_colour_rgb(100, 130, 160),
        .border = (AeBorder) {.colour = ae_colour_rgb(0, 20, 60), .t = 5}
    };
    AeText text = (AeText) {
        .str = "Lorem ipsum", .str_len = 11,
        .font_path = "res/lmsans12-regular.otf", .font_path_len = 24,
        .font_size = 16,
        .colour = ae_colour_rgb(0, 0, 0),
        .x = 5, .y = 5,
        .wrap = 0
    };
    AeId rect_id = ae_make_rectangle(&ctx, 0x01, &rect).id;
    AeId text_id = ae_make_text(&ctx, &text).id;
    ae_draw_shape(&ctx, cvs_id, rect_id);
    ae_draw_shape(&ctx, cvs_id, text_id);
}

extern inline void label_primitive_test_4(AeCtx ctx, AeId cvs_id) {
    label_primitive_test_3(ctx, cvs_id);
}

extern inline void label_primitive_test_5(AeCtx ctx, AeId cvs_id) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    AeText text = (AeText) {
        .str = "Lorem ipsum", .str_len = 11,
        .font_path = "res/lmsans12-regular.otf", .font_path_len = 24,
        .font_size = 16,
        .colour = ae_colour_rgb(0, 0, 0),
        .x = 5, .y = 5,
        .wrap = 0
    };
    AeId text_id = ae_make_text(&ctx, &text).id;
    ae_draw_shape(&ctx, cvs_id, text_id);
}

