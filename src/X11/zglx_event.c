#include <stdlib.h> // just for exit()
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/XKBlib.h> // for XkbSetDetectableAutoRepeat()
#include <stdio.h>

#include "src/zigil_input.h"
#include "src/zigil_event.h"
#include "src/X11/zglx.h"
//#include "src/X11/zglx_ping.h"
#include "src/X11/zglx_event.h"
#include "src/X11/zglx_ZK_from_XK.h"

#define temp(f, m, w) (w) ^= (-(f) ^ (w)) & (m)

/* -------------------------------------------------------------- */
/* Zigil API functions implemented here; also declared in zigil.h */
/* -------------------------------------------------------------- */
extern zgl_Result zgl_UpdateEventQueue(void);
extern void zgl_SetCursor(zgl_CursorCode code);
extern void zgl_SetPointerCapture(void);
extern void zgl_ReleasePointerCapture(void);
extern void zgl_SetRelativePointer(void);
extern void zgl_ReleaseRelativePointer(zgl_Pixel *pos);
extern void zgl_SetPointerPos(int x, int y);
extern zgl_Pixel zgl_GetPointerPos(void);


/* --------- */
/* Internals */
/* --------- */

extern zgl_Result _zgl_WaitUpdateEventQueue(void); // internal to zigil, but not
                                                   // system-specific
static int upscale_center_x = 0, upscale_center_y = 0;
static int upscale_curr_x = 0, upscale_curr_y = 0;
static int upscale_last_x = 0, upscale_last_y = 0;
static Cursor cursors[NUM_ZC];
static Cursor get_empty_cursor(Display *d, Window w);
static void IgnoreMotionNotify_handler(XMotionEvent *xevt);
static void RelativeMotionNotify_handler(XMotionEvent *xevt);
static void AbsoluteMotionNotify_handler(XMotionEvent *xevt);
void (*MotionNotify_handler)(XMotionEvent *xevt) = AbsoluteMotionNotify_handler;
    

//static zgl_Result do_next_XEvent(void);
static zgl_Result wait_next_XEvent(void);
static void handle_XEvent(XEvent *xevt);

static void analyze_keycodes_and_keysyms(void);


/* ----------- */
/* Definitions */
/* ----------- */

zgl_Result init_XEvent(void) {
    analyze_keycodes_and_keysyms();
    
    init_LK_from_XK();

    // no free required
    cursors[ZC_arrow]     = XCreateFontCursor(x11_info.display, XC_left_ptr);
    cursors[ZC_drag]      = XCreateFontCursor(x11_info.display, XC_fleur);
    cursors[ZC_crosshair] = XCreateFontCursor(x11_info.display, XC_crosshair);
    cursors[ZC_plus]      = XCreateFontCursor(x11_info.display, XC_plus);
    cursors[ZC_cross]     = XCreateFontCursor(x11_info.display, XC_cross);
    cursors[ZC_text]      = XCreateFontCursor(x11_info.display, XC_xterm);
    cursors[ZC_empty]     = get_empty_cursor(x11_info.display, x11_info.win_info.win);

    // TODO: Consider rolling this on my own to avoid the dependency of Xlib KB
    // extension
    XkbSetDetectableAutoRepeat(x11_info.display, True, NULL);
    
    return ZR_SUCCESS;
}

zgl_Result term_XEvent(void) {
    if (cursors[ZC_arrow] != None)
        XFreeCursor(x11_info.display, cursors[ZC_arrow]);

    if (cursors[ZC_drag] != None)
        XFreeCursor(x11_info.display, cursors[ZC_drag]);

    if (cursors[ZC_crosshair] != None)
        XFreeCursor(x11_info.display, cursors[ZC_crosshair]);

    if (cursors[ZC_plus] != None)
        XFreeCursor(x11_info.display, cursors[ZC_plus]);

    if (cursors[ZC_cross] != None)
        XFreeCursor(x11_info.display, cursors[ZC_cross]);

    if (cursors[ZC_text] != None)
        XFreeCursor(x11_info.display, cursors[ZC_text]);

    if (cursors[ZC_empty] != None)
        XFreeCursor(x11_info.display, cursors[ZC_empty]);
    
    return ZR_SUCCESS;
}

