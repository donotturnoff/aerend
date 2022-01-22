#include "Client.h"
#include "AerendServer.h"
#include "bitmap/BitmapException.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace aerend {

Client::Client(uint32_t cid, int sock, struct sockaddr_in addr) : handlers{
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
       }, cid(cid), sock(sock), addr(addr) {

    in_thread = std::thread(&Client::run_in, this);
    out_thread = std::thread(&Client::run_out, this);
    std::cout << "Accepted connection from " << inet_ntoa(addr.sin_addr) << std::endl;
}

Client::~Client() {
    running = false;
    shutdown(sock, SHUT_RDWR);
    close(sock);
    HaltEvent halt;
    push_event(&halt);
    in_thread.join();
    out_thread.join();
    std::cout << "Closed connection to " << inet_ntoa(addr.sin_addr) << std::endl;
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
    auto type = recv<uint8_t>();
    if (type == 0x00) { // GridLayout with equal-sized rows and columns
        auto cols = ntohs(recv<int16_t>());
        auto rows = ntohs(recv<int16_t>());
        return std::make_unique<GridLayout>(rows, cols);
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
        return std::make_unique<GridLayout>(x_props, y_props);
    }
    return std::make_unique<LayoutManager>();
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

template <>
std::string Client::recv<std::string>() {
    uint16_t len {ntohs(recv<uint16_t>())};
    std::string text(len, '\0');
    recv_into((uint8_t*)&text[0], len);
    return text;
}

void Client::send_from(uint8_t* buf, size_t len) {
    auto bytes = write(sock, buf, len);
    if (bytes < 0) {
        throw NetworkException{"failed to write to socket", errno};
    }
}

void Client::send_status(uint8_t status) {
    send<uint8_t>(status);
}

void Client::send_status_wid(uint8_t status, uint32_t wid) {
    const size_t len = sizeof(status) + sizeof(wid);
    uint8_t buf[len];
    buf[0] = 0x00;
    *((uint32_t*)(buf+1)) = htonl(wid);
    send_from(buf, len);
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
            } catch (ProtocolException& e) {
                std::cerr << "Client::run_in(): " << e.what() << std::endl;
                send_status(e.get_status());
            }
        } else {
            std::cerr << "Client::run_in(): invalid operation: " << (uint32_t) type << std::endl;
            send_status(0xFE);
        }
    }

    AerendServer::the().get_connection_listener().rm_client(cid);
}

