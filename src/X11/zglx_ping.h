#ifndef ZGLX_PING_H
#define ZGLX_PING_H

#include <X11/Xlib.h>
#include "src/X11/zglx_macro_str.h"

#define HIDE_PINGS

#ifndef HIDE_PINGS

static inline void put_XAnyEvent(XAnyEvent const *const evt) {
    putchar('\n');
    fputs(     "                           Event\n", stdout);
    fputs(     "--------------------------------\n", stdout);
    printdf( "                           type: %!s\n", Event_Type_strs[evt->type]);
    lputdf( "                         serial", "du", evt->serial);
    //put_idBool("                     send_event", evt->send_event);
    //put_idvar( "                    display (*)", evt->display, p);
}

static inline void put_XKeyEvent(XKeyEvent const *const evt) {
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "                    root (hndl)", evt->root, lu);
    put_idvar(     "               subwindow (hndl)", evt->subwindow, lu);
    put_idvar(     "                           time", evt->time, lu);
    put_idvar(     "                              x", evt->x, i);
    put_idvar(     "                              y", evt->y, i);
    put_idvar(     "                         x_root", evt->x_root, i);
    put_idvar(     "                         y_root", evt->y_root, i);
    putn_idbitstr( "                          state", evt->state, 13);
    put_idvar(     "                        keycode", evt->keycode, u);
    put_idBool(    "                    same_screen", evt->same_screen);
}

static inline void put_XButtonEvent(XButtonEvent const *const evt) {
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "                    root (hndl)", evt->root, lu);
    put_idvar(     "               subwindow (hndl)", evt->subwindow, lu);
    put_idvar(     "                           time", evt->time, lu);
    put_idvar(     "                              x", evt->x, i);
    put_idvar(     "                              y", evt->y, i);
    put_idvar(     "                         x_root", evt->x_root, i);
    put_idvar(     "                         y_root", evt->y_root, i);
    putn_idbitstr( "                          state", evt->state, 13);
    put_idvar(     "                         button", Button_str[evt->button], s);
    put_idBool(    "                    same_screen", evt->same_screen);
}

static inline void put_XMotionEvent(XMotionEvent const *const evt) {
    lputdf(     "                  window (hndl)", "du", evt->window);
    lputdf(     "                    root (hndl)", "du", evt->root);
    lputdf(     "               subwindow (hndl)", "du", evt->subwindow);
    lputdf(     "                           time", "du", evt->time);
    lputdf(     "                              x", "di", evt->x);
    lputdf(     "                              y", "di", evt->y);
    lputdf(     "                         x_root", "di", evt->x_root);
    lputdf(     "                         y_root", "di", evt->y_root);
    lputdf(     "                          state", "=.13bu", evt->state);
    lputdf(     "                        is_hint", "di", evt->is_hint);
    //put_idBool(    "                    same_screen", evt->same_screen);
}

static inline void put_XCrossingEvent(XCrossingEvent const *const evt) {
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "                    root (hndl)", evt->root, lu);
    put_idvar(     "               subwindow (hndl)", evt->subwindow, lu);
    put_idvar(     "                           time", evt->time, lu);
    put_idvar(     "                              x", evt->x, i);
    put_idvar(     "                              y", evt->y, i);
    put_idvar(     "                         x_root", evt->x_root, i);
    put_idvar(     "                         y_root", evt->y_root, i);
    put_idvar(     "                           mode", NotifyMode_str[evt->mode], s);
    put_idvar(     "                        is_hint", NotifyDetail_str[evt->detail], s);
    put_idBool(    "                    same_screen", evt->same_screen);
    put_idBool(    "                          focus", evt->focus);
    putn_idbitstr( "                          state", evt->state, 13);
}

static inline void put_XFocusChangeEvent(XFocusChangeEvent const *const evt) {
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "                           mode", NotifyMode_str[evt->mode], s);
    put_idvar(     "                         detail", NotifyDetail_str[evt->detail], s);
}

static inline void put_XExposeEvent(XExposeEvent const *const evt) {
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "                              x", evt->x, i);
    put_idvar(     "                              y", evt->y, i);
    put_idvar(     "                          width", evt->width, i);
    put_idvar(     "                         height", evt->height, i);
    put_idvar(     "                          count", evt->count, i);
}

