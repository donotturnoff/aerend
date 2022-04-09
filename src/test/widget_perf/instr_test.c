#include "libaerend.h"
#include "perf.h"
#include "widgets.h"
#include "primitives.h"
#include "bitmaps.h"
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
#define BITMAP_TESTS_NUM 7
#define TEST_ITERS 100

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
    inet_pton(AF_INET, "192.168.0.129", &addr.sin_addr.s_addr);
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    AeCtx ctx = ae_init(sock, NULL, 0);

    int fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd < 0) {
        fprintf(stderr, "Failed to start event logging\n");
        return 1;
    }

    for (int i = 0; i < WIDGET_TESTS_NUM; i++) {
        for (int j = 0; j < TEST_ITERS; j++) {
            ioctl(fd, PERF_EVENT_IOC_RESET, 0);
            ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

            AeId win_id = widget_test_init(ctx);

            ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
            long long int fixed_cost;
            read(fd, &fixed_cost, sizeof(long long int));

            ioctl(fd, PERF_EVENT_IOC_RESET, 0);
            ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

            widget_tests[i](ctx, win_id);

            ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
            long long int cost;
            read(fd, &cost, sizeof(long long int));

            printf("widget %d %lld %lld\n", i, fixed_cost, cost);

            widget_test_cleanup(ctx, win_id);
        }
    }

    for (int i = 0; i < PRIMITIVE_TESTS_NUM; i++) {
        for (int j = 0; j < TEST_ITERS; j++) {
            ioctl(fd, PERF_EVENT_IOC_RESET, 0);
            ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

            AeId2 ids = primitive_test_init(ctx);

            ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
            long long int fixed_cost;
            read(fd, &fixed_cost, sizeof(long long int));

            ioctl(fd, PERF_EVENT_IOC_RESET, 0);
            ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

            primitive_tests[i](ctx, ids.snd);

            ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
            long long int cost;
            read(fd, &cost, sizeof(long long int));

            printf("primitive %d %lld %lld\n", i, fixed_cost, cost);

            primitive_test_cleanup(ctx, ids.fst);
        }
    }

    int pix = 1;
    for (int i = 0; i < BITMAP_TESTS_NUM; i++) {
        for (int j = 0; j < TEST_ITERS; j++) {
            ioctl(fd, PERF_EVENT_IOC_RESET, 0);
            ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

            AeId2 ids = bitmap_test_init(ctx);

            ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
            long long int fixed_cost;
            read(fd, &fixed_cost, sizeof(long long int));

            ioctl(fd, PERF_EVENT_IOC_RESET, 0);
            ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

            bitmap_test(ctx, ids.snd, pix);

            ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
            long long int cost;
            read(fd, &cost, sizeof(long long int));

            printf("bitmap %d %lld %lld\n", i, fixed_cost, cost);

            bitmap_test_cleanup(ctx, ids.fst);
        }
        pix *= 10;
    }

    return 0;
}