void Client::run_out() {
    while (running) {
        auto events = pop_events();
        for (const auto& event: events) {
            auto type = event->get_type();
            auto source = event->get_source();
            auto wid = source ? source->get_wid() : 0;

            uint8_t buf[6];
            buf[0] = (uint8_t) type;
            *((uint32_t*)(buf+1)) = wid;
            int8_t len = 0;

            switch (type) {
                case EventType::KEY_PRESS:
                case EventType::KEY_RELEASE:
                case EventType::KEY_TYPE:
                    buf[5] = event->get_flags();
                    buf[6] = event->get_char();
                    len = 7;
                    break;
                case EventType::MOUSE_PRESS:
                case EventType::MOUSE_RELEASE:
                case EventType::MOUSE_CLICK:
                    buf[5] = event->get_flags();
                    len = 6;
                    break;
                case EventType::MOUSE_MOVE:
                case EventType::MOUSE_SCROLL:
                    buf[5] = event->get_flags();
                    *((int16_t*)(buf+6)) = htons(event->get_dx());
                    *((int16_t*)(buf+8)) = htons(event->get_dy());
                    len = 10;
                    break;
                case EventType::ACTION:
                case EventType::MOUSE_ENTER:
                case EventType::MOUSE_EXIT:
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
    auto args{recv<uint8_t>()};
    auto x      {(args & 0x1) ? ntohs(recv<int16_t>()) : Window::def_x};
    auto y      {(args & 0x1) ? ntohs(recv<int16_t>()) : Window::def_y};
    auto w      {(args & 0x2) ? ntohs(recv<int16_t>()) : Window::def_w};
    auto h      {(args & 0x2) ? ntohs(recv<int16_t>()) : Window::def_h};
    auto title  {args & (0x4) ? recv<std::string>() : Window::def_title};
    auto window = make_widget<Window>(x, y, w, h, title);
    send_status_wid(0x00, window->get_wid());
}

void Client::make_panel() {
    auto args{recv<uint8_t>()};
    auto lm         {(args & 0x01) ? recv<std::unique_ptr<LayoutManager>>() : Panel::def_lm()};
    auto bg_colour  {(args & 0x02) ? recv<Colour>() : Panel::def_bg_colour};
    auto border     {(args & 0x04) ? recv<Border>() : Panel::def_border};
    auto margin     {(args & 0x08) ? recv<Margin>() : Panel::def_margin};
    auto padding    {(args & 0x10) ? recv<Padding>() : Panel::def_padding};
    auto panel = make_widget<Panel>(std::move(lm), bg_colour, border, margin, padding);
    send_status_wid(0x00, panel->get_wid());
}

void Client::make_button() {
    auto args{recv<uint8_t>()};
    auto str        {(args & 0x01) ? recv<std::string>() : Button::def_str};
    auto font_path  {(args & 0x02) ? recv<std::string>() : Button::def_font_path};
    auto font_size  {(args & 0x02) ? recv<uint16_t>() : Button::def_font_size};
    auto colour     {(args & 0x04) ? recv<Colour>() : Button::def_colour};
    auto bg_colour  {(args & 0x08) ? recv<Colour>() : Button::def_bg_colour};
    auto border     {(args & 0x10) ? recv<Border>() : Button::def_border};
    auto margin     {(args & 0x20) ? recv<Margin>() : Button::def_margin};
    auto padding    {(args & 0x40) ? recv<Padding>() : Button::def_padding};
    auto wrap       {(args & 0x80) ? recv<uint16_t>() : Button::def_wrap};
    auto button{make_widget<Button>(str, font_path, font_size, colour, bg_colour, border, margin, padding, wrap)};
    send_status_wid(0x00, button->get_wid());
}

void Client::make_label() {
    auto args{recv<uint8_t>()};
    auto str        {(args & 0x01) ? recv<std::string>() : Label::def_str};
    auto font_path  {(args & 0x02) ? recv<std::string>() : Label::def_font_path};
    auto font_size  {(args & 0x02) ? recv<uint16_t>() : Label::def_font_size};
    auto colour     {(args & 0x04) ? recv<Colour>() : Label::def_colour};
    auto bg_colour  {(args & 0x08) ? recv<Colour>() : Label::def_bg_colour};
    auto border     {(args & 0x10) ? recv<Border>() : Label::def_border};
    auto margin     {(args & 0x20) ? recv<Margin>() : Label::def_margin};
    auto padding    {(args & 0x40) ? recv<Padding>() : Label::def_padding};
    auto wrap       {(args & 0x80) ? recv<uint16_t>() : Label::def_wrap};
    auto label{make_widget<Label>(str, font_path, font_size, colour, bg_colour, border, margin, padding, wrap)};
    send_status_wid(0x00, label->get_wid());
}

void Client::make_canvas() {
    auto cvs_w{recv<uint16_t>()};
    auto cvs_h{recv<uint16_t>()};
    auto canvas{make_widget<Canvas>(cvs_w, cvs_h)};
    send_status_wid(0x00, canvas->get_wid());
}

void Client::make_picture() {
    auto flags{recv<uint8_t>()};
    auto pic_w{ntohs(recv<uint16_t>())};
    auto pic_h{ntohs(recv<uint16_t>())};
    auto size{pic_w*pic_h*4};
    // TODO: rewrite to avoid malloc
    std::vector<uint32_t> data(pic_w*pic_h, 0);
    if (flags & 0x1) {
        recv_into((uint8_t*) (data.data()), size);
    }
    try {
        auto picture{make_widget<Picture>(pic_w, pic_h, data)};
        send_status_wid(0x00, picture->get_wid());
    } catch (BitmapException& e) {
        send_status(0x01);
    }
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
    if (!parent) {
        send_status(0x01);
    } else if (!child) {
        send_status(0x02);
    } else {
        parent->add(child);
        send_status(0x00);
    }
}

void Client::rm_widget() {
    auto p_wid = ntohl(recv<uint32_t>());
    auto c_wid = ntohl(recv<uint32_t>());
    auto parent = get_widget<Container>(p_wid);
    auto child = get_widget<Widget>(c_wid);
    // TODO: return different status code if child does not belong to parent
    if (!parent) {
        send_status(0x01);
    } else if (!child) {
        send_status(0x02);
    } else {
        parent->rm(child);
        send_status(0x00);
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
    // TODO: return different code if window was already open
    if (window) {
        window->open();
        send_status(0x00);
    } else {
        send_status(0x01);
    }
}

void Client::close_window() {
    auto wid = ntohl(recv<uint32_t>());
    auto window = get_widget<Window>(wid);
    // TODO: return different code if window was already closed
    if (window) {
        window->close();
        send_status(0x00);
    } else {
        send_status(0x01);
    }
}

}
