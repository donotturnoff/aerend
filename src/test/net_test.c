#include "libaerend.h"
#include <stdio.h>
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
    AeStatusId s_wid1 = ae_make_window(&ctx, 0, NULL);
    printf("[%d] %d\n", s_wid1.status, s_wid1.id);
    AeEventHandler handler = (AeEventHandler){
        .type=AE_MOUSE_PRESS, .wid=s_wid1.id, .action_type=AE_EVENT_NOTIFY_CLIENT
    };
    AeStatus status;
    status = ae_add_event_handler(&ctx, &handler);
    printf("[%d]\n", status);
    status = ae_open_window(&ctx, s_wid1.id);
    printf("[%d]\n", status);
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