zgl_Result zgl_UpdateEventQueue(void) {
    XEvent xevt;

    while (XPending(x11_info.display)) {
        XNextEvent(x11_info.display, &xevt); // blocks, hence pending above
        handle_XEvent(&xevt);
    }

    return ZR_SUCCESS;
}

zgl_Result _zgl_WaitUpdateEventQueue(void) {
    XEvent xevt;

    wait_next_XEvent(); // this blocks until an event actually happens
    while (XPending(x11_info.display)) {
        XNextEvent(x11_info.display, &xevt);
        handle_XEvent(&xevt);
    }

    return ZR_SUCCESS;
}

/*
static zgl_Result do_next_XEvent(void) {
    XEvent xevt;
    
    if (XPending(x11_info.display)) {
        XNextEvent(x11_info.display, &xevt);
        handle_XEvent(&xevt);
    }

    return ZR_SUCCESS;
}
*/


static zgl_Result wait_next_XEvent(void) {
    XEvent xevt;

    XNextEvent(x11_info.display, &xevt); // blocks
    handle_XEvent(&xevt);

    return ZR_SUCCESS;
}




/* ------- */
/* Cursors */
/* ------- */

static Cursor get_empty_cursor(Display *d, Window w) {
    static Cursor cursor = 0;
    static char const data[16] = { 0 };

    if (!cursor) { // only create once
        XColor bg;
        Pixmap pixmap;

        bg.red = bg.green = bg.blue = 0x0000;
        pixmap = XCreateBitmapFromData(d, w, data, 4, 4);
        if (pixmap) {
            cursor = XCreatePixmapCursor(d, pixmap, pixmap, &bg, &bg, 0, 0);
            XFreePixmap(d, pixmap);
        }
    }
    
    return cursor;
}
/*
void zgl_SetPointerCapture(void) {
    SetCapture(x11_info.display);
}

void zgl_ReleasePointerCapture(void) {
    ReleaseCapture();
}
*/
void zgl_SetPointerPos(int x, int y) {
    XWarpPointer(x11_info.display, None,
                 x11_info.win_info.win, 0, 0, 0, 0,
                 x<<fb_info.upscale_shift,
                 y<<fb_info.upscale_shift);
}

zgl_Pixel zgl_GetPointerPos(void) {
    return (zgl_Pixel){upscale_curr_x>>fb_info.upscale_shift,
                       upscale_curr_y>>fb_info.upscale_shift};
}

void zgl_SetCursor(zgl_CursorCode code) {
    XDefineCursor(x11_info.display, x11_info.win_info.win, cursors[code]);
}
void zgl_SetRelativePointer(void) {
    /**
       This is a mess. A major change of mouse mode. We want to cleanly change
       the state of the mouse so that it is located at the window center.

       1) Hide the cursor.

       2) Change mouse motion handler to the one which ignores motion until the
       pointer is at the center of the screne (see item (5) for explanation).
       
       3) Update the window's center coordinate based on width and height (which
       may have changed).
       
       4) Change the position of the pointer to the window center.
       
       5) IMPORTANT: Prepare to ignore any pending MotionNotifyEvents from the X
       server until we receive the motion event triggered by step (4). For now
       we just set a flag, and only unset it when a motion event is read whose
       coordinates are the window center. But this is imperfect: it is unlikely
       and yet *possible* that there are pending motion events at the window
       center before the one we deliberately sent.

       6) Confine the pointer to the window (TODO: is this necessary?). If the
       window is very very small then some motion might get clipped.
    */

    zgl_SetCursor(ZC_empty);
    MotionNotify_handler = IgnoreMotionNotify_handler;
    upscale_center_x = x11_info.width/2;
    upscale_center_y = x11_info.height/2;
    upscale_last_x = upscale_center_x;
    upscale_last_y = upscale_center_y;
    XWarpPointer(x11_info.display, None,
                 x11_info.win_info.win, 0, 0, 0, 0,
                 upscale_center_x,
                 upscale_center_y);
    
    // similar to Windows' ClipCursor, we confine pointer to window
    XGrabPointer(x11_info.display, x11_info.win_info.win,
                 True, // owner_events
                 ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask,
                 GrabModeAsync, // pointer_mode
                 GrabModeAsync, // keyboard_mode
                 x11_info.win_info.win, // confine_to
                 None, // cursor
                 CurrentTime);
}
void zgl_ReleaseRelativePointer(zgl_Pixel *pos /* App coordinates */) {
    int dest_x, dest_y;
    if (pos) {
        dest_x = pos->x << fb_info.upscale_shift;
        dest_y = pos->y << fb_info.upscale_shift;
    }
    else {
        dest_x = upscale_center_x;
        dest_y = upscale_center_y;
    }
    
    XUngrabPointer(x11_info.display, CurrentTime);
    XWarpPointer(x11_info.display, None,
                 x11_info.win_info.win, 0, 0, 0, 0,
                 dest_x,
                 dest_y);
    upscale_last_x = upscale_curr_x = dest_x;
    upscale_last_y = upscale_curr_y = dest_y;
    MotionNotify_handler = AbsoluteMotionNotify_handler;
    zgl_SetCursor(ZC_arrow);    
}



