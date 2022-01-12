#include "ConnectionListener.h"
#include "NetworkException.h"
#include <cstring>
#include <netdb.h>
#include <iostream>

namespace aerend {

ConnectionListener::ConnectionListener(in_port_t port) : port(port) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        throw NetworkException{"failed to open socket", errno};
    }
    int reuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0) {
        throw NetworkException{"failed to set SO_REUSEADDR", errno};
    }

    #ifdef SO_REUSEPORT
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) {
        throw NetworkException{"failed to set SO_REUSEPORT", errno};
    }
    #endif

    struct sockaddr_in s_addr;
    bzero((char *) &s_addr, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = INADDR_ANY;
    s_addr.sin_port = htons(port);
    if (bind(sock, (struct sockaddr*) &s_addr, sizeof(s_addr)) < 0) {
        throw NetworkException{"failed to bind socket", errno};
    }
    if (listen(sock, 5) < 0) {
        throw NetworkException{"failed to listen on socket", errno};
    }
    thread = std::thread(&ConnectionListener::run, this);
}

ConnectionListener::~ConnectionListener() {
    running = false;
    shutdown(sock, SHUT_RD);
    close(sock);
    thread.join();
}

void ConnectionListener::run() {
    while (running) {
        socklen_t c_len = sizeof(struct sockaddr_in);
        struct sockaddr_in c_addr;
        int c_sock = accept(sock, (struct sockaddr*) &c_addr, &c_len);
        if (c_sock < 0) {
            // TODO: log error
            continue;
        }
        clients[next_cid++] = std::move(std::make_unique<Client>(c_sock, c_addr));
    }
}

void ConnectionListener::rm_client(uint32_t cid) {
    clients.erase(cid);
}

}