static inline void put_XGraphicsExposeEvent(XGraphicsExposeEvent const *const evt) {
    (void)evt;
}

static inline void put_XNoExposeEvent(XNoExposeEvent const *const evt) {
    (void)evt;
}

static inline void put_XVisibilityEvent(XVisibilityEvent const *const evt) {
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "                          state", Visibility_str[evt->state], s);
}

static inline void put_XCreateWindowEvent(XCreateWindowEvent const *const evt) {
    put_idvar(     "                  parent (hndl)", evt->parent, lu);
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "                              x", evt->x, i);
    put_idvar(     "                              y", evt->y, i);
    put_idvar(     "                          width", evt->width, i);
    put_idvar(     "                         height", evt->height, i);
    put_idBool(    "                   border_width", evt->override_redirect);
}

static inline void put_XDestroyWindowEvent(XDestroyWindowEvent const *const evt) {
    put_idvar(     "                   event (hndl)", evt->event, lu);
    put_idvar(     "                  window (hndl)", evt->window, lu);
}

static inline void put_XUnmapEvent(XUnmapEvent const *const evt) {
    put_idvar(     "                   event (hndl)", evt->event, lu);
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idBool(    "                 from_configure", evt->from_configure);
}

static inline void put_XMapEvent(XMapEvent const *const evt) {
    put_idvar(     "                   event (hndl)", evt->event, lu);
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idBool(    "              override_redirect", evt->override_redirect);
}

static inline void put_XMapRequestEvent(XMapRequestEvent const *const evt) {
    put_idvar(     "                  parent (hndl)", evt->parent, lu);
    put_idvar(     "                  window (hndl)", evt->window, lu);
}

static inline void put_XReparentEvent(XReparentEvent const *const evt) {
    put_idvar(     "                   event (hndl)", evt->event, lu);
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "                  parent (hndl)", evt->parent, lu);
    put_idvar(     "                              x", evt->x, i);
    put_idvar(     "                              y", evt->y, i);
    put_idBool(    "              override_redirect", evt->override_redirect);
}

static inline void put_XConfigureEvent(XConfigureEvent const *const evt) {
    put_idvar(     "                   event (hndl)", evt->event, lu);
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "                              x", evt->x, i);
    put_idvar(     "                              y", evt->y, i);
    put_idvar(     "                          width", evt->width, i);
    put_idvar(     "                         height", evt->height, i);
    put_idvar(     "                   border_width", evt->border_width, i);
    put_idvar(     "                   above (hndl)", evt->above, lu);
    put_idBool(    "              override_redirect", evt->override_redirect);
}

static inline void put_XGravityEvent(XGravityEvent const *const evt) {
    put_idvar(     "                   event (hndl)", evt->event, lu);
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "                              x", evt->x, i);
    put_idvar(     "                              y", evt->y, i);
}

static inline void put_XResizeRequestEvent(XResizeRequestEvent const *const evt) {
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "                          width", evt->width, i);
    put_idvar(     "                         height", evt->height, i);
}

static inline void put_XConfigureRequestEvent(XConfigureRequestEvent const *const evt) {
    put_idvar(     "                  parent (hndl)", evt->parent, lu);
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "                              x", evt->x, i);
    put_idvar(     "                              y", evt->y, i);
    put_idvar(     "                          width", evt->width, i);
    put_idvar(     "                         height", evt->height, i);
    put_idvar(     "                   border_width", evt->border_width, i);
    put_idvar(     "                   above (hndl)", evt->above, lu);
    put_idvar(     "                         detail", evt->detail, i);
    putn_idbitstr( "                          state", evt->value_mask, 13);
}

static inline void put_XCirculateEvent(XCirculateEvent const *const evt) {
    put_idvar(     "                   event (hndl)", evt->event, lu);
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "                          place", Circulation_str[evt->place], s);
}

static inline void put_XCirculateRequestEvent(XCirculateRequestEvent const *const evt) {
    put_idvar(     "                  parent (hndl)", evt->parent, lu);
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "                          place", Circulation_str[evt->place], s);
}