static void KeyPress_handler(XKeyPressedEvent *xevt) {
    //put_XKeyEvent(&evt_union->xkey);
    
    zgl_Event zevt;
    zevt.type = EC_KeyPress;

    zevt.u.key_press.zk = ZK_from_XK(XLookupKeysym(xevt, 0));
    zevt.u.key_press.lk = ZK_to_LK(zevt.u.key_press.zk);

    switch (zevt.u.key_press.zk) {
    case ZK_LCTRL:
    case ZK_RCTRL:
        modifier_states |= ZGL_CTRL_MASK;
        break;
    case ZK_LALT:
    case ZK_RALT:
        modifier_states |= ZGL_ALT_MASK;
        break;
    case ZK_LSHIFT:
    case ZK_RSHIFT:
        modifier_states |= ZGL_SHIFT_MASK;
        break;
    default:
        break;
    }
    
    zevt.u.key_press.mod_flags = modifier_states;

    if (zevt.u.key_press.lk != LK_NONE) {
        // NOTE: .zk == ZK_NONE if and only if .lk == LK_NONE
        if ( ! (ZK_states[zevt.u.key_press.zk] & ZK_PRESSED)) {
            ZK_states[zevt.u.key_press.zk] |= ZK_PRESSED;
            zgl_PushEvent(&zevt);
        }
        
        if ( ! (LK_states[zevt.u.key_press.lk] & LK_PRESSED)) {
            LK_states[zevt.u.key_press.lk] |= LK_PRESSED;
        }
    }
}

static void KeyRelease_handler(XKeyReleasedEvent *xevt) {
    //put_XKeyEvent(&evt_union->xkey);

    // TODO: If I press shift-A, then release shift, then that should also
    // release shift-A
    
    zgl_Event zevt;
    zevt.type = EC_KeyRelease;

    zevt.u.key_release.zk = ZK_from_XK(XLookupKeysym(xevt, 0));
    zevt.u.key_release.lk = ZK_to_LK(zevt.u.key_release.zk);

    switch (zevt.u.key_release.zk) {
    case ZK_LCTRL:
    case ZK_RCTRL:
        modifier_states &= ~ZGL_CTRL_MASK;
        break;
    case ZK_LALT:
    case ZK_RALT:
        modifier_states &= ~ZGL_ALT_MASK;
        break;
    case ZK_LSHIFT:
    case ZK_RSHIFT:
        modifier_states &= ~ZGL_SHIFT_MASK;
        break;
    default:
        break;
    }
    
    zevt.u.key_release.mod_flags = modifier_states;
    
    if (zevt.u.key_release.zk != ZK_NONE) {
        // NOTE: .zk == ZK_NONE if and only if .lk == LK_NONE
        if (ZK_states[zevt.u.key_release.zk] & ZK_PRESSED) {
            ZK_states[zevt.u.key_release.zk] &= ~ZK_PRESSED;
            zgl_PushEvent(&zevt);
        }
        if (LK_states[zevt.u.key_release.lk] & LK_PRESSED) {
            LK_states[zevt.u.key_release.lk] &= ~LK_PRESSED;
        }
    }    
}

