#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "libaerend.h"

#define AE_STACK_DEBUG 1

#ifdef AE_STACK_DEBUG
size_t ae_max_stack = -1;
bool track_stack = false;
#endif // AE_STACK_DEBUG

const size_t event_lens[] = {
    0, // Halt, not used
    7, 7, 7, // Key press & release & type
    6, 6, 6, // Mouse press & release & click
    10, 10,  // Mouse move & scroll
    5, 5, 5  // Action, mouse enter & exit
};

static inline uint8_t event_type_from_status(uint8_t status) {
    return status - 0x80;
}

static inline uint8_t code_from_event_type(uint8_t type) {
    return type + 0x14;
}

static inline uint8_t *push_int8_t(uint8_t *buf, int8_t n) {
    *(int8_t *)buf = n;
    return buf + sizeof(n);
}

static inline uint8_t *push_uint8_t(uint8_t *buf, uint8_t n) {
    *buf = n;
    return buf + sizeof(n);
}

static inline uint8_t *push_int16_t(uint8_t *buf, int16_t n) {
    *(int16_t *)buf = htons(n);
    return buf + sizeof(n);
}

static inline uint8_t *push_uint16_t(uint8_t *buf, uint16_t n) {
    *(uint16_t *)buf = htons(n);
    return buf + sizeof(n);
}

static inline uint8_t *push_int32_t(uint8_t *buf, int32_t n) {
    *(int32_t *)buf = htonl(n);
    return buf + sizeof(n);
}

static inline uint8_t *push_uint32_t(uint8_t *buf, uint32_t n) {
    *(uint32_t *)buf = htonl(n);
    return buf + sizeof(n);
}

static inline uint8_t *push_id(uint8_t *buf, AeId id) {
    *(AeId *)buf = htonl(id);
    return buf + sizeof(id);
}

static inline uint8_t *push_colour(uint8_t *buf, AeColour colour) {
    *(AeColour *)buf = htonl(colour);
    return buf + sizeof(colour);
}

static inline uint8_t *push_border(uint8_t *buf, AeBorder border) {
    buf = push_colour(buf, border.colour);
    buf = push_uint16_t(buf, border.t);
    return buf;
}

static inline uint8_t *push_margin(uint8_t *buf, AeMargin margin) {
    return push_int16_t(buf, margin);
}

static inline uint8_t *push_padding(uint8_t *buf, AePadding padding) {
    return push_int16_t(buf, padding);
}

static inline uint8_t *push_buf(uint8_t *buf, const uint8_t *src_buf, uint32_t src_len) {
    memcpy(buf, src_buf, src_len);
    return buf + src_len;
}

static inline uint8_t *push_str(uint8_t *buf, const char *str, uint16_t str_len) {
    buf = push_uint16_t(buf, str_len);
    return push_buf(buf, (const uint8_t *)str, str_len);
}

#ifdef AE_STACK_DEBUG
static inline void update_max_stack(void *sp) {
    if ((size_t) sp < ae_max_stack && track_stack) {
        ae_max_stack = (size_t) sp;
    }
}

void ae_track_stack(bool track) {
    track_stack = track;
    if (!track) {
        ae_max_stack = -1;
    }
}
#endif

AeCtx ae_init(int sock, AeEvent *evbuf, size_t evbuf_len) {
    return (AeCtx){.sock = sock, .err = AE_NO_ERR, .evbuf_len = evbuf_len, .first_ev = -1, .last_ev = -1, .evbuf = evbuf};
}

void recv_into(AeCtx *ctx, void *buf, size_t len) {
    ssize_t bytes = recv(ctx->sock, buf, len, 0);
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        ctx->err |= AE_RECV_TIMEOUT;
    } else if (bytes < 0) {
        ctx->err |= AE_SOCK_ERR;
    } else if (bytes == 0) {
        ctx->err |= AE_SOCK_CLOSED;
    }
#ifdef AE_STACK_DEBUG
    int x = 0;
    update_max_stack(&x);
#endif
}

void send_from(AeCtx *ctx, const void *buf, size_t len) {
    ssize_t bytes = send(ctx->sock, buf, len, 0);
    if (bytes < 0) {
        ctx->err |= AE_SOCK_ERR;
    }
#ifdef AE_STACK_DEBUG
    int x = 0;
    update_max_stack(&x);
#endif
}