static inline void put_XPropertyEvent(XPropertyEvent const *const evt) {
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "                    atom (Atom)", evt->atom, lu);
    put_idvar(     "                           time", evt->time, lu);
    put_idvar(     "                          state", Property_str[evt->state], s);
}

static inline void put_XSelectionClearEvent(XSelectionClearEvent const *const evt) {
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "               selection (Atom)", evt->selection, lu);
    put_idvar(     "                           time", evt->time, lu);
}

static inline void put_XSelectionRequestEvent(XSelectionRequestEvent const *const evt) {
    put_idvar(     "                   owner (hndl)", evt->owner, lu);
    put_idvar(     "               requestor (hndl)", evt->requestor, lu);
    put_idvar(     "               selection (Atom)", evt->selection, lu);
    put_idvar(     "                  target (Atom)", evt->target, lu);
    put_idvar(     "                property (Atom)", evt->property, lu);
    put_idvar(     "                           time", evt->time, lu);
}

static inline void put_XSelectionEvent(XSelectionEvent const *const evt) {
    put_idvar(     "               requestor (hndl)", evt->requestor, lu);
    put_idvar(     "               selection (Atom)", evt->selection, lu);
    put_idvar(     "                  target (Atom)", evt->target, lu);
    put_idvar(     "                property (Atom)", evt->property, lu);
    put_idvar(     "                           time", evt->time, lu);
}

static inline void put_XColormapEvent(XColormapEvent const *const evt) {
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "                colormap (hndl)", evt->colormap, lu);
    put_idBool(    "                            new", evt->new);
    put_idvar(     "                          state", ColorMap_str[evt->state], s);
}

static inline void put_XClientMessageEvent(XClientMessageEvent const *const evt) {
    put_idvar(     "                  window (hndl)", evt->window, lu);
    put_idvar(     "            message_type (Atom)", evt->message_type, lu);
    put_idvar(     "                         format", evt->format, i);
}

static inline void put_XErrorEvent(XErrorEvent const *const evt) {
    (void)evt;
}

static inline void put_XKeymapEvent(XKeymapEvent const *const evt) {
    put_idvar( "                  window (hndl)", evt->window, lu);
}

static inline void put_XMappingEvent(XMappingEvent const *const evt) {
    put_idvar( "                  window (hndl)", evt->window, lu);
	put_idvar( "                        request", Mapping_str[evt->request], s);
	put_idvar( "                  first_keycode", evt->first_keycode, i);
	put_idvar( "                          count", evt->count, count);
}
#endif

#ifdef HIDE_PINGS
#define put_XAnyEvent(evt) (void)evt
#define put_XKeyEvent(evt) (void)evt
#define put_XButtonEvent(evt) (void)evt
#define put_XMotionEvent(evt) (void)evt
#define put_XCrossingEvent(evt) (void)evt
#define put_XFocusChangeEvent(evt) (void)evt
#define put_XExposeEvent(evt) (void)evt
#define put_XGraphicsExposeEvent(evt) (void)evt
#define put_XNoExposeEvent(evt) (void)evt
#define put_XVisibilityEvent(evt) (void)evt
#define put_XCreateWindowEvent(evt) (void)evt
#define put_XDestroyWindowEvent(evt) (void)evt
#define put_XUnmapEvent(evt) (void)evt
#define put_XMapEvent(evt) (void)evt
#define put_XMapRequestEvent(evt) (void)evt
#define put_XReparentEvent(evt) (void)evt
#define put_XConfigureEvent(evt) (void)evt
#define put_XGravityEvent(evt) (void)evt
#define put_XResizeRequestEvent(evt) (void)evt
#define put_XConfigureRequestEvent(evt) (void)evt
#define put_XCirculateEvent(evt) (void)evt
#define put_XPropertyEvent(evt) (void)evt
#define put_XSelectionClearEvent(evt) (void)evt
#define put_XSelectionRequestEvent(evt) (void)evt
#define put_XSelectionEvent(evt) (void)evt
#define put_XColormapEvent(evt) (void)evt
#define put_XClientMessageEvent(evt) (void)evt
#define put_XErrorEvent(evt) (void)evt
#define put_XKeymapEvent(evt) (void)evt
#define put_XMappingEvent(evt) (void)evt
#endif

#endif /* ZGLX_PING_H */