static void ButtonPress_handler(XButtonPressedEvent *xevt) {
    //put_XButtonEvent(&evt);
    
    zgl_Event zevt = {
        .type = EC_PointerPress,
        .u.pointer_press.x = xevt->x >> fb_info.upscale_shift,
        .u.pointer_press.y = xevt->y >> fb_info.upscale_shift
	};

    unsigned int xstate = xevt->state;
    zevt.u.pointer_press.mod_flags = 0;
    if (xstate & ControlMask) zevt.u.pointer_press.mod_flags |= ZGL_CTRL_MASK;
    if (xstate & Mod1Mask)    zevt.u.pointer_press.mod_flags |= ZGL_ALT_MASK;
    if (xstate & ShiftMask)   zevt.u.pointer_press.mod_flags |= ZGL_SHIFT_MASK;
    modifier_states = zevt.u.pointer_press.mod_flags;
    
    switch (xevt->button) {
    case Button1:
        zevt.u.pointer_press.zk = ZK_POINTER_LEFT;
        zevt.u.pointer_press.lk = LK_POINTER_LEFT;
        mouse_states |= ZGL_MOUSELEFT_MASK;
        break;
    case Button2:
        zevt.u.pointer_press.zk = ZK_POINTER_MIDDLE;
        zevt.u.pointer_press.lk = LK_POINTER_MIDDLE;
        mouse_states |= ZGL_MOUSEMID_MASK;
        break;
    case Button3:
        zevt.u.pointer_press.zk = ZK_POINTER_RIGHT;
        zevt.u.pointer_press.lk = LK_POINTER_RIGHT;
        mouse_states |= ZGL_MOUSERIGHT_MASK;
        break;
    case Button4:
        zevt.u.pointer_press.zk = ZK_POINTER_WHEELUP;
        zevt.u.pointer_press.lk = LK_POINTER_WHEELUP;
        mouse_states |= ZGL_MOUSEWHEELUP_MASK;
        break;
    case Button5:
        zevt.u.pointer_press.zk = ZK_POINTER_WHEELDOWN;;
        zevt.u.pointer_press.lk = LK_POINTER_WHEELDOWN;;
        mouse_states |= ZGL_MOUSEWHEELDOWN_MASK;
        break;
    default:
        return;
    }

    zevt.u.pointer_press.but_flags = mouse_states;

    if ( ! (ZK_states[zevt.u.pointer_press.zk] & ZK_PRESSED)) {
        ZK_states[zevt.u.pointer_press.zk] |= ZK_PRESSED;
    }

    if ( ! (LK_states[zevt.u.pointer_press.lk] & LK_PRESSED)) {
        zgl_PushEvent(&zevt);
        LK_states[zevt.u.pointer_press.lk] |= LK_PRESSED;

    }

}




