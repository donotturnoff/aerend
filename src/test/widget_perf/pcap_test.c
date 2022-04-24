// https://www.devdungeon.com/content/using-libpcap-c

#include "libaerend.h"
#include "widgets.h"
#include "primitives.h"
#include "bitmaps.h"
#include <stdio.h>
#include <time.h>
#include <pcap.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <net/if.h>	//ifreq
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

pthread_mutex_t pcap_mtx;
pthread_cond_t pcap_cond;
int test_ready = 0;
uint8_t mac[6];

int own_mac(uint8_t *mac2) {
    int same = 1;
    for (int i = 0; i < 6; i++) {
        same &= mac[i] == mac2[i];
    }
    return same;
}

/* Finds the payload of a TCP/IP packet */
int handle_packet(const struct pcap_pkthdr *header, const u_char *packet, long int *sec, long int *usec) {
    /* First, lets make sure we have an IP packet */
    struct ether_header *eth_header;
    eth_header = (struct ether_header *) packet;
    if (ntohs(eth_header->ether_type) != ETHERTYPE_IP) {
        printf("Not an IP packet. Skipping...\n\n");
        return 0;
    }

    /* Pointers to start point of various headers */
    const u_char *ip_header;
    const u_char *tcp_header;
    const u_char *payload;

    /* Header lengths in bytes */
    int ethernet_header_length = 14; /* Doesn't change */
    int ip_header_length;
    int tcp_header_length;
    int payload_length;

    /* Find start of IP header */
    ip_header = packet + ethernet_header_length;
    /* The second-half of the first byte in ip_header
       contains the IP header length (IHL). */
    ip_header_length = ((*ip_header) & 0x0F);
    /* The IHL is number of 32-bit segments. Multiply
       by four to get a byte count for pointer arithmetic */
    ip_header_length = ip_header_length * 4;

    /* Now that we know where the IP header is, we can
       inspect the IP header for a protocol number to
       make sure it is TCP before going any further.
       Protocol is always the 10th byte of the IP header */
    u_char protocol = *(ip_header + 9);
    if (protocol != IPPROTO_TCP) {
        printf("Not a TCP packet. Skipping...\n\n");
        return 0;
    }

    /* Add the ethernet and ip header length to the start of the packet
       to find the beginning of the TCP header */
    tcp_header = packet + ethernet_header_length + ip_header_length;
    /* TCP header length is stored in the first half
       of the 12th byte in the TCP header. Because we only want
       the value of the top half of the byte, we have to shift it
       down to the bottom half otherwise it is using the most
       significant bits instead of the least significant bits */
    tcp_header_length = ((*(tcp_header + 12)) & 0xF0) >> 4;
    /* The TCP header length stored in those 4 bits represents
       how many 32-bit words there are in the header, just like
       the IP header length. We multiply by four again to get a
       byte count. */
    tcp_header_length = tcp_header_length * 4;

    /* Add up all the header sizes to find the payload offset */
    int total_headers_size = ethernet_header_length+ip_header_length+tcp_header_length;
    payload_length = header->caplen -
        (ethernet_header_length + ip_header_length + tcp_header_length);
    payload = packet + total_headers_size;

    *sec = header->ts.tv_sec;
    *usec = header->ts.tv_usec;
    int data = payload_length > 0;
    int outbound = own_mac(eth_header->ether_shost);
    /*if (data) {
        const u_char *temp_pointer = payload;
        int byte_count = 0;
        while (byte_count++ < payload_length) {
            printf("%02x %c\n", *temp_pointer, *temp_pointer);
            temp_pointer++;
        }
        printf("\n");
    }*/
    return data && !outbound;
}

struct pcap_test_args {
    long long int usec_out;
    int msgs;
};

