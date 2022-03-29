#include "libaerend.h"
#include "widgets.h"
#include "primitives.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define WIDGET_TESTS_NUM 10
#define PRIMITIVE_TESTS_NUM 10
#define TEST_ITERS 100

#define AE_STACK_DEBUG 1

size_t stack_base = 0;

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
    inet_pton(AF_INET, "192.168.0.129", &addr.sin_addr.s_addr);
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    AeCtx ctx = ae_init(sock, NULL, 0);

    for (int i = 0; i < WIDGET_TESTS_NUM; i++) {
        for (int j = 0; j < TEST_ITERS; j++) {
            AeId win_id = widget_test_init(ctx);
            size_t fixed_cost = stack_base-ae_max_stack;

            widget_tests[i](ctx, win_id);
            size_t cost = stack_base-ae_max_stack;

            printf("widget %d %lu %lu\n", i, fixed_cost, cost);

            primitive_test_cleanup(ctx, win_id);
        }
    }

    for (int i = 0; i < PRIMITIVE_TESTS_NUM; i++) {
        for (int j = 0; j < TEST_ITERS; j++) {
            AeId2 ids = primitive_test_init(ctx);
            size_t fixed_cost = stack_base-ae_max_stack;

            primitive_tests[i](ctx, ids.snd);
            size_t cost = stack_base-ae_max_stack;

            printf("primitive %d %lu %lu\n", i, fixed_cost, cost);

            primitive_test_cleanup(ctx, ids.fst);
        }
    }

    return 0;
}