void process_event(AeCtx *ctx, uint8_t type) {
    uint8_t buf[event_lens[type]-1];
    recv_into(ctx, buf, event_lens[type]-1);
    if (ctx->err) return;

    size_t buf_len = ctx->evbuf_len;
    if ((ctx->last_ev+1)%buf_len != ctx->first_ev) {
        if (ctx->first_ev == -1 && ctx->last_ev == -1) {
            ctx->first_ev = 0;
            ctx->last_ev = 0;
        } else if (ctx->last_ev == buf_len-1 && ctx->first_ev != 0) {
            ctx->last_ev = 0;
        } else {
            ctx->last_ev = (ctx->last_ev + 1) % buf_len;
        }

        AeEvent *event = ctx->evbuf + ctx->last_ev;
        AeId wid = ((AeId *) buf)[0];
        event->h.type = (AeEventType) type;
        event->h.wid = wid;
        switch (type) {
            case AE_KEY_PRESS:
            case AE_KEY_RELEASE:
            case AE_KEY_TYPE:
                event->kp.keys = buf[sizeof(wid)];
                event->kp.c = *(char *)(buf+sizeof(wid)+sizeof(event->kp.keys));
                break;
            case AE_MOUSE_MOVE:
            case AE_MOUSE_SCROLL:
                event->mm.dx = *(int16_t *)(buf+sizeof(wid)+sizeof(event->mm.buttons));
                event->mm.dy = *(int16_t *)(buf+sizeof(wid)+sizeof(event->mm.buttons)+sizeof(event->mm.dx));
            case AE_MOUSE_PRESS:
            case AE_MOUSE_RELEASE:
            case AE_MOUSE_CLICK:
                event->mm.buttons = buf[sizeof(wid)];
                break;
            default:
                break;
        }
    } else {
        ctx->err |= AE_EVBUF_FULL;
    }
}

// To prevent an infinite loop if server fails to send response, use setsockopt to specify SO_RCVTIMEO
AeStatus recv_status(AeCtx *ctx) {
    uint8_t status;
    while (1) {
        recv_into(ctx, &status, sizeof(status));
        if (ctx->err) return (AeStatus) 0xFF;
        if (status >= 0x80 && status < 0xC0) { // Response is actually an incoming event
            process_event(ctx, event_type_from_status(status));
            if (ctx->err) return (AeStatus) 0xFF;
        } else {
            break;
        }
    }
    return (AeStatus) status;
}

AeStatusId recv_status_id(AeCtx *ctx) {
    uint8_t status;
    while (1) {
        recv_into(ctx, &status, sizeof(status));
        if (ctx->err) return (AeStatusId){.status=0xFF};
        if (status >= 0x80 && status < 0xC0) { // Response is actually an incoming event
            process_event(ctx, event_type_from_status(status));
            if (ctx->err) return (AeStatusId){.status=0xFF};
        } else {
            break;
        }
    }
    if (status == 0) {
        uint32_t id;
        recv_into(ctx, &id, sizeof(id));
        if (ctx->err) return (AeStatusId){.status=0xFF};
        return (AeStatusId){.status=status, .id=ntohl(id)};
    } else {
        return (AeStatusId){.status=status};
    }
}

AeEvent *ae_recv_event(AeCtx *ctx) {
    uint8_t status;
    recv_into(ctx, &status, sizeof(status));
    if (ctx->err) return NULL;
    if (status >= 0x80 && status < 0xC0) { // Response is actually an incoming event
        process_event(ctx, event_type_from_status(status));
        if (ctx->err) return NULL;
        return ctx->evbuf + ctx->last_ev;
    } else {
        return NULL;
    }
}

AeEvent *ae_peek_event(AeCtx *ctx) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    update_max_stack(&x);
#endif
    if (ctx->first_ev > -1) {
        return ctx->evbuf + ctx->first_ev;
    } else {
        return NULL;
    }
}

void ae_pop_event(AeCtx *ctx) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    update_max_stack(&x);
#endif
    if (ctx->first_ev > -1) {
        if (ctx->first_ev == ctx->last_ev) {
            ctx->first_ev = -1;
            ctx->last_ev = -1;
        } else if (ctx->first_ev == ctx->evbuf_len-1) {
            ctx->first_ev = 0;
        } else {
            ctx->first_ev++;
        }
    }
}

AeColour ae_colour_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    update_max_stack(&x);
#endif
    return (a << 24) | (r << 16) | (g << 8) | b;
}

