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
    std::cout << "Listening on port " << port << std::endl;
    thread = std::thread(&ConnectionListener::run, this);
    rm_thread = std::thread(&ConnectionListener::process_removals, this);
}

ConnectionListener::~ConnectionListener() {
    running = false;
    shutdown(sock, SHUT_RD);
    close(sock);
    rm_client(0);
    thread.join();
    rm_thread.join();
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
        // TODO: not just IPv4
        uint32_t cid = next_cid++;
        clients[cid] = std::move(std::make_unique<Client>(cid, c_sock, c_addr));
    }
}

void ConnectionListener::rm_client(uint32_t cid) {
    std::lock_guard<std::mutex> lock{rm_mtx};
    rm_q.push(cid);
    rm_cond.notify_one();
}

std::vector<uint32_t> ConnectionListener::get_removals() {
    std::vector<uint32_t> rms;
    std::unique_lock<std::mutex> lock{rm_mtx};
    rm_cond.wait(lock, [&]{ return !rm_q.empty(); });
    while (!rm_q.empty()) {
        rms.push_back(rm_q.front());
        rm_q.pop();
    }
    return rms;
}

void ConnectionListener::process_removals() {
    while (running) {
        auto cids = get_removals();
        for (const auto& cid : cids) {
            if (cid == 0) {
                break;
            }
            clients.erase(cid);
        }
    }
}

}
