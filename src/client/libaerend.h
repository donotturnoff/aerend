#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

typedef uint8_t AeStatus;
typedef uint32_t AeId;

typedef uint32_t AeColour;

typedef struct ae_border_t {
    AeColour colour;
    uint16_t t;
} AeBorder;

typedef uint16_t AeMargin;

typedef uint16_t AePadding;

typedef enum ae_layout_type_t {
    AE_GRID_LAYOUT, AE_IRREGULAR_GRID_LAYOUT
} AeLayoutType;

typedef struct ae_grid_layout_t {
    int16_t cols, rows;
} AeGridLayout;

typedef struct ae_irregular_grid_layout_t {
    int16_t col_count, row_count;
    uint8_t *cols, *rows;
} AeIrregularGridLayout;

typedef union ae_layout_value_t {
    AeGridLayout gl;
    AeIrregularGridLayout igl;
} AeLayoutValue;

typedef struct ae_layout_t {
    AeLayoutType type;
    AeLayoutValue value;
} AeLayout;

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
    AE_NO_ERR=0, AE_SOCK_ERR=1, AE_SOCK_CLOSED=2, AE_EVBUF_FULL=4, AE_INVALID_ARG=8
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
    uint16_t title_len;
} AeWindow;

typedef struct ae_panel_t {
    AeLayout layout;
    AeColour bg_colour;
    AeBorder border;
    AeMargin margin;
    AePadding padding;
} AePanel;

typedef struct ae_button_t {
    const char *str;
    uint16_t str_len;
    const char *font_path;
    uint16_t font_path_len;
    uint16_t font_size;
    AeColour colour, bg_colour;
    AeBorder border;
    AeMargin margin;
    AePadding padding;
    uint16_t wrap;
} AeButton;

typedef struct ae_label_t {
    const char *str;
    uint16_t str_len;
    const char *font_path;
    uint16_t font_path_len;
    uint16_t font_size;
    AeColour colour, bg_colour;
    AeBorder border;
    AeMargin margin;
    AePadding padding;
    uint16_t wrap;
} AeLabel;

typedef struct ae_canvas_t {
    uint16_t w, h;
} AeCanvas;

typedef struct ae_picture_t {
    uint16_t w, h;
    uint8_t *pix;
} AePicture;

typedef struct ae_rectangle_t {
    int16_t x, y;
    uint16_t w, h;
    AeColour colour;
    AeBorder border;
} AeRectangle;

typedef struct ae_ellipse_t {
    int16_t x, y;
    uint16_t w, h;
    AeColour colour;
    AeBorder border;
} AeEllipse;

typedef struct ae_line_t {
    int16_t x0, y0, x1, y1;
    AeColour colour;
} AeLine;

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

// TODO: inline
AeColour ae_colour_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
AeColour ae_colour_rgb(uint8_t r, uint8_t g, uint8_t b);

AeStatusId ae_make_window(AeCtx *ctx, uint8_t args, AeWindow *win);
AeStatusId ae_make_panel(AeCtx *ctx, uint8_t args, AePanel *pnl);
AeStatusId ae_make_button(AeCtx *ctx, uint8_t args, AeButton *btn);
AeStatusId ae_make_label(AeCtx *ctx, uint8_t args, AeLabel *lbl);
AeStatusId ae_make_canvas(AeCtx *ctx, AeCanvas *cvs);
AeStatusId ae_make_picture(AeCtx *ctx, uint8_t args, AePicture *pic);
AeStatusId ae_make_rectangle(AeCtx *ctx, uint8_t args, AeRectangle *rect);
AeStatusId ae_make_ellipse(AeCtx *ctx, uint8_t args, AeEllipse *ellp);
AeStatusId ae_make_line(AeCtx *ctx, AeLine *line);
AeStatus ae_destroy_widget(AeCtx *ctx, AeId wid);
AeStatus ae_destroy_shape(AeCtx *ctx, AeId sid);
AeStatus ae_add_widget(AeCtx *ctx, AeId p_wid, AeId c_wid);
AeStatus ae_rm_widget(AeCtx *ctx, AeId wid);
AeStatus ae_draw_shape(AeCtx *ctx, AeId wid, AeId sid);
AeStatus ae_fill_canvas(AeCtx *ctx, AeId wid, AeColour colour);
AeStatus ae_set_picture_data(AeCtx *ctx, AeId wid, uint8_t *pix, uint32_t pix_len);
AeStatus ae_open_window(AeCtx *ctx, AeId wid);
AeStatus ae_close_window(AeCtx *ctx, AeId wid);
AeStatus ae_add_event_handler(AeCtx *ctx, AeEventHandler *handler);