void *pcap_test(void *test_args) {
    struct pcap_test_args *args = test_args;
    args->usec_out = 0;
    int msgs = args->msgs;

    char error_buffer[PCAP_ERRBUF_SIZE];
    struct bpf_program filter;
    pcap_t *handle = pcap_open_live("enp1s0", BUFSIZ, 1, 1000, error_buffer);
    if (handle == NULL) {
        printf("Could not open device - %s\n", error_buffer);
        return NULL;
    }
    if (pcap_compile(handle, &filter, "tcp port 5000", 0, PCAP_NETMASK_UNKNOWN) == -1) {
        printf("Bad filter - %s\n", pcap_geterr(handle));
        return NULL;
    }
    if (pcap_setfilter(handle, &filter) == -1) {
        printf("Error setting filter - %s\n", pcap_geterr(handle));
        return NULL;
    }

    pthread_mutex_lock(&pcap_mtx);
    test_ready = 1;
    pthread_cond_signal(&pcap_cond);
    pthread_mutex_unlock(&pcap_mtx);
    for (int i = 0; i < msgs; i++) {
        int done = 0;
        int first = 1;
        long int sec, usec, start_sec, start_usec;

        while (!done) {
            struct pcap_pkthdr packet_header;
            const u_char *packet;
            packet = pcap_next(handle, &packet_header);
            if (packet == NULL) {
                printf("No packet found.\n");
                return NULL;
            }

            done = handle_packet(&packet_header, packet, &sec, &usec);
            if (first) {
                start_sec = sec;
                start_usec = usec;
                first = 0;
            }
        }
        //printf("%ld.%ld -> %ld.%ld\n", start_sec, start_usec, sec, usec);
        args->usec_out += ((long long int)sec*1000000+usec)-((long long int)start_sec*1000000+start_usec);
    }
    pcap_freecode(&filter);
    pcap_close(handle);
    return NULL;
}

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

    pthread_mutex_init(&pcap_mtx, NULL);
    pthread_cond_init(&pcap_cond, NULL);

    int sock = socket(AF_INET, SOCK_STREAM, 0);

	struct ifreq ifr;
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, "enp1s0", IFNAMSIZ-1);

	ioctl(sock, SIOCGIFHWADDR, &ifr);
    for (int i = 0; i < 6; i++) {
        mac[i] = ((uint8_t *)ifr.ifr_hwaddr.sa_data)[i];
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5000);
    inet_pton(AF_INET, "169.254.1.1", &addr.sin_addr.s_addr);
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    AeCtx ctx = ae_init(sock, NULL, 0);

    for (int i = 0; i < WIDGET_TESTS_NUM; i++) {
        for (int j = 0; j < TEST_ITERS; j++) {
            struct pcap_test_args args;
            args.msgs = 2;

            AeId win_id = widget_test_init(ctx);

            pthread_t thread;
            pthread_create(&thread, NULL, &pcap_test, &args);

            pthread_mutex_lock(&pcap_mtx);
            while (!test_ready) {
                pthread_cond_wait(&pcap_cond, &pcap_mtx);
            }

            widget_tests[i](ctx, win_id);

            test_ready = 0;
            pthread_mutex_unlock(&pcap_mtx);

            pthread_join(thread, NULL);

            printf("widget %d %lld\n", i, args.usec_out);
            fflush(stdout);

            widget_test_cleanup(ctx, win_id);
        }
    }

    int prim_msgs[PRIMITIVE_TESTS_NUM] = {
        2, 4, 4, 4, 4, 4, 4, 4, 4, 2
    };

    for (int i = 0; i < PRIMITIVE_TESTS_NUM; i++) {
        for (int j = 0; j < TEST_ITERS; j++) {
            struct pcap_test_args args;
            args.msgs = prim_msgs[i];

            AeId2 ids = primitive_test_init(ctx);

            pthread_t thread;
            pthread_create(&thread, NULL, &pcap_test, &args);

            pthread_mutex_lock(&pcap_mtx);
            while (!test_ready) {
                pthread_cond_wait(&pcap_cond, &pcap_mtx);
            }

            primitive_tests[i](ctx, ids.snd);

            test_ready = 0;
            pthread_mutex_unlock(&pcap_mtx);

            pthread_join(thread, NULL);

            printf("primitive %d %lld\n", i, args.usec_out);
            fflush(stdout);

            primitive_test_cleanup(ctx, ids.fst);
        }
    }

    int bitmap_test_iters[BITMAP_TESTS_NUM];
    for (int i = 0; i < BITMAP_TESTS_NUM; i++) bitmap_test_iters[i] = TEST_ITERS;
    bitmap_test_iters[BITMAP_TESTS_NUM-1] = 20;
    int pix = 1;
    for (int i = 0; i < BITMAP_TESTS_NUM; i++) {
        for (int j = 0; j < bitmap_test_iters[i]; j++) {
            struct pcap_test_args args;
            args.msgs = 1;

            AeId2 ids = bitmap_test_init(ctx);

            pthread_t thread;
            pthread_create(&thread, NULL, &pcap_test, &args);

            pthread_mutex_lock(&pcap_mtx);
            while (!test_ready) {
                pthread_cond_wait(&pcap_cond, &pcap_mtx);
            }

            bitmap_test(ctx, ids.snd, pix);

            test_ready = 0;
            pthread_mutex_unlock(&pcap_mtx);

            pthread_join(thread, NULL);

            printf("bitmap %d %lld\n", i, args.usec_out);
            fflush(stdout);

            bitmap_test_cleanup(ctx, ids.fst);
        }
        pix *= 10;
    }

    return 0;
}

