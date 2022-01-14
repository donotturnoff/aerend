#ifndef CLIENT_H
#define CLIENT_H

#include "NetworkException.h"
#include "gui/Widget.h"
#include "event/Event.h"
#include "gui/Window.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <cstdint>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <array>

namespace aerend {

class Client {
public:
    Client(uint32_t cid, int sock, struct sockaddr_in port);
    ~Client();
    uint32_t next_wid();
    void push_event(Event* event);
    template <typename T, typename... Args> T* make_widget(Args... args);
private:
    std::vector<Event*> pop_events();
    template <typename T> T recv();
    template <typename T> void send(T data);
    void recv_into(uint8_t* buf, size_t len);
    void send_from(uint8_t* buf, size_t len);
    template <typename T> T* get_widget(uint32_t wid);
    void run_in();
    void run_out();

    void make_window();
    void make_panel();
    void make_button();
    void make_label();
    void make_canvas();
    void make_picture();
    void make_rectangle();
    void make_ellipse();
    void make_line();
    void destroy_widget();
    void destroy_shape();
    void add_widget();
    void rm_widget();
    void draw_shape();
    void fill_canvas();
    void set_picture_data();
    void open_window();
    void close_window();

    const std::array<std::function<void()>, 18> handlers;

    uint32_t cid;
    int sock;
    struct sockaddr_in addr;
    std::atomic<uint32_t> wid = 0;
    std::atomic<bool> running = true, closed = false;
    std::unordered_map<uint32_t, std::unique_ptr<Widget>> widgets;
    std::thread in_thread, out_thread;
    std::queue<Event*> event_q;
    std::mutex event_q_mtx;
    std::condition_variable event_cond;
};

template <typename T>
T Client::recv() {
    T data;
    auto bytes = read(sock, &data, sizeof(T));
    if (bytes == 0) {
        throw NetworkException{"client closed connection"};
    } else if (bytes < 0) {
        throw NetworkException{"failed to read from socket", errno};
    } else if ((unsigned) bytes < sizeof(T)) {
        throw NetworkException{"read truncated data"};
    }
    return data;
}

// TODO: specialise to int types and include ntohs

template <typename T>
void Client::send(T data) {
    auto bytes = write(sock, &data, sizeof(T));
    if (bytes < 0) {
        throw NetworkException{"failed to write to socket", errno};
    }
}

template <typename T>
T* Client::get_widget(uint32_t wid) {
    if (widgets.count(wid) == 0) {
        return nullptr;
    }
    T* widget = dynamic_cast<T*>(widgets[wid].get());
    return widget;
}

template <typename T, typename... Args>
T* Client::make_widget(Args... args) {
    auto widget = std::make_unique<T>(*this, std::forward<Args>(args)...);
    auto wid = widget->get_wid();
    widgets.emplace(wid, std::move(widget));
    return get_widget<T>(wid);
}

}

#endif // CLIENT_H

