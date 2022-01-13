#include "Client.h"
#include <sys/socket.h>

namespace aerend {

Client::Client(int sock, struct sockaddr_in addr) : handlers{
            std::bind(&Client::create_window, this),
            std::bind(&Client::create_panel, this),
            std::bind(&Client::create_button, this),
            std::bind(&Client::create_label, this),
            std::bind(&Client::create_canvas, this),
            std::bind(&Client::create_picture, this),
            std::bind(&Client::create_rectangle, this),
            std::bind(&Client::create_ellipse, this),
            std::bind(&Client::create_line, this),
            std::bind(&Client::destroy_widget, this),
            std::bind(&Client::destroy_shape, this),
            std::bind(&Client::add_widget, this),
            std::bind(&Client::rm_widget, this),
            std::bind(&Client::draw_shape, this),
            std::bind(&Client::fill_canvas, this),
            std::bind(&Client::set_picture_data, this),
            std::bind(&Client::open_window, this),
            std::bind(&Client::close_window, this),
       }, sock(sock), addr(addr) {

    in_thread = std::thread(&Client::run_in, this);
    out_thread = std::thread(&Client::run_out, this);
}

Client::~Client() {
    running = false;
    shutdown(sock, SHUT_RDWR);
    close(sock);
    HaltEvent halt;
    push_event(&halt);
    in_thread.join();
    out_thread.join();
}

uint32_t Client::next_wid() {
    return wid++;
}

void Client::recv_into(uint8_t* buf, size_t len) {
    auto bytes = read(sock, buf, len);
    if (bytes == 0) {
        throw NetworkException{"client closed connection"};
    } else if (bytes < 0) {
        throw NetworkException{"failed to read from socket", errno};
    }
}

void Client::send_from(uint8_t* buf, size_t len) {
    auto bytes = write(sock, buf, len);
    if (bytes < 0) {
        throw NetworkException{"failed to write to socket", errno};
    }
}

void Client::run_in() {
    while (running) {
        uint8_t type;
        // TODO: use Client::recv
        auto bytes = read(sock, &type, 1);
        if (bytes == 0) {
            break;
        } else if (bytes < 0) {
            if (errno == EBADF) {
                break;
            }
            throw NetworkException{"failed to read message type", errno};
        }
        handlers[type]();
    }
    // TODO: remove from connection listener on client close
}

void Client::run_out() {
    while (running) {
        auto events = pop_events();
        for (const auto& event: events) {
            auto type = event->get_type();
            uint8_t buf[6];
            buf[0] = (uint8_t) type;
            int8_t len = 0;
            switch (type) {
                case EventType::KEY_PRESS:
                case EventType::KEY_RELEASE:
                case EventType::KEY_TYPE:
                    buf[1] = event->get_flags();
                    buf[2] = event->get_char();
                    len = 3;
                    break;
                case EventType::MOUSE_PRESS:
                case EventType::MOUSE_RELEASE:
                case EventType::MOUSE_CLICK:
                    buf[1] = event->get_flags();
                    len = 2;
                    break;
                case EventType::MOUSE_MOVE:
                case EventType::MOUSE_SCROLL:
                    buf[1] = event->get_flags();
                    *((int16_t*)(buf+2)) = htons(event->get_dx());
                    *((int16_t*)(buf+4)) = htons(event->get_dy());
                    len = 6;
                    break;
                case EventType::ACTION:
                case EventType::MOUSE_ENTER:
                case EventType::MOUSE_EXIT:
                    *((uint32_t*)(buf+1)) = event->get_widget()->get_wid();
                    len = 5;
                    break;
                case EventType::HALT:
                    break;
                default: // TODO: log unknown event
                    break;
            }
            if (len > 0) {
                send_from(buf, len);
            }
        }
    }
}

void Client::push_event(Event* event) {
    std::lock_guard<std::mutex> lock{event_q_mtx};
    event_q.push(event);
    event_cond.notify_one();
}

std::vector<Event*> Client::pop_events() {
    std::vector<Event*> events;
    std::unique_lock<std::mutex> lock{event_q_mtx};
    event_cond.wait(lock, [&]{ return !event_q.empty(); });
    while (!event_q.empty()) {
        events.push_back(event_q.front());
        event_q.pop();
    }
    return events;
}

void Client::create_window() {
    auto args = recv<uint8_t>();
    bool has_pos = args & 0x1;
    bool has_size = args & 0x2;
    bool has_title = args & 0x4;
    std::string title;
    int16_t x = 0, y = 0;
    int16_t w = 100, h = 100; // TODO: link these to defaults set in Window class
    if (has_pos) {
        x = ntohs(recv<int16_t>());
        y = ntohs(recv<int16_t>());
    }
    if (has_size) {
        w = ntohs(recv<int16_t>());
        h = ntohs(recv<int16_t>());
    }
    if (has_title) {
        uint16_t len = ntohs(recv<uint16_t>());
        title = std::string(len, '\0');
        recv_into((uint8_t*)&title[0], len);
    }
    auto window = std::make_unique<Window>(*this, x, y, w, h, title);
    uint32_t wid = window->get_wid();
    widgets[wid] = std::move(window);
    uint32_t wid_n = htonl(wid);
    send(wid_n);
}

void Client::create_panel() {

}

void Client::create_button() {

}

void Client::create_label() {

}

void Client::create_canvas() {

}

void Client::create_picture() {

}

void Client::create_rectangle() {

}

void Client::create_ellipse() {

}

void Client::create_line() {

}

void Client::destroy_widget() {

}

void Client::destroy_shape() {

}

void Client::add_widget() {

}

void Client::rm_widget() {

}

void Client::draw_shape() {

}

void Client::fill_canvas() {

}

void Client::set_picture_data() {

}

void Client::open_window() {
    auto wid = ntohl(recv<uint32_t>());
    auto window = get_widget<Window>(wid);
    if (window) {
        window->open();
    }
}

void Client::close_window() {
    auto wid = ntohl(recv<uint32_t>());
    auto window = get_widget<Window>(wid);
    if (window) {
        window->close();
    }
}

}
