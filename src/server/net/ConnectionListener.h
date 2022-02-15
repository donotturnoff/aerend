#ifndef CONNECTION_LISTENER_H
#define CONNECTION_LISTENER_H

#include "Client.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <cstdint>

namespace aerend {

class ConnectionListener {
public:
    ConnectionListener(in_port_t port);
    ~ConnectionListener();
    void rm_client(uint32_t cid);
private:
    void run();
    std::vector<uint32_t> get_removals();
    void process_removals();
    in_port_t port;
    int sock;
    std::thread thread, rm_thread;
    std::queue<uint32_t> rm_q;
    std::mutex rm_mtx;
    std::condition_variable rm_cond;
    std::unordered_map<uint32_t, std::unique_ptr<Client>> clients;
    std::atomic<uint32_t> next_cid = 1;
    std::atomic<bool> running = true;
};

}

#endif // CONNECTION_LISTENER_H

