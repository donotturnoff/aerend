#include <sys/socket.h>
#include <arpa/inet.h>
#include "libaerend.h"

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

// TODO: from -> to
static inline uint8_t code_from_event_type(uint8_t type) {
    return type + 0x11;
}

static inline uint32_t colour_to_int(AeColour colour) {
    return (colour.r << 24) | (colour.g << 16) | (colour.b << 8) | (colour.a);
}

// TODO: hide AeCtx implementation?
AeCtx ae_init(int sock, AeEvent *evbuf, size_t evbuf_len) {
    return (AeCtx){.sock = sock, .err = AE_NO_ERR, .evbuf_len = evbuf_len, .first_ev = -1, .last_ev = -1, .evbuf = evbuf};
}

void recv_into(AeCtx *ctx, void *buf, size_t len) {
    ssize_t bytes = recv(ctx->sock, buf, len, 0);
    if (bytes < 0) {
        ctx->err |= AE_SOCK_ERR;
    } else if (bytes == 0) {
        ctx->err |= AE_SOCK_CLOSED;
    }
}

void send_from(AeCtx *ctx, const void *buf, size_t len) {
    ssize_t bytes = send(ctx->sock, buf, len, 0);
    if (bytes < 0) {
        ctx->err |= AE_SOCK_ERR;
    }
}

void process_event(AeCtx *ctx, uint8_t type) {
    uint8_t buf[sizeof(AeEvent)-1];
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

AeStatus recv_status(AeCtx *ctx) {
    uint8_t status;
    // TODO: prevent infinite loop if server fails to send response
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
    // TODO: prevent infinite loop if server fails to send response
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
        return NULL; // TODO: set ctx->err ?
    }
}

AeEvent *ae_peek_event(AeCtx *ctx) {
    if (ctx->first_ev > -1) {
        return ctx->evbuf + ctx->first_ev;
    } else {
        return NULL;
    }
}

void ae_pop_event(AeCtx *ctx) {
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

AeStatusId ae_make_window(AeCtx *ctx, uint8_t args, AeWindow *win) {
    // TODO: client side input validation?
    const size_t max_len = sizeof(uint8_t) + sizeof(args) + sizeof(win->x) + sizeof(win->y) + sizeof(win->w) + sizeof(win->h) + sizeof(win->title_len);
    uint8_t buf[max_len];
    buf[0] = (uint8_t) AE_MAKE_WINDOW;
    buf[sizeof(uint8_t)] = args;
    size_t len = sizeof(uint8_t) + sizeof(args);
    if (args & 0x01) {
        *((int16_t *)(buf+len)) = htons(win->x);
        len += sizeof(win->x);
    }
    if (args & 0x02) {
        *((int16_t *)(buf+len)) = htons(win->y);
        len += sizeof(win->y);
    }
    if (args & 0x04) {
        *((int16_t *)(buf+len)) = htons(win->w);
        len += sizeof(win->w);
    }
    if (args & 0x08) {
        *((int16_t *)(buf+len)) = htons(win->h);
        len += sizeof(win->h);
    }
    if (args & 0x10) {
        *((size_t *)(buf+len)) = htons(win->title_len);
        len += sizeof(win->title_len);
    }
    send_from(ctx, &buf, len);
    if (ctx->err) return (AeStatusId){.status=0xFF};
    if (args & 0x10) {
        send_from(ctx, win->title, win->title_len);
        if (ctx->err) return (AeStatusId){.status=0xFF};
    }
    return recv_status_id(ctx);
}

AeStatus ae_open_window(AeCtx *ctx, AeId wid) {
    const size_t len = 1+sizeof(wid);
    uint8_t buf[len];
    buf[0] = AE_OPEN_WINDOW;
    *(AeId *)(buf+1) = htonl(wid);
    send_from(ctx, buf, len);
    if (ctx->err) return (AeStatus) 0xFF;
    return recv_status(ctx);
}

AeStatus ae_close_window(AeCtx *ctx, AeId wid) {
    const size_t len = 1+sizeof(wid);
    uint8_t buf[len];
    buf[0] = AE_CLOSE_WINDOW;
    *(AeId *)(buf+1) = htonl(wid);
    send_from(ctx, buf, len);
    if (ctx->err) return (AeStatus) 0xFF;
    return recv_status(ctx);
}

AeStatus ae_add_event_handler(AeCtx *ctx, AeEventHandler *handler) {
    uint8_t code = code_from_event_type(handler->type);
    AeEventActionType action_type = handler->action_type;
    const size_t max_len = sizeof(code) + sizeof(handler->wid) + sizeof(handler->action);
    uint8_t buf[max_len];
    buf[0] = code;
    size_t len = sizeof(code);
    *((AeId *)(buf+len)) = htonl(handler->wid);
    len += sizeof(handler->wid);
    buf[len] = (uint8_t) action_type;
    len += sizeof(uint8_t);
    if (action_type == AE_EVENT_ADD_WIDGET) {
        *((AeId *)(buf+len)) = htonl(handler->action.aw.p_wid);
        len += sizeof(handler->action.aw.p_wid);
        *((AeId *)(buf+len)) = htonl(handler->action.aw.c_wid);
        len += sizeof(handler->action.aw.c_wid);
    } else if (action_type == AE_EVENT_RM_WIDGET) {
        *((AeId *)(buf+len)) = htonl(handler->action.rw.wid);
        len += sizeof(handler->action.rw.wid);
    } else if (action_type == AE_EVENT_DRAW_SHAPE) {
        *((AeId *)(buf+len)) = htonl(handler->action.ds.wid);
        len += sizeof(handler->action.ds.wid);
        *((AeId *)(buf+len)) = htonl(handler->action.ds.sid);
        len += sizeof(handler->action.ds.wid);
    } else if (action_type == AE_EVENT_FILL_CANVAS) {
        *((AeId *)(buf+len)) = htonl(handler->action.fc.wid);
        len += sizeof(handler->action.fc.wid);
        *((uint32_t *)(buf+len)) = colour_to_int(handler->action.fc.colour);
        len += sizeof(handler->action.fc.colour);
    } else if (action_type == AE_EVENT_SET_PICTURE_DATA) {
        *((AeId *)(buf+len)) = htonl(handler->action.spd.wid);
        len += sizeof(handler->action.spd.wid);
    } else if (action_type == AE_EVENT_OPEN_WINDOW) {
        *((AeId *)(buf+len)) = htonl(handler->action.ow.wid);
        len += sizeof(handler->action.ow.wid);
    } else if (action_type == AE_EVENT_CLOSE_WINDOW) {
        *((AeId *)(buf+len)) = htonl(handler->action.cw.wid);
        len += sizeof(handler->action.cw.wid);
    }
    // TODO: send -> send_from
    send_from(ctx, buf, len);
    if (ctx->err) return (AeStatus) 0xFF;
    if (action_type == AE_EVENT_SET_PICTURE_DATA) {
        send_from(ctx, handler->action.spd.pix, handler->action.spd.pix_len);
        if (ctx->err < 0) return (AeStatus) 0xFF;
    }
    return recv_status(ctx);
}