static void ButtonRelease_handler(XButtonReleasedEvent *xevt) {
    //put_XButtonEvent(&evt);

    zgl_Event zevt = {
        .type = EC_PointerRelease,
        .u.pointer_release.x = xevt->x >> fb_info.upscale_shift,
        .u.pointer_release.y = xevt->y >> fb_info.upscale_shift
    };
    
    unsigned int xstate = xevt->state;
    zevt.u.pointer_release.mod_flags = 0;
    if (xstate & ControlMask) zevt.u.pointer_release.mod_flags |= ZGL_CTRL_MASK;
    if (xstate & Mod1Mask)    zevt.u.pointer_release.mod_flags |= ZGL_ALT_MASK;
    if (xstate & ShiftMask)   zevt.u.pointer_release.mod_flags |= ZGL_SHIFT_MASK;
    modifier_states = zevt.u.pointer_release.mod_flags;
    
    switch (xevt->button) {
    case Button1:
        zevt.u.pointer_release.zk = ZK_POINTER_LEFT;
        zevt.u.pointer_release.lk = LK_POINTER_LEFT;
        mouse_states &= ~ZGL_MOUSELEFT_MASK;
        break;
    case Button2:
        zevt.u.pointer_release.zk = ZK_POINTER_MIDDLE;
        zevt.u.pointer_release.lk = LK_POINTER_MIDDLE;
        mouse_states &= ~ZGL_MOUSEMID_MASK;
        break;
    case Button3:
        zevt.u.pointer_release.zk = ZK_POINTER_RIGHT;
        zevt.u.pointer_release.lk = LK_POINTER_RIGHT;
        mouse_states &= ~ZGL_MOUSERIGHT_MASK;
        break;
    case Button4:
        zevt.u.pointer_release.zk = ZK_POINTER_WHEELUP;
        zevt.u.pointer_release.lk = LK_POINTER_WHEELUP;
        mouse_states &= ~ZGL_MOUSEWHEELUP_MASK;
        break;
    case Button5:
        zevt.u.pointer_release.zk = ZK_POINTER_WHEELDOWN;
        zevt.u.pointer_release.lk = LK_POINTER_WHEELDOWN;
        mouse_states &= ~ZGL_MOUSEWHEELDOWN_MASK;
        break;
    default:
        return;
    }

    zevt.u.pointer_release.but_flags = mouse_states;

    if (ZK_states[zevt.u.pointer_release.zk] & ZK_PRESSED) {
        ZK_states[zevt.u.pointer_release.zk] &= ~ZK_PRESSED;
    }

    if (LK_states[zevt.u.pointer_release.lk] & LK_PRESSED) {
        LK_states[zevt.u.pointer_release.lk] &= ~LK_PRESSED;
        zgl_PushEvent(&zevt);
    }

}

#define ABS_MACRO(x) ((x) < 0 ? (-x) : (x))

static void IgnoreMotionNotify_handler(XMotionEvent *xevt) {
    upscale_curr_x = xevt->x;
    upscale_curr_y = xevt->y;
    
    if (xevt->x != upscale_center_x && xevt->y != upscale_center_y)
        return;
    else
        MotionNotify_handler = RelativeMotionNotify_handler;
}

static void RelativeMotionNotify_handler(XMotionEvent *xevt) {
    //put_XMotionEvent(&evt);
    upscale_curr_x = xevt->x;
    upscale_curr_y = xevt->y;

    zgl_Event zevt = {
        .type = EC_PointerMotion,
        .u.pointer_motion.x = upscale_center_x >> fb_info.upscale_shift,
        .u.pointer_motion.y = upscale_center_y >> fb_info.upscale_shift,
        .u.pointer_motion.dx = (upscale_curr_x - upscale_center_x) >> fb_info.upscale_shift,
        .u.pointer_motion.dy = (upscale_curr_y - upscale_center_y) >> fb_info.upscale_shift,
        .u.pointer_motion.mod_flags = modifier_states,
        .u.pointer_motion.but_flags = mouse_states,
	};
    
    if (upscale_curr_x == upscale_center_x && upscale_curr_y == upscale_center_y)
        return;

    upscale_curr_x = upscale_center_x;
    upscale_curr_y = upscale_center_y;
    
    XWarpPointer(x11_info.display, None,
                 x11_info.win_info.win, 0, 0, 0, 0,
                 upscale_center_x,
                 upscale_center_y);
    
    zgl_PushEvent(&zevt);
}


