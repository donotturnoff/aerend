#include "libaerend.h"

extern size_t stack_base;

extern inline AeId2 bitmap_test_init(AeCtx ctx) {
    AePicture pic = (AePicture) {.w = 1000, .h = 1000};
    AeId win_id = ae_make_window(&ctx, 0, NULL).id;
    AeId pic_id = ae_make_picture(&ctx, 0, &pic).id;
    ae_open_window(&ctx, win_id);
    ae_add_widget(&ctx, win_id, pic_id);
    return (AeId2) {win_id, pic_id};
}

extern inline void bitmap_test_cleanup(AeCtx ctx, AeId win_id) {
    ae_destroy_widget(&ctx, win_id);
}

extern inline void bitmap_test(AeCtx ctx, AeId pic_id, int pix) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    stack_base = (size_t) &x;
#endif
    uint32_t arr[pix];
    ae_set_picture_data(&ctx, pic_id, arr, pix*4);
}

