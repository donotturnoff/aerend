#include "libaerend.h"
#include "widgets.h"
#include "primitives.h"
#include "bitmaps.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>	//ifreq
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>

#define WIDGET_TESTS_NUM 10
#define PRIMITIVE_TESTS_NUM 10
#define BITMAP_TESTS_NUM 7
#define TEST_ITERS 20

int main(int argc, char *argv[]) {
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
        for (int j = 0; j < TEST_ITERS; j++) {
            struct timespec start, end;

            AeId win_id = widget_test_init(ctx);

            clock_gettime(CLOCK_MONOTONIC, &start);
            widget_tests[i](ctx, win_id);
            clock_gettime(CLOCK_MONOTONIC, &end);

            double time_taken;
            time_taken = (end.tv_sec - start.tv_sec) * 1e9;
            time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;

            printf("widget %d %lf\n", i, time_taken);
            fflush(stdout);

            widget_test_cleanup(ctx, win_id);
        }
    }

    for (int i = 0; i < PRIMITIVE_TESTS_NUM; i++) {
        for (int j = 0; j < TEST_ITERS; j++) {
            struct timespec start, end;

            AeId2 ids = primitive_test_init(ctx);

            clock_gettime(CLOCK_MONOTONIC, &start);
            primitive_tests[i](ctx, ids.snd);
            clock_gettime(CLOCK_MONOTONIC, &end);

            double time_taken;
            time_taken = (end.tv_sec - start.tv_sec) * 1e9;
            time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;

            printf("primitive %d %lf\n", i, time_taken);
            fflush(stdout);

            primitive_test_cleanup(ctx, ids.fst);
        }
    }

    int pix = 1;
    for (int i = 0; i < BITMAP_TESTS_NUM; i++) {
        for (int j = 0; j < TEST_ITERS; j++) {
            struct timespec start, end;

            AeId2 ids = bitmap_test_init(ctx);

            clock_gettime(CLOCK_MONOTONIC, &start);
            bitmap_test(ctx, ids.snd, pix);
            clock_gettime(CLOCK_MONOTONIC, &end);

            double time_taken;
            time_taken = (end.tv_sec - start.tv_sec) * 1e9;
            time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;

            printf("bitmap %d %lf\n", i, time_taken);
            fflush(stdout);

            bitmap_test_cleanup(ctx, ids.fst);
        }
        pix *= 10;
    }

    return 0;
}