static void AbsoluteMotionNotify_handler(XMotionEvent *xevt) {
    //put_XMotionEvent(&evt);
    
    upscale_curr_x = xevt->x;
    upscale_curr_y = xevt->y;
    

    zgl_Event zevt = {
        .type = EC_PointerMotion,
        .u.pointer_motion.x = upscale_curr_x >> fb_info.upscale_shift,
        .u.pointer_motion.y = upscale_curr_y >> fb_info.upscale_shift,
        .u.pointer_motion.dx = (upscale_curr_x >> fb_info.upscale_shift) - (upscale_last_x >> fb_info.upscale_shift),
        .u.pointer_motion.dy = (upscale_curr_y >> fb_info.upscale_shift) - (upscale_last_y >> fb_info.upscale_shift),
        .u.pointer_motion.mod_flags = modifier_states,
        .u.pointer_motion.but_flags = mouse_states,
	};

    upscale_last_x = upscale_curr_x;
    upscale_last_y = upscale_curr_y;
    
    zgl_PushEvent(&zevt);
}

static void EnterNotify_handler(XEnterWindowEvent *xevt) {
    zgl_Event zevt = {
        .type = EC_PointerEnter,
        .u.pointer_enter.x = xevt->x >> fb_info.upscale_shift,
        .u.pointer_enter.y = xevt->y >> fb_info.upscale_shift
	};

    zgl_PushEvent(&zevt);
}
static void LeaveNotify_handler(XLeaveWindowEvent *xevt) {
    zgl_Event zevt = {
        .type = EC_PointerLeave,
        .u.pointer_leave.x = xevt->x >> fb_info.upscale_shift,
        .u.pointer_leave.y = xevt->y >> fb_info.upscale_shift
	};

    zgl_PushEvent(&zevt);
}
static void FocusIn_handler(XFocusInEvent *xevt) {
    (void)xevt;
    //put_XFocusChangeEvent(&evt);
}
static void FocusOut_handler(XFocusOutEvent *xevt) {
    (void)xevt;
    //put_XFocusChangeEvent(&evt);
}
static void Expose_handler(XExposeEvent *xevt) {
    //put_XExposeEvent(&evt);
    
    if (xevt->count == 0)
	zgl_VideoUpdate();
}
static void CirculateNotify_handler(XCirculateEvent *xevt) {
    (void)xevt;
    //put_XCirculateEvent(&evt);
}
static void ConfigureNotify_handler(XConfigureEvent *xevt) {
    (void)xevt;
    //put_XConfigureEvent(&evt);
}
static void DestroyNotify_handler(XDestroyWindowEvent *xevt) {
    (void)xevt;
    //put_XDestroyWindowEvent(&evt);
}
static void GravityNotify_handler(XGravityEvent *xevt) {
    (void)xevt;
    //put_XGravityEvent(&evt);
}
static void MapNotify_handler(XMapEvent *xevt) {
    (void)xevt;
    //put_XMapEvent(&evt);
}
static void UnmapNotify_handler(XUnmapEvent *xevt) {
    (void)xevt;
    //put_XUnmapEvent(&evt);
}
static void ReparentNotify_handler(XReparentEvent *xevt) {
    (void)xevt;
    //put_XReparentEvent(&evt);
}

static void ClientMessage_handler(XClientMessageEvent *xevt) {
    //put_XClientMessageEvent(xevt);
    zgl_Event zevt;
    
    Atom a = xevt->data.l[0];
    if (a == WM_DELETE_WINDOW) {
        if (xevt->window == x11_info.win_info.win) {
            zevt.type = EC_CloseRequest;
            zgl_PushEvent(&zevt);
        }
    }
}
static void MappingNotify_handler(XMappingEvent *xevt) {
    //put_XMappingEvent(&evt);
    
    XRefreshKeyboardMapping(xevt);
}