AeColour ae_colour_rgb(uint8_t r, uint8_t g, uint8_t b) {
#ifdef AE_STACK_DEBUG
    int x = 0;
    update_max_stack(&x);
#endif
    return 0xFF000000 | (r << 16) | (g << 8) | b;
}

AeStatusId ae_make_window(AeCtx *ctx, uint8_t args, AeWindow *win) {
    const size_t max_len = 1 + sizeof(args) + sizeof(AeWindow) + (win ? win->title_len : 0);
    uint8_t buf[max_len];
    uint8_t *tmp = push_uint8_t(buf, AE_MAKE_WINDOW);
    tmp = push_uint8_t(tmp, args);
    if (args & 0x01) tmp = push_int16_t(tmp, win->x);
    if (args & 0x02) tmp = push_int16_t(tmp, win->y);
    if (args & 0x04) tmp = push_int16_t(tmp, win->w);
    if (args & 0x08) tmp = push_int16_t(tmp, win->h);
    if (args & 0x10) tmp = push_str(tmp, win->title, win->title_len);
    send_from(ctx, buf, tmp-buf);
    if (ctx->err) return (AeStatusId){.status=0xFF};
    return recv_status_id(ctx);
}

AeStatusId ae_make_panel(AeCtx *ctx, uint8_t args, AePanel *pnl) {
    const size_t max_len = 1 + sizeof(args) + sizeof(AePanel) + (pnl && args&0x01 && pnl->layout.type == AE_IRREGULAR_GRID_LAYOUT ? pnl->layout.value.igl.col_count + pnl->layout.value.igl.row_count : 0);
    uint8_t buf[max_len];
    uint8_t *tmp = push_uint8_t(buf, AE_MAKE_PANEL);
    tmp = push_uint8_t(tmp, args);
    if (args & 0x01) {
        tmp = push_uint8_t(tmp, pnl->layout.type);
        if (pnl->layout.type == AE_GRID_LAYOUT) {
            tmp = push_int16_t(tmp, pnl->layout.value.gl.cols);
            tmp = push_int16_t(tmp, pnl->layout.value.gl.rows);
        } else if (pnl->layout.type == AE_IRREGULAR_GRID_LAYOUT) {
            uint16_t col_count = pnl->layout.value.igl.col_count;
            uint16_t row_count = pnl->layout.value.igl.row_count;
            tmp = push_uint16_t(tmp, col_count);
            tmp = push_buf(tmp, pnl->layout.value.igl.cols, col_count);
            tmp = push_uint16_t(tmp, row_count);
            tmp = push_buf(tmp, pnl->layout.value.igl.rows, row_count);
        }
    }
    if (args & 0x02) tmp = push_colour(tmp, pnl->bg_colour);
    if (args & 0x04) tmp = push_border(tmp, pnl->border);
    if (args & 0x08) tmp = push_margin(tmp, pnl->margin);
    if (args & 0x10) tmp = push_padding(tmp, pnl->padding);
    send_from(ctx, buf, tmp-buf);
    if (ctx->err) return (AeStatusId){.status=0xFF};
    return recv_status_id(ctx);
}

AeStatusId ae_make_button(AeCtx *ctx, uint8_t args, AeButton *btn) {
    size_t len = 1 + sizeof(AeButton) + (btn ? (btn->str_len + btn->font_path_len) : 0);
    uint8_t buf[len];
    uint8_t *tmp = push_uint8_t(buf, AE_MAKE_BUTTON);
    tmp = push_uint8_t(tmp, args);
    if (args & 0x01) tmp = push_str(tmp, btn->str, btn->str_len);
    if (args & 0x02) {
        tmp = push_str(tmp, btn->font_path, btn->font_path_len);
        tmp = push_uint16_t(tmp, btn->font_size);
    }
    if (args & 0x04) tmp = push_colour(tmp, btn->colour);
    if (args & 0x08) tmp = push_colour(tmp, btn->bg_colour);
    if (args & 0x10) tmp = push_border(tmp, btn->border);
    if (args & 0x20) tmp = push_margin(tmp, btn->margin);
    if (args & 0x40) tmp = push_padding(tmp, btn->padding);
    if (args & 0x80) tmp = push_int16_t(tmp, btn->wrap);
    send_from(ctx, buf, tmp-buf);
    if (ctx->err) return (AeStatusId){.status=0xFF};
    return recv_status_id(ctx);
}

