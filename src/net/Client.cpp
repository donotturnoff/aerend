#include "Client.h"
#include <sys/socket.h>

namespace aerend {

Client::Client(int sock, struct sockaddr_in addr) : handlers{
            std::bind(&Client::make_window, this),
            std::bind(&Client::make_panel, this),
            std::bind(&Client::make_button, this),
            std::bind(&Client::make_label, this),
            std::bind(&Client::make_canvas, this),
            std::bind(&Client::make_picture, this),
            std::bind(&Client::make_rectangle, this),
            std::bind(&Client::make_ellipse, this),
            std::bind(&Client::make_line, this),
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

template <>
std::unique_ptr<LayoutManager> Client::recv<std::unique_ptr<LayoutManager>>() {
    std::unique_ptr<LayoutManager> lm;
    auto type = recv<uint8_t>();
    if (type == 0x00) { // GridLayout with equal-sized rows and columns
        auto cols = ntohs(recv<int16_t>());
        auto rows = ntohs(recv<int16_t>());
        lm = std::make_unique<GridLayout>(rows, cols);
    } else if (type == 0x01) { // GridLayout with custom proportions
        int16_t col_count = ntohs(recv<int16_t>());
        int16_t row_count = ntohs(recv<int16_t>());
        std::vector<int16_t> x_props, y_props;
        for (int16_t col = 0; col < col_count; col++) {
            x_props.push_back(ntohs(recv<int16_t>()));
        }
        for (int16_t row = 0; row < row_count; row++) {
            y_props.push_back(ntohs(recv<int16_t>()));
        }
        lm = std::make_unique<GridLayout>(x_props, y_props);
    }
    return lm;
}

template <>
Colour Client::recv<Colour>() {
    return Colour{recv<uint8_t>(), recv<uint8_t>(), recv<uint8_t>(), recv<uint8_t>()};
}

template <>
Border Client::recv<Border>() {
    return Border{recv<Colour>(), ntohs(recv<int16_t>())};
}

template <>
Margin Client::recv<Margin>() {
    return Margin{ntohs(recv<int16_t>())};
}

template <>
Padding Client::recv<Padding>() {
    return Padding{ntohs(recv<int16_t>())};
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
        if (type < handlers.size()) {
            try {
                handlers[type]();
            } catch (NetworkException& e) {
                std::cerr << "Client::run_in(): " << e.what() << std::endl;
            }
        } else {
            std::cerr << "Client::run_in(): invalid operation: " << (uint32_t) type << std::endl;
        }
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
                    std::cerr << "Client::run_out(): unknown event type: " << (uint32_t) type << std::endl;
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

void Client::make_window() {
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
    auto window = make_widget<Window>(x, y, w, h, title);
    uint32_t wid = htonl(window->get_wid());
    send(wid);
}

void Client::make_panel() {
    auto args = recv<uint8_t>();
    bool has_lm = args & 0x1;
    bool has_bg_colour = args & 0x2;
    bool has_border = args & 0x4;
    bool has_margin = args & 0x8;
    bool has_padding = args & 0x10;
    std::unique_ptr<LayoutManager> lm;
    Colour bg_colour{Colour::white()};
    Border border;
    Margin margin;
    Padding padding;
    if (has_lm) {
        lm = recv<std::unique_ptr<LayoutManager>>();
        if (!lm) {
            lm = std::make_unique<GridLayout>();
        }
    }
    if (has_bg_colour) {
        bg_colour = recv<Colour>();
    }
    if (has_border) {
        border = recv<Border>();
    }
    if (has_margin) {
        margin = recv<Margin>();
    }
    if (has_padding) {
        padding = recv<Padding>();
    }
    auto panel = make_widget<Panel>(std::move(lm), bg_colour, border, margin, padding);
    auto wid = htonl(panel->get_wid());
    send(wid);
}

void Client::make_button() {

}

void Client::make_label() {

}

void Client::make_canvas() {

}

void Client::make_picture() {

}

void Client::make_rectangle() {

}

void Client::make_ellipse() {

}

void Client::make_line() {

}

void Client::destroy_widget() {

}

void Client::destroy_shape() {

}

void Client::add_widget() {
    auto p_wid = ntohl(recv<uint32_t>());
    auto c_wid = ntohl(recv<uint32_t>());
    auto parent = get_widget<Container>(p_wid);
    auto child = get_widget<Widget>(c_wid);
    if (parent && child) {
        parent->add(child);
    }
}

void Client::rm_widget() {
    auto p_wid = ntohl(recv<uint32_t>());
    auto c_wid = ntohl(recv<uint32_t>());
    auto parent = get_widget<Container>(p_wid);
    auto child = get_widget<Widget>(c_wid);
    if (parent && child) {
        parent->rm(child);
    }
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