static void handle_XEvent(XEvent *xevt) {
    switch (xevt->type) {
    case KeyPress:        KeyPress_handler(&xevt->xkey); break;
    case KeyRelease:      KeyRelease_handler(&xevt->xkey); break;
    case ButtonPress:     ButtonPress_handler(&xevt->xbutton); break;
    case ButtonRelease:   ButtonRelease_handler(&xevt->xbutton); break;
    case MotionNotify:    MotionNotify_handler(&xevt->xmotion); break;
    case EnterNotify:     EnterNotify_handler(&xevt->xcrossing); break;
    case LeaveNotify:     LeaveNotify_handler(&xevt->xcrossing); break;
    case FocusIn:         FocusIn_handler(&xevt->xfocus); break;
    case FocusOut:        FocusOut_handler(&xevt->xfocus); break;
    case Expose:          Expose_handler(&xevt->xexpose); break;
    case DestroyNotify:   DestroyNotify_handler(&xevt->xdestroywindow); break;
    case UnmapNotify:     UnmapNotify_handler(&xevt->xunmap); break;
    case MapNotify:       MapNotify_handler(&xevt->xmap); break;
    case ReparentNotify:  ReparentNotify_handler(&xevt->xreparent); break;
    case ConfigureNotify: ConfigureNotify_handler(&xevt->xconfigure); break;
    case GravityNotify:   GravityNotify_handler(&xevt->xgravity); break;
    case CirculateNotify: CirculateNotify_handler(&xevt->xcirculate); break;
    case ClientMessage:   ClientMessage_handler(&xevt->xclient); break;
    case MappingNotify:   MappingNotify_handler(&xevt->xmapping); break;
    }
}



#ifdef X_VERBOSITY
static KeySym xkeymap[256][8];
static KeySym reduced_xkeymap[256]; // = xkeymap[i][0]
static void analyze_keycodes_and_keysyms(void) {
    /* TODO: Label as "practice" function. Running this function is for informational purposes only. */
    int min_keycode, max_keycode;
    XDisplayKeycodes(x11_info.display, &min_keycode, &max_keycode);
    putdf("di", min_keycode);
    putdf("di", max_keycode);
    
    KeySym *x_keysym;
    KeyCode first_keycode = min_keycode;
    int keycode_count = max_keycode - first_keycode + 1;
    int keysyms_per_keycode;
    x_keysym = XGetKeyboardMapping(x11_info.display, first_keycode, keycode_count, &keysyms_per_keycode);
    int listlen = keycode_count * keysyms_per_keycode;
    KeyCode K = 0;
    KeySym N = keysyms_per_keycode + 1;
    for (int i = 0; i < listlen; i++) {
        if (N >= (unsigned long)keysyms_per_keycode) {
            printf("\n%3d: ", first_keycode + K);
            N = 0;
            reduced_xkeymap[K] = x_keysym[(K - first_keycode)*keysyms_per_keycode];
            K++;
        }
        xkeymap[K][N] = x_keysym[(K - first_keycode)*keysyms_per_keycode + N];
        printf("%s ", XKeysymToString(xkeymap[K][N]));
        N++;
    }
    putchar('\n');
    XFree(x_keysym);

    putchar('\n');
    for (int i = 0; i <= 255; i++, K++) {
        printf("%3i : %.8lx : %s\n", K, reduced_xkeymap[K], XKeysymToString(reduced_xkeymap[K]));
    }
    
    XModifierKeymap *mkm = XGetModifierMapping(x11_info.display);
    // 8 modifiers by mkm->max_keypermod keycodes

    putchar('\n');
    for(int i_mod = 0; i_mod < 8; i_mod++) {
        switch (i_mod) {
        case 0:
            printf("Shift: ");
            break;
        case 1:
            printf(" Lock: ");
            break;
        case 2:
            printf(" Ctrl: ");
            break;
        case 3:
            printf(" Mod1: ");
            break;
        case 4:
            printf(" Mod2: ");
            break;
        case 5:
            printf(" Mod3: ");
            break;
        case 6:
            printf(" Mod4: ");
            break;
        case 7:
            printf(" Mod5: ");
            break;
        }
	
        for (int i_keycode = 0; i_keycode < mkm->max_keypermod; i_keycode++) {
            printf("%3d ", mkm->modifiermap[i_mod * 8 + i_keycode] + 7);
        }
        putchar('\n');
    }

    XFreeModifiermap(mkm);
}
#else
static void analyze_keycodes_and_keysyms(void) {
    return;
}
#endif