AeStatusId ae_make_label(AeCtx *ctx, uint8_t args, AeLabel *lbl) {
    size_t len = 1 + sizeof(AeLabel) + (lbl ? (lbl->str_len + lbl->font_path_len) : 0);
    uint8_t buf[len];
    uint8_t *tmp = push_uint8_t(buf, AE_MAKE_LABEL);
    tmp = push_uint8_t(tmp, args);
    if (args & 0x01) tmp = push_str(tmp, lbl->str, lbl->str_len);
    if (args & 0x02) {
        tmp = push_str(tmp, lbl->font_path, lbl->font_path_len);
        tmp = push_uint16_t(tmp, lbl->font_size);
    }
    if (args & 0x04) tmp = push_colour(tmp, lbl->colour);
    if (args & 0x08) tmp = push_colour(tmp, lbl->bg_colour);
    if (args & 0x10) tmp = push_border(tmp, lbl->border);
    if (args & 0x20) tmp = push_margin(tmp, lbl->margin);
    if (args & 0x40) tmp = push_padding(tmp, lbl->padding);
    if (args & 0x80) tmp = push_int16_t(tmp, lbl->wrap);
    send_from(ctx, buf, tmp-buf);
    if (ctx->err) return (AeStatusId){.status=0xFF};
    return recv_status_id(ctx);
}

AeStatusId ae_make_canvas(AeCtx *ctx, AeCanvas *cvs) {
    const size_t len = sizeof(uint8_t) + sizeof(cvs->w) + sizeof(cvs->h);
    uint8_t buf[len];
    uint8_t *tmp = push_uint8_t(buf, AE_MAKE_CANVAS);
    tmp = push_uint16_t(tmp, cvs->w);
    tmp = push_uint16_t(tmp, cvs->h);
    send_from(ctx, buf, tmp-buf);
    if (ctx->err) return (AeStatusId){.status=0xFF};
    return recv_status_id(ctx);
}

AeStatusId ae_make_picture(AeCtx *ctx, uint8_t args, AePicture *pic) {
    const size_t len = 1 + sizeof(args) + sizeof(AePicture) + (pic ? pic->w*pic->h*4 : 0);
    uint8_t buf[len];
    uint8_t *tmp = push_uint8_t(buf, AE_MAKE_PICTURE);
    tmp = push_uint8_t(tmp, args);
    tmp = push_uint16_t(tmp, pic->w);
    tmp = push_uint16_t(tmp, pic->h);
    if (args & 0x01) {
        tmp = push_buf(tmp, pic->pix, pic->w*pic->h*4);
    }
    send_from(ctx, buf, tmp-buf);
    if (ctx->err) return (AeStatusId){.status=0xFF};
    return recv_status_id(ctx);
}

AeStatusId ae_make_rectangle(AeCtx *ctx, uint8_t args, AeRectangle *rect) {
    const size_t max_len = 1 + sizeof(args) + sizeof(AeRectangle);
    uint8_t buf[max_len];
    uint8_t *tmp = push_uint8_t(buf, AE_MAKE_RECTANGLE);
    tmp = push_uint8_t(tmp, args);
    tmp = push_int16_t(tmp, rect->x);
    tmp = push_int16_t(tmp, rect->y);
    tmp = push_uint16_t(tmp, rect->w);
    tmp = push_uint16_t(tmp, rect->h);
    tmp = push_colour(tmp, rect->colour);
    if (args & 0x01) tmp = push_border(tmp, rect->border);
    send_from(ctx, buf, tmp-buf);
    if (ctx->err) return (AeStatusId){.status=0xFF};
    return recv_status_id(ctx);
}

AeStatusId ae_make_ellipse(AeCtx *ctx, uint8_t args, AeEllipse *ellp) {
    const size_t max_len = sizeof(uint8_t) + sizeof(args) + sizeof(ellp->x) + sizeof(ellp->y) + sizeof(ellp->w) + sizeof(ellp->h) + sizeof(ellp->colour) + sizeof(ellp->border.colour) + sizeof(ellp->border.t);
    uint8_t buf[max_len];
    uint8_t *tmp = push_uint8_t(buf, AE_MAKE_ELLIPSE);
    tmp = push_uint8_t(tmp, args);
    tmp = push_int16_t(tmp, ellp->x);
    tmp = push_int16_t(tmp, ellp->y);
    tmp = push_uint16_t(tmp, ellp->w);
    tmp = push_uint16_t(tmp, ellp->h);
    tmp = push_colour(tmp, ellp->colour);
    if (args & 0x01) tmp = push_border(tmp, ellp->border);
    send_from(ctx, buf, tmp-buf);
    if (ctx->err) return (AeStatusId){.status=0xFF};
    return recv_status_id(ctx);
}

