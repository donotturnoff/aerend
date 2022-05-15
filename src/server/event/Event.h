#ifndef EVENT_H
#define EVENT_H

#include <cstdint>
#include <iostream>
#include <vector>

namespace aerend {

class Widget;
class Button;

enum class EventType : int8_t {
    HALT, KEY_PRESS, KEY_RELEASE, KEY_TYPE, MOUSE_PRESS, MOUSE_RELEASE, MOUSE_CLICK, MOUSE_MOVE, MOUSE_SCROLL, ACTION, MOUSE_ENTER, MOUSE_EXIT, MAX_NUM=MOUSE_EXIT
};

class Event {
public:
    void set_source(Widget* source) noexcept;
    EventType get_type() const noexcept;
    virtual char get_char() const noexcept { return '\0'; };
    virtual bool is_shift_down() const noexcept { return false; };
    virtual bool is_ctrl_down() const noexcept { return false; };
    virtual bool is_alt_down() const noexcept { return false; };
    virtual bool is_meta_down() const noexcept { return false; };
    virtual bool is_fn_down() const noexcept { return false; };
    virtual bool is_repeated() const noexcept { return false; };
    virtual int16_t get_dx() const noexcept { return 0; };
    virtual int16_t get_dy() const noexcept { return 0; };
    virtual bool is_left_down() const noexcept { return false; };
    virtual bool is_middle_down() const noexcept { return false; };
    virtual bool is_right_down() const noexcept { return false; };
    virtual Widget* get_source() const noexcept { return source; };
    virtual uint8_t get_flags() const noexcept { return 0; };
    virtual std::vector<uint8_t> get_buf() const noexcept;
protected:
    Event(EventType type, Widget* source = nullptr);
    Widget* source;
    const EventType type;
};



class HaltEvent : public Event {
public:
    HaltEvent();
    std::vector<uint8_t> get_buf() const noexcept;
};

class KeyEvent: public Event {
public:
    char get_char() const noexcept;
    bool is_shift_down() const noexcept;
    bool is_ctrl_down() const noexcept;
    bool is_alt_down() const noexcept;
    bool is_meta_down() const noexcept;
    bool is_fn_down() const noexcept;
    bool is_repeated() const noexcept;
    uint8_t get_flags() const noexcept;
    std::vector<uint8_t> get_buf() const noexcept;
protected:
    KeyEvent(EventType type, char c, bool shift, bool ctrl, bool alt, bool meta, bool fn, bool repeated);
private:
    const char c;
    const uint8_t keys;
};

class MouseEvent: public Event {
public:
    bool is_left_down() const noexcept;
    bool is_middle_down() const noexcept;
    bool is_right_down() const noexcept;
    uint8_t get_flags() const noexcept;
    std::vector<uint8_t> get_buf() const noexcept;
protected:
    MouseEvent(EventType type, int16_t dx, int16_t dy, bool left, bool middle, bool right);
    const int16_t dx, dy;
    const uint8_t buttons;
};

class WidgetEvent: public Event {
protected:
    WidgetEvent(EventType type, Widget* source);
};



class KeyPressEvent : public KeyEvent {
public:
    KeyPressEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn, bool repeated);
};

class KeyReleaseEvent : public KeyEvent {
public:
    KeyReleaseEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn);
};

class KeyTypeEvent : public KeyEvent {
public:
    KeyTypeEvent(char c, bool shift, bool ctrl, bool alt, bool meta, bool fn);
    KeyTypeEvent(Event& event);
};

class MouseClickEvent: public MouseEvent {
public:
    MouseClickEvent(bool left, bool middle, bool right);
    MouseClickEvent(Event& event);
};

class MouseMoveEvent: public MouseEvent {
public:
    MouseMoveEvent(int16_t dx, int16_t dy, bool left, bool middle, bool right);
    int16_t get_dx() const noexcept;
    int16_t get_dy() const noexcept;
    std::vector<uint8_t> get_buf() const noexcept;
};

class MousePressEvent: public MouseEvent {
public:
    MousePressEvent(bool left, bool middle, bool right);
};

class MouseReleaseEvent: public MouseEvent {
public:
    MouseReleaseEvent(bool left, bool middle, bool right);
};

class MouseScrollEvent: public MouseEvent {
public:
    MouseScrollEvent(int16_t dx, int16_t dy, bool left, bool middle, bool right);
    int16_t get_dx() const noexcept;
    int16_t get_dy() const noexcept;
    std::vector<uint8_t> get_buf() const noexcept;
};

class ActionEvent: public WidgetEvent {
public:
    ActionEvent(Widget* source);
};

class MouseEnterEvent: public WidgetEvent {
public:
    MouseEnterEvent(Widget* source);
};

class MouseExitEvent: public WidgetEvent {
public:
    MouseExitEvent(Widget* source);
};

}

#endif // EVENT_H

