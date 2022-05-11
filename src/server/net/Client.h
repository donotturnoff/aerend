#ifndef CLIENT_H
#define CLIENT_H

#include "NetworkException.h"
#include "ProtocolException.h"
#include "gui/Widget.h"
#include "event/Event.h"
#include "gui/Window.h"
#include "gui/Button.h"
#include "gui/Canvas.h"
#include "gui/Picture.h"
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

enum class EventHandlerAction {
    NOTIFY_CLIENT, ADD_WIDGET, RM_WIDGET, RM_CHILD, DRAW_SHAPE, FILL_CANVAS, SET_PICTURE_DATA, OPEN_WINDOW, CLOSE_WINDOW, SET_STR
};

class Client {
public:
    Client(uint32_t cid, int sock, struct sockaddr_in port);
    ~Client();
    uint32_t next_wid();
    uint32_t next_sid();
    void push_event(Event* event);
    template <typename T, typename... Args> T* make_widget(Args... args);
    template <typename T, typename... Args> T* make_shape(Args... args);
private:
    std::vector<std::vector<uint8_t>> pop_event_bufs();

    template <typename T> T recv();
    template <typename T> void send(T data);
    void recv_into(uint8_t* buf, size_t len);
    void send_from(uint8_t* buf, size_t len);
    void send_status(uint8_t status);
    void send_status_id(uint8_t status, uint32_t id);

    template <typename T> T* get_widget(uint32_t wid);
    template <typename T> T* get_shape(uint32_t sid);

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
    void make_text();
    void destroy_widget();
    void destroy_shape();
    void add_widget();
    void rm_widget();
    void rm_child();
    void draw_shape();
    void fill_canvas();
    void set_picture_data();
    void open_window();
    void close_window();
    void set_str();
    void add_handler(EventType type);
    void add_key_press_handler();
    void add_key_release_handler();
    void add_key_type_handler();
    void add_mouse_press_handler();
    void add_mouse_release_handler();
    void add_mouse_click_handler();
    void add_mouse_move_handler();
    void add_mouse_scroll_handler();
    void add_action_handler();
    void add_mouse_enter_handler();
    void add_mouse_exit_handler();

    const std::array<std::function<void()>, 32> handlers;
    uint32_t cid;
    int sock;
    struct sockaddr_in addr;
    std::atomic<uint32_t> wid = 1, sid = 1;
    std::atomic<bool> running = true, closed = false;
    std::thread in_thread, out_thread;
    std::queue<std::vector<uint8_t>> event_buf_q;
    std::mutex event_q_mtx, send_mtx;
    std::condition_variable event_cond;
    std::unordered_map<uint32_t, std::unique_ptr<Widget>> widgets;
    std::unordered_map<uint32_t, std::unique_ptr<Shape>> shapes;
};

template <typename T>
T Client::recv() {
    T data;
    recv_into(&data, sizeof(T));
    return data;
}

template <typename T>
void Client::send(T data) {
    send_from(&data, sizeof(data));
}

/* Get widget of type T with given id, or null if it is the wrong type or does not exist */
template <typename T>
T* Client::get_widget(uint32_t wid) {
    auto it{widgets.find(wid)};
    if (it == widgets.end()) {
        return nullptr;
    }
    T* widget{dynamic_cast<T*>(it->second.get())};
    return widget;
}

template <typename T>
T* Client::get_shape(uint32_t sid) {
    auto it{shapes.find(sid)};
    if (it == shapes.end()) {
        return nullptr;
    }
    T* shape{dynamic_cast<T*>(it->second.get())};
    return shape;
}

/* Construct and register widget with correct id */
template <typename T, typename... Args>
T* Client::make_widget(Args... args) {
    auto widget{std::make_unique<T>(*this, std::forward<Args>(args)...)};
    auto wid{widget->get_wid()};
    widgets.emplace(wid, std::move(widget));
    return get_widget<T>(wid);
}

template <typename T, typename... Args>
T* Client::make_shape(Args... args) {
    auto shape{std::make_unique<T>(*this, std::forward<Args>(args)...)};
    auto sid{shape->get_sid()};
    shapes.emplace(sid, std::move(shape));
    return get_shape<T>(sid);
}

}

#endif // CLIENT_H