AeStatusId ae_make_line(AeCtx *ctx, AeLine *line) {
    const size_t len = sizeof(uint8_t) + sizeof(line->x0) + sizeof(line->y0) + sizeof(line->x1) + sizeof(line->y1);
    uint8_t buf[len];
    uint8_t *tmp = push_uint8_t(buf, AE_MAKE_LINE);
    tmp = push_int16_t(tmp, line->x0);
    tmp = push_int16_t(tmp, line->y0);
    tmp = push_int16_t(tmp, line->x1);
    tmp = push_int16_t(tmp, line->y1);
    tmp = push_colour(tmp, line->colour);
    send_from(ctx, buf, tmp-buf);
    if (ctx->err) return (AeStatusId){.status=0xFF};
    return recv_status_id(ctx);
}

AeStatusId ae_make_text(AeCtx *ctx, AeText *text) {
    size_t len = 1 + sizeof(*text) + text->str_len + text->font_path_len;
    uint8_t buf[len];
    uint8_t *tmp = push_uint8_t(buf, AE_MAKE_TEXT);
    tmp = push_str(tmp, text->str, text->str_len);
    tmp = push_str(tmp, text->font_path, text->font_path_len);
    tmp = push_uint16_t(tmp, text->font_size);
    tmp = push_colour(tmp, text->colour);
    tmp = push_int32_t(tmp, text->x);
    tmp = push_int32_t(tmp, text->y);
    tmp = push_int16_t(tmp, text->wrap);
    send_from(ctx, buf, tmp-buf);
    if (ctx->err) return (AeStatusId){.status=0xFF};
    return recv_status_id(ctx);
}

AeStatus single_id_req(AeCtx *ctx, AeActionType type, AeId id) {
    const size_t len = 1 + sizeof(id);
    uint8_t buf[len];
    uint8_t *tmp = push_uint8_t(buf, type);
    tmp = push_id(tmp, id);
    send_from(ctx, buf, tmp-buf);
    if (ctx->err) return (AeStatus) 0xFF;
    return recv_status(ctx);
}

AeStatus double_id_req(AeCtx *ctx, AeActionType type, AeId id1, AeId id2) {
    const size_t len = 1 + sizeof(id1) + sizeof(id2);
    uint8_t buf[len];
    uint8_t *tmp = push_uint8_t(buf, type);
    tmp = push_id(tmp, id1);
    tmp = push_id(tmp, id2);
    send_from(ctx, buf, tmp-buf);
    if (ctx->err) return (AeStatus) 0xFF;
    return recv_status(ctx);
}

AeStatus ae_destroy_widget(AeCtx *ctx, AeId wid) {
    return single_id_req(ctx, AE_DESTROY_WIDGET, wid);
}

AeStatus ae_destroy_shape(AeCtx *ctx, AeId sid) {
    return single_id_req(ctx, AE_DESTROY_SHAPE, sid);
}

AeStatus ae_add_widget(AeCtx *ctx, AeId p_wid, AeId c_wid) {
    return double_id_req(ctx, AE_ADD_WIDGET, p_wid, c_wid);
}

AeStatus ae_rm_widget(AeCtx *ctx, AeId wid) {
    return single_id_req(ctx, AE_RM_WIDGET, wid);
}

AeStatus ae_rm_child(AeCtx *ctx, AeId p_wid, uint32_t c_index) {
    const size_t len = 1 + sizeof(p_wid) + sizeof(c_index);
    uint8_t buf[len];
    uint8_t *tmp = push_uint8_t(buf, AE_RM_CHILD);
    tmp = push_id(tmp, p_wid);
    tmp = push_uint32_t(tmp, c_index);
    send_from(ctx, buf, tmp-buf);
    if (ctx->err) return 0xFF;
    return recv_status(ctx);
}

AeStatus ae_draw_shape(AeCtx *ctx, AeId wid, AeId sid) {
    return double_id_req(ctx, AE_DRAW_SHAPE, wid, sid);
}

