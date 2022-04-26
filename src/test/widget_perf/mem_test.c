#define AE_STACK_DEBUG 1

#include "libaerend.h"
#include "widgets.h"
#include "primitives.h"
#include "bitmaps.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define WIDGET_TESTS_NUM 10
#define PRIMITIVE_TESTS_NUM 10
#define BITMAP_TESTS_NUM 7

size_t ae_stack_base = 0;

int main() {
    void (*widget_tests[WIDGET_TESTS_NUM]) (AeCtx ctx, AeId win_id);
    widget_tests[0] = panel_widget_test;
    widget_tests[1] = button_widget_test_1;
    widget_tests[2] = button_widget_test_2;
    widget_tests[3] = button_widget_test_3;
    widget_tests[4] = button_widget_test_4;
    widget_tests[5] = label_widget_test_1;
    widget_tests[6] = label_widget_test_2;
    widget_tests[7] = label_widget_test_3;
    widget_tests[8] = label_widget_test_4;
    widget_tests[9] = label_widget_test_5;

    void (*primitive_tests[PRIMITIVE_TESTS_NUM]) (AeCtx ctx, AeId cvs_id);
    primitive_tests[0] = panel_primitive_test;
    primitive_tests[1] = button_primitive_test_1;
    primitive_tests[2] = button_primitive_test_2;
    primitive_tests[3] = button_primitive_test_3;
    primitive_tests[4] = button_primitive_test_4;
    primitive_tests[5] = label_primitive_test_1;
    primitive_tests[6] = label_primitive_test_2;
    primitive_tests[7] = label_primitive_test_3;
    primitive_tests[8] = label_primitive_test_4;
    primitive_tests[9] = label_primitive_test_5;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5000);
    inet_pton(AF_INET, "169.254.1.1", &addr.sin_addr.s_addr);
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    AeCtx ctx = ae_init(sock, NULL, 0);

    for (int i = 0; i < WIDGET_TESTS_NUM; i++) {
        AeId win_id = widget_test_init(ctx);

        ae_track_stack(true);
        widget_tests[i](ctx, win_id);
        size_t cost = ae_stack_base-ae_max_stack;
        ae_track_stack(false);

        printf("widget %d %lu\n", i, cost);
        fflush(stdout);

        widget_test_cleanup(ctx, win_id);
    }

    for (int i = 0; i < PRIMITIVE_TESTS_NUM; i++) {
        AeId2 ids = primitive_test_init(ctx);

        ae_track_stack(true);
        primitive_tests[i](ctx, ids.snd);
        size_t cost = ae_stack_base-ae_max_stack;
        ae_track_stack(false);

        printf("primitive %d %lu\n", i, cost);
        fflush(stdout);

        primitive_test_cleanup(ctx, ids.fst);
    }

    int pix_num = 1;
    for (int i = 0; i < BITMAP_TESTS_NUM; i++) {
        AeId2 ids = bitmap_test_init(ctx);

        uint32_t pix[pix_num];// = (uint32_t *)malloc(pix_num*sizeof(uint32_t));
        ae_track_stack(true);
        bitmap_test_2(ctx, ids.snd, pix, pix_num);

        size_t cost = ae_stack_base-ae_max_stack;
        ae_track_stack(false);

        printf("bitmap %d %lu\n", i, cost);
        fflush(stdout);

        bitmap_test_cleanup(ctx, ids.fst);
        pix_num *= 10;
    }

    return 0;
}

