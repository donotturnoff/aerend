#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

typedef uint8_t AeStatus;
typedef uint32_t AeId;

typedef struct ae_colour_t {
    uint8_t r, g, b, a;
} AeColour;

typedef enum ae_event_type_t {
    AE_HALT, AE_KEY_PRESS, AE_KEY_RELEASE, AE_KEY_TYPE, AE_MOUSE_PRESS, AE_MOUSE_RELEASE, AE_MOUSE_CLICK, AE_MOUSE_MOVE, AE_MOUSE_SCROLL, AE_ACTION, AE_MOUSE_ENTER, AE_MOUSE_EXIT
} AeEventType;

typedef struct ae_halt_event_t {
    AeEventType type;
    AeId wid;
} AeHaltEvent; // TODO: make server actually send this

typedef struct ae_key_press_event_t {
    AeEventType type;
    AeId wid;
    uint8_t keys;
    char c;
} AeKeyPressEvent;

typedef struct ae_key_release_event_t {
    AeEventType type;
    AeId wid;
    uint8_t keys;
    char c;
} AeKeyReleaseEvent;

typedef struct ae_key_type_event_t {
    AeEventType type;
    AeId wid;
    uint8_t keys;
    char c;
} AeKeyTypeEvent;

typedef struct ae_mouse_press_event_t {
    AeEventType type;
    AeId wid;
    uint8_t buttons;
} AeMousePressEvent;

typedef struct ae_mouse_release_event_t {
    AeEventType type;
    AeId wid;
    uint8_t buttons;
} AeMouseReleaseEvent;

typedef struct ae_mouse_click_event_t {
    AeEventType type;
    AeId wid;
    uint8_t buttons;
} AeMouseClickEvent;

typedef struct ae_mouse_move_event_t {
    AeEventType type;
    AeId wid;
    uint8_t buttons;
    int16_t dx, dy;
} AeMouseMoveEvent;

typedef struct ae_mouse_scroll_event_t {
    AeEventType type;
    AeId wid;
    uint8_t buttons;
    int16_t dx, dy;
} AeMouseScrollEvent;

typedef struct ae_action_event_t {
    AeEventType type;
    AeId wid;
} AeActionEvent;

typedef struct ae_mouse_enter_event_t {
    AeEventType type;
    AeId wid;
} AeMouseEnterEvent;

typedef struct ae_mouse_exit_event_t {
    AeEventType type;
    AeId wid;
} AeMouseExitEvent;

// TODO: move type and wid into wrapper struct
typedef union ae_event_t {
    AeHaltEvent h;
    AeKeyPressEvent kp;
    AeKeyReleaseEvent kr;
    AeKeyTypeEvent kt;
    AeMousePressEvent mp;
    AeMouseReleaseEvent mr;
    AeMouseClickEvent mc;
    AeMouseMoveEvent mm;
    AeMouseScrollEvent ms;
    AeActionEvent a;
    AeMouseEnterEvent men;
    AeMouseExitEvent mex;
} AeEvent;

typedef enum ae_err_t {
    AE_NO_ERR=0, AE_SOCK_ERR=1, AE_SOCK_CLOSED=2, AE_EVBUF_FULL=4
} AeErr;

typedef struct ae_ctx_t {
    int sock;
    AeErr err;
    AeEvent *evbuf;
    size_t evbuf_len;
    ssize_t first_ev, last_ev;
} AeCtx;

typedef struct ae_status_id_t {
    AeStatus status;
    AeId id;
} AeStatusId;

typedef struct ae_window_t {
    int16_t x, y, w, h;
    const char *title;
    size_t title_len;
} AeWindow;

typedef enum ae_event_action_type {
    AE_EVENT_NOTIFY_CLIENT, AE_EVENT_ADD_WIDGET, AE_EVENT_RM_WIDGET, AE_EVENT_DRAW_SHAPE, AE_EVENT_FILL_CANVAS, AE_EVENT_SET_PICTURE_DATA, AE_EVENT_OPEN_WINDOW, AE_EVENT_CLOSE_WINDOW
} AeEventActionType;

typedef struct ae_event_add_widget_t {
    AeId p_wid, c_wid;
} AeEventAddWidget;

typedef struct ae_event_rm_widget_t {
    AeId wid;
} AeEventRmWidget;

typedef struct ae_event_draw_shape_t {
    AeId wid;
    AeId sid;
} AeEventDrawShape;

typedef struct ae_event_fill_canvas_t {
    AeId wid;
    AeColour colour;
} AeEventFillCanvas;

typedef struct ae_event_set_picture_data_t {
    AeEventActionType type;
    AeId wid;
    uint32_t* pix;
    size_t pix_len;
} AeEventSetPictureData;

typedef struct ae_event_open_window_t {
    AeId wid;
} AeEventOpenWindow;

typedef struct ae_event_close_window_t {
    AeId wid;
} AeEventCloseWindow;

typedef union ae_event_action_t {
    AeEventAddWidget aw;
    AeEventRmWidget rw;
    AeEventDrawShape ds;
    AeEventFillCanvas fc;
    AeEventSetPictureData spd;
    AeEventOpenWindow ow;
    AeEventCloseWindow cw;
} AeEventAction;

typedef struct ae_event_handler_t {
    AeEventType type;
    AeId wid;
    AeEventActionType action_type;
    AeEventAction action;
} AeEventHandler;

typedef enum ae_action_type_t {
    AE_MAKE_WINDOW, AE_MAKE_PANEL, AE_MAKE_BUTTON, AE_MAKE_LABEL, AE_MAKE_CANVAS, AE_MAKE_PICTURE,
    AE_MAKE_RECTANGLE, AE_MAKE_ELLIPSE, AE_MAKE_LINE,
    AE_DESTROY_WIDGET, AE_DESTROY_SHAPE,
    AE_ADD_WIDGET, AE_RM_WIDGET, AE_DRAW_SHAPE, AE_FILL_CANVAS, AE_SET_PICTURE_DATA,
    AE_OPEN_WINDOW, AE_CLOSE_WINDOW
} AeActionType;

AeCtx ae_init(int sock, AeEvent *evbuf, size_t evbuf_len);
AeEvent *ae_recv_event(AeCtx *ctx);
AeEvent *ae_peek_event(AeCtx *ctx);
void ae_pop_event(AeCtx *ctx);

AeStatusId ae_make_window(AeCtx *ctx, uint8_t args, AeWindow *win);
AeStatus ae_add_widget(AeCtx *ctx, AeId p_wid, AeId c_wid);
AeStatus ae_open_window(AeCtx *ctx, AeId wid);
AeStatus ae_close_window(AeCtx *ctx, AeId wid);
AeStatus ae_add_event_handler(AeCtx *ctx, AeEventHandler *handler);