AeStatus ae_fill_canvas(AeCtx *ctx, AeId wid, AeColour colour) {
    const size_t len = 1 + sizeof(wid) + sizeof(colour);
    uint8_t buf[len];
    uint8_t *tmp = push_uint8_t(buf, AE_FILL_CANVAS);
    tmp = push_id(tmp, wid);
    tmp = push_colour(tmp, colour);
    send_from(ctx, buf, tmp-buf);
    if (ctx->err) return (AeStatus) 0xFF;
    return recv_status(ctx);
}

AeStatus ae_set_picture_data(AeCtx *ctx, AeId wid, uint32_t *pix, uint32_t pix_len) {
    size_t len = 1 + sizeof(wid) + sizeof(pix_len) + pix_len;
    uint8_t buf[len];
    uint8_t *tmp = push_uint8_t(buf, AE_SET_PICTURE_DATA);
    tmp = push_id(tmp, wid);
    tmp = push_uint32_t(tmp, pix_len);
    tmp = push_buf(tmp, (uint8_t *)pix, pix_len);
    send_from(ctx, buf, tmp-buf);
    if (ctx->err) return (AeStatus) 0xFF;
    return recv_status(ctx);
}

AeStatus ae_open_window(AeCtx *ctx, AeId wid) {
    return single_id_req(ctx, AE_OPEN_WINDOW, wid);
}

AeStatus ae_close_window(AeCtx *ctx, AeId wid) {
    return single_id_req(ctx, AE_CLOSE_WINDOW, wid);
}

AeStatus ae_set_str(AeCtx *ctx, AeId wid, char *str, uint16_t str_len) {
    const size_t len = 1 + sizeof(wid) + sizeof(str_len) + str_len;
    uint8_t buf[len];
    uint8_t *tmp = push_uint8_t(buf, AE_SET_STR);
    tmp = push_id(tmp, wid);
    tmp = push_str(tmp, str, str_len);
    send_from(ctx, buf, tmp-buf);
    if (ctx->err) return 0xFF;
    return recv_status(ctx);
}

AeStatus ae_add_event_handler(AeCtx *ctx, AeEventHandler *handler) {
    uint8_t code = code_from_event_type(handler->type);
    AeEventActionType action_type = handler->action_type;
    size_t len = 1 + sizeof(*handler);
    if (action_type == AE_EVENT_SET_PICTURE_DATA) {
        len += handler->action.spd.pix_len;
    } else if (action_type == AE_EVENT_SET_STR) {
        len += handler->action.ss.str_len;
    }
    uint8_t buf[len];
    uint8_t *tmp = push_uint8_t(buf, code);
    tmp = push_id(tmp, handler->wid);
    tmp = push_uint8_t(tmp, action_type);
    if (action_type == AE_EVENT_ADD_WIDGET) {
        tmp = push_id(tmp, handler->action.aw.p_wid);
        tmp = push_id(tmp, handler->action.aw.c_wid);
    } else if (action_type == AE_EVENT_RM_WIDGET) {
        tmp = push_id(tmp, handler->action.rw.wid);
    } else if (action_type == AE_EVENT_RM_CHILD) {
        tmp = push_id(tmp, handler->action.rc.p_wid);
        tmp = push_uint32_t(tmp, handler->action.rc.c_index);
    } else if (action_type == AE_EVENT_DRAW_SHAPE) {
        tmp = push_id(tmp, handler->action.ds.wid);
        tmp = push_id(tmp, handler->action.ds.sid);
    } else if (action_type == AE_EVENT_FILL_CANVAS) {
        tmp = push_id(tmp, handler->action.fc.wid);
        tmp = push_colour(tmp, handler->action.fc.colour);
    } else if (action_type == AE_EVENT_SET_PICTURE_DATA) {
        tmp = push_id(tmp, handler->action.spd.wid);
        tmp = push_uint32_t(tmp, handler->action.spd.pix_len);
        tmp = push_buf(tmp, (uint8_t *)handler->action.spd.pix, handler->action.spd.pix_len);
    } else if (action_type == AE_EVENT_OPEN_WINDOW) {
        tmp = push_id(tmp, handler->action.ow.wid);
    } else if (action_type == AE_EVENT_CLOSE_WINDOW) {
        tmp = push_id(tmp, handler->action.cw.wid);
    } else if (action_type == AE_EVENT_SET_STR) {
        tmp = push_id(tmp, handler->action.ss.wid);
        tmp = push_str(tmp, handler->action.ss.str, handler->action.ss.str_len);
    }
    send_from(ctx, buf, tmp-buf);
    if (ctx->err) return (AeStatus) 0xFF;
    return recv_status(ctx);
}

