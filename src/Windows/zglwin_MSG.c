#include <stdlib.h> // just for exit()
#include <stdio.h>
#include <windowsx.h> // GET_X_LPARAM and GET_Y_LPARAM

#include "src/zigil_input.h"
#include "src/zigil_event.h"
#include "src/Windows/zglwin.h"
#include "src/Windows/zglwin_MSG.h"
#include "src/Windows/zglwin_LK_from_VK.h"

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

static int center_x = 0, center_y = 0;
static int curr_x = 0, curr_y = 0;
static int last_x = 0, last_y = 0;
static HCURSOR cursors[NUM_ZC];
extern zgl_Result _zgl_WaitUpdateEventQueue(void); // internal to zigil, but not
                                                   // system-specific
//static zgl_Result do_next_message(void);
static zgl_Result wait_next_message(void);
static void handle_message(MSG *msg);
static void RelativeMouseMove_handler(MSG *msg);
static void AbsoluteMouseMove_handler(MSG *msg);
void (*MouseMove_handler)(MSG *msg) = AbsoluteMouseMove_handler;



/* ------- */
/* Cursors */
/* ------- */

void zgl_SetPointerCapture(void) {
    SetCapture(win_info.handle);
}

void zgl_ReleasePointerCapture(void) {
    ReleaseCapture();
}

void zgl_SetPointerPos(int x, int y) {
    POINT pt;
    pt.x = x;
    pt.y = y;
    ClientToScreen(win_info.handle, &pt);
    SetCursorPos(pt.x, pt.y);
}

zgl_Pixel zgl_GetPointerPos(void) {
    return (zgl_Pixel){curr_x, curr_y};
}

void zgl_SetCursor(zgl_CursorCode code) {
    SetCursor(cursors[code]);
}

RECT rcClip;
RECT rcOldClip;
void zgl_SetRelativePointer(void) {
    zgl_SetCursor(ZC_empty);
    MouseMove_handler = RelativeMouseMove_handler;
    center_x = win_info.width/2;
    center_y = win_info.height/2;
    last_x = center_x;
    last_y = center_y;

    zgl_SetPointerPos(center_x, center_y);
    
    GetClipCursor(&rcOldClip); // most likely entire screen
    GetClientRect(win_info.handle, &rcClip);
    POINT pt = { rcClip.left, rcClip.top };
    POINT pt2 = { rcClip.right, rcClip.bottom };
    ClientToScreen(win_info.handle, &pt);
    ClientToScreen(win_info.handle, &pt2);
    SetRect(&rcClip, pt.x, pt.y, pt2.x, pt2.y);
    ClipCursor(&rcClip); 
}
void zgl_ReleaseRelativePointer(zgl_Pixel *pos) {
    int dest_x, dest_y;
    if (pos) {
        dest_x = pos->x;
        dest_y = pos->y;
    }
    else {
        dest_x = 0;
        dest_y = 0;
    }
    
    ClipCursor(&rcOldClip);
    zgl_SetPointerPos(dest_x, dest_y);
    last_x = curr_x = dest_x;
    last_y = curr_y = dest_y;
    MouseMove_handler = AbsoluteMouseMove_handler;
    zgl_SetCursor(ZC_arrow);
}

#define SYSCOMMANDS                             \
    X(SC_CLOSE)                                 \
         X(SC_CONTEXTHELP)                      \
         X(SC_DEFAULT)                          \
         X(SC_HOTKEY)                           \
         X(SC_HSCROLL)                          \
         X(SCF_ISSECURE)                        \
         X(SC_KEYMENU)                          \
         X(SC_MAXIMIZE)                         \
         X(SC_MINIMIZE)                         \
    X(SC_MONITORPOWER)                          \
    X(SC_MOUSEMENU)                             \
    X(SC_MOVE)                                  \
    X(SC_NEXTWINDOW)                            \
    X(SC_PREVWINDOW)                            \
    X(SC_RESTORE)                               \
    X(SC_SCREENSAVE)                            \
    X(SC_SIZE)                                  \
    X(SC_TASKLIST)                              \
    X(SC_VSCROLL)                

#define NUM_SC (19)

struct SysCommand {
    WPARAM scid;
    char *name;
} SC_strs[NUM_SC] = {
#define X(x) {.scid = x, .name = #x},
    SYSCOMMANDS
#undef X
};









#define SUPPORTED_WINDOWS_MSG                   \
    X(WM_NULL)                                  \
         X(WM_CREATE)                           \
         X(WM_DESTROY)                          \
         X(WM_MOVE)                             \
         X(WM_SIZE)                             \
         X(WM_ACTIVATE)                         \
         X(WM_SETFOCUS)                         \
         X(WM_KILLFOCUS)                        \
         X(WM_ENABLE)                           \
    X(WM_SETREDRAW)                             \
    X(WM_SETTEXT)                               \
    X(WM_GETTEXT)                               \
    X(WM_GETTEXTLENGTH)                         \
    X(WM_PAINT)                                 \
    X(WM_CLOSE)                                 \
    X(WM_QUERYENDSESSION)                       \
    X(WM_QUIT)                                  \
    X(WM_QUERYOPEN)                             \
    X(WM_ERASEBKGND)                            \
    X(WM_SYSCOLORCHANGE)                        \
    X(WM_ENDSESSION)                            \
    X(WM_SHOWWINDOW)                            \
         X(WM_CTLCOLOR)                         \
         X(WM_WININICHANGE)                     \
         X(WM_DEVMODECHANGE)                    \
         X(WM_ACTIVATEAPP)                      \
         X(WM_FONTCHANGE)                       \
         X(WM_TIMECHANGE)                       \
         X(WM_CANCELMODE)                       \
         X(WM_SETCURSOR)                        \
    X(WM_MOUSEACTIVATE)                         \
    X(WM_CHILDACTIVATE)                         \
    X(WM_QUEUESYNC)                             \
    X(WM_GETMINMAXINFO)                         \
    X(WM_PAINTICON)                             \
    X(WM_ICONERASEBKGND)                        \
    X(WM_NEXTDLGCTL)                            \
    X(WM_SPOOLERSTATUS)                         \
    X(WM_DRAWITEM)                              \
    X(WM_MEASUREITEM)                           \
    X(WM_DELETEITEM)                            \
         X(WM_VKEYTOITEM)                       \
         X(WM_CHARTOITEM)                       \
         X(WM_SETFONT)                          \
         X(WM_GETFONT)                          \
         X(WM_SETHOTKEY)                        \
         X(WM_GETHOTKEY)                        \
         X(WM_QUERYDRAGICON)                    \
         X(WM_COMPAREITEM)                      \
         X(WM_GETOBJECT)                        \
    X(WM_COMPACTING)                            \
    X(WM_COMMNOTIFY)                            \
    X(WM_WINDOWPOSCHANGING)                     \
    X(WM_WINDOWPOSCHANGED)                      \
    X(WM_POWER)                                 \
    X(WM_COPYDATA)                              \
    X(WM_CANCELJOURNAL)                         \
    X(WM_NOTIFY)                                \
    X(WM_INPUTLANGCHANGEREQUEST)                \
    X(WM_INPUTLANGCHANGE)                       \
    X(WM_TCARD)                                 \
         X(WM_HELP)                             \
         X(WM_USERCHANGED)                      \
         X(WM_NOTIFYFORMAT)                     \
         X(WM_CONTEXTMENU)                      \
         X(WM_STYLECHANGING)                    \
         X(WM_STYLECHANGED)                     \
         X(WM_DISPLAYCHANGE)                    \
         X(WM_GETICON)                          \
    X(WM_SETICON)                               \
    X(WM_NCCREATE)                              \
    X(WM_NCDESTROY)                             \
    X(WM_NCCALCSIZE)                            \
    X(WM_NCHITTEST)                             \
    X(WM_NCPAINT)                               \
    X(WM_NCACTIVATE)                            \
    X(WM_GETDLGCODE)                            \
    X(WM_SYNCPAINT)                             \
    X(WM_NCMOUSEMOVE)                           \
    X(WM_NCLBUTTONDOWN)                         \
         X(WM_NCLBUTTONUP)                      \
         X(WM_NCLBUTTONDBLCLK)                  \
         X(WM_NCRBUTTONDOWN)                    \
         X(WM_NCRBUTTONUP)                      \
         X(WM_NCRBUTTONDBLCLK)                  \
         X(WM_NCMBUTTONDOWN)                    \
         X(WM_NCMBUTTONUP)                      \
         X(WM_NCMBUTTONDBLCLK)                  \
         X(WM_NCXBUTTONDOWN)                    \
    X(WM_NCXBUTTONUP)                           \
    X(WM_NCXBUTTONDBLCLK)                       \
    X(WM_INPUT)                                 \
    X(WM_KEYDOWN)                               \
    /*X(WM_KEYFIRST)*/                          \
    X(WM_KEYUP)                                 \
    X(WM_CHAR)                                  \
    X(WM_DEADCHAR)                              \
    X(WM_SYSKEYDOWN)                            \
    X(WM_SYSKEYUP)                              \
    X(WM_SYSCHAR)                               \
         X(WM_SYSDEADCHAR)                      \
         /*X(WM_KEYLAST)*/                      \
         X(WM_IME_STARTCOMPOSITION)             \
         X(WM_IME_ENDCOMPOSITION)               \
         X(WM_IME_COMPOSITION)                  \
    X(WM_IME_KEYLAST)                           \
    X(WM_INITDIALOG)                            \
    X(WM_COMMAND)                               \
    X(WM_SYSCOMMAND)                            \
    X(WM_TIMER)                                 \
    X(WM_HSCROLL)                               \
    X(WM_VSCROLL)                               \
    X(WM_INITMENU)                              \
    X(WM_INITMENUPOPUP)                         \
    X(WM_MENUSELECT)                            \
    X(WM_MENUCHAR)                              \
         X(WM_ENTERIDLE)                        \
         X(WM_MENURBUTTONUP)                    \
         X(WM_MENUDRAG)                         \
         X(WM_MENUGETOBJECT)                    \
         X(WM_UNINITMENUPOPUP)                  \
    X(WM_MENUCOMMAND)                           \
         X(WM_CHANGEUISTATE)                    \
         X(WM_UPDATEUISTATE)                    \
    X(WM_QUERYUISTATE)                          \
    X(WM_CTLCOLORMSGBOX)                        \
    X(WM_CTLCOLOREDIT)                          \
    X(WM_CTLCOLORLISTBOX)                       \
    X(WM_CTLCOLORBTN)                           \
    X(WM_CTLCOLORDLG)                           \
    X(WM_CTLCOLORSCROLLBAR)                     \
    X(WM_CTLCOLORSTATIC)                        \
    /*X(WM_MOUSEFIRST)*/                        \
    X(WM_MOUSEMOVE)                             \
    X(WM_LBUTTONDOWN)                           \
         X(WM_LBUTTONUP)                        \
         X(WM_LBUTTONDBLCLK)                    \
         X(WM_RBUTTONDOWN)                      \
         X(WM_RBUTTONUP)                        \
         X(WM_RBUTTONDBLCLK)                    \
    X(WM_MBUTTONDOWN)                           \
         X(WM_MBUTTONUP)                        \
         X(WM_MBUTTONDBLCLK)                    \
         /*X(WM_MOUSELAST)*/                    \
    X(WM_MOUSEWHEEL)                            \
    X(WM_XBUTTONDOWN)                           \
    X(WM_XBUTTONUP)                             \
    X(WM_XBUTTONDBLCLK)                         \
    X(WM_MOUSEHWHEEL)                           \
    X(WM_PARENTNOTIFY)                          \
    X(WM_ENTERMENULOOP)                         \
    X(WM_EXITMENULOOP)                          \
    X(WM_NEXTMENU)                              \
    X(WM_SIZING)                                \
    X(WM_CAPTURECHANGED)                        \
    X(WM_MOVING)                                \
         X(WM_POWERBROADCAST)                   \
         X(WM_DEVICECHANGE)                     \
         X(WM_MDICREATE)                        \
         X(WM_MDIDESTROY)                       \
         X(WM_MDIACTIVATE)                      \
         X(WM_MDIRESTORE)                       \
         X(WM_MDINEXT)                          \
    X(WM_MDIMAXIMIZE)                           \
    X(WM_MDITILE)                               \
    X(WM_MDICASCADE)                            \
    X(WM_MDIICONARRANGE)                        \
    X(WM_MDIGETACTIVE)                          \
    X(WM_MDISETMENU)                            \
    X(WM_ENTERSIZEMOVE)                         \
    X(WM_EXITSIZEMOVE)                          \
    X(WM_DROPFILES)                             \
    X(WM_MDIREFRESHMENU)                        \
    X(WM_IME_SETCONTEXT)                        \
         X(WM_IME_NOTIFY)                       \
         X(WM_IME_CONTROL)                      \
         X(WM_IME_COMPOSITIONFULL)              \
         X(WM_IME_SELECT)                       \
         X(WM_IME_CHAR)                         \
         X(WM_IME_REQUEST)                      \
         X(WM_IME_KEYDOWN)                      \
    X(WM_IME_KEYUP)                             \
    X(WM_NCMOUSEHOVER)                          \
    X(WM_MOUSEHOVER)                            \
    X(WM_NCMOUSELEAVE)                          \
    X(WM_MOUSELEAVE)                            \
    X(WM_CUT)                                   \
    X(WM_COPY)                                  \
    X(WM_PASTE)                                 \
    X(WM_CLEAR)                                 \
    X(WM_UNDO)                                  \
    X(WM_RENDERFORMAT)                          \
    X(WM_RENDERALLFORMATS)                      \
         X(WM_DESTROYCLIPBOARD)                 \
         X(WM_DRAWCLIPBOARD)                    \
         X(WM_PAINTCLIPBOARD)                   \
         X(WM_VSCROLLCLIPBOARD)                 \
         X(WM_SIZECLIPBOARD)                    \
         X(WM_ASKCBFORMATNAME)                  \
         X(WM_CHANGECBCHAIN)                    \
    X(WM_HSCROLLCLIPBOARD)                      \
    X(WM_QUERYNEWPALETTE)                       \
    X(WM_PALETTEISCHANGING)                     \
    X(WM_PALETTECHANGED)                        \
    X(WM_HOTKEY)                                \
    X(WM_PRINT)                                 \
    X(WM_PRINTCLIENT)                           \
    X(WM_APPCOMMAND)                            \
    X(WM_USER)                                  \
    X(WM_APP)                                   

#define NUM_SUPPORTED_WINDOWS_MSG (206)

struct WindowsMessage {
    UINT uMsg;
    char *name;
} MSG_strs[NUM_SUPPORTED_WINDOWS_MSG] = {
#define X(x) {.uMsg = x, .name = #x},
    SUPPORTED_WINDOWS_MSG
#undef X
};

//#define LOG_MSG
#ifdef LOG_MSG
void print_MSG_QUEUE(UINT uMsg) {
    for (int i = 0; i < NUM_SUPPORTED_WINDOWS_MSG; i++) {
        if (MSG_strs[i].uMsg == uMsg) {
            printf("[MSG_QUEUE]: %s\n", MSG_strs[i].name);
        }
    }
}
void print_MSG_WNDPROC(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    (void)hWnd, (void)lParam;
    
    for (int i = 0; i < NUM_SUPPORTED_WINDOWS_MSG; i++) {
        if (MSG_strs[i].uMsg == uMsg) {
            printf("[MSG_WNDPROC]: %s", MSG_strs[i].name);
            if (uMsg == WM_SYSCOMMAND) {
                for (int j = 0; j < NUM_SC; j++) {
                    if (SC_strs[j].scid == (wParam & 0xFFF0)) {
                        printf(" --> %s", SC_strs[j].name);
                    }
                }
            }
        }
    }

    putchar('\n');
}
#else
#define print_MSG_QUEUE(ignored)
#define print_MSG_WNDPROC(a1, a2, a3, a4)
#endif

zgl_Result zgl_UpdateEventQueue(void) {
    MSG msg;

    // TODO: NULL instead of window handle so all thread messages?  Do I get any
    // thread messages besides those for the handle, not counting WM_QUIT?
    while (PeekMessageA(&msg, NULL/*win_info.handle*/, 0, 0, PM_REMOVE)) {
        print_MSG_QUEUE(msg.message);
        if (msg.message == WM_QUIT) {
            break;
        }
        handle_message(&msg);
    }
    
    return ZR_SUCCESS;
}

zgl_Result _zgl_WaitUpdateEventQueue(void) { // the blocking version of
    MSG msg;
    
    wait_next_message(); // this blocks until an event actually happens
    while (PeekMessageA(&msg, win_info.handle, 0, 0, PM_REMOVE)) {
        handle_message(&msg);
    }
    return ZR_SUCCESS;
}

/*
static zgl_Result do_next_message(void) {
    MSG msg;
    
    if (PeekMessageA(&msg, win_info.handle, 0, 0, PM_REMOVE)) {
        handle_message(&msg);
    }

    return ZR_SUCCESS;
}
*/

static zgl_Result wait_next_message(void) {
    MSG msg;
    
    GetMessage(&msg, win_info.handle, 0, 0);
    handle_message(&msg);

    return ZR_SUCCESS;
}

zgl_Result init_MSG(void) {
    /*
    RAWINPUTDEVICE *rid = win_info.rid;
    
    rid[0].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
    rid[0].usUsage = 0x02;              // HID_USAGE_GENERIC_MOUSE
    rid[0].dwFlags = 0; //RIDEV_NOLEGACY;
    rid[0].hwndTarget = 0;

    rid[1].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
    rid[1].usUsage = 0x06;              // HID_USAGE_GENERIC_KEYBOARD
    rid[1].dwFlags = 0; //RIDEV_NOLEGACY;
    rid[1].hwndTarget = 0;

    if (FALSE == RegisterRawInputDevices(rid, 2, sizeof(rid[0]))) {
        //registration failed. Call GetLastError for the cause of the error
    }
    */
    
    init_LK_from_VK();

    cursors[ZC_arrow]     = LoadImageA(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0,
                                       LR_DEFAULTSIZE | LR_SHARED);
    cursors[ZC_drag]      = LoadImageA(NULL, IDC_SIZEALL, IMAGE_CURSOR, 0, 0,
                                       LR_DEFAULTSIZE | LR_SHARED);
    cursors[ZC_crosshair] = LoadImageA(NULL, IDC_CROSS, IMAGE_CURSOR, 0, 0,
                                       LR_DEFAULTSIZE | LR_SHARED);
    cursors[ZC_plus]      = LoadImageA(NULL, IDC_CROSS, IMAGE_CURSOR, 0, 0,
                                       LR_DEFAULTSIZE | LR_SHARED);
    cursors[ZC_cross]     = LoadImageA(NULL, IDC_CROSS, IMAGE_CURSOR, 0, 0,
                                       LR_DEFAULTSIZE | LR_SHARED);
    cursors[ZC_text]      = LoadImageA(NULL, IDC_IBEAM, IMAGE_CURSOR, 0, 0,
                                       LR_DEFAULTSIZE | LR_SHARED);    
    cursors[ZC_empty]     = NULL;

    for (int i = 0; i < NUM_ZC-1; i++) {
        if (!cursors[i]) {
            printf("ZIGIL: Could not load cursors.\n");
            return ZR_ERROR;
        }
    }    

    return ZR_SUCCESS;
}
zgl_Result term_MSG(void) {
    /* TODO: Verify: Don't need to destroy image loaded with LR_SHARED flag?
    DestroyCursor(cursors[ZC_arrow]);
    DestroyCursor(cursors[ZC_drag]);
    DestroyCursor(cursors[ZC_crosshair]);
    DestroyCursor(cursors[ZC_plus]);
    DestroyCursor(cursors[ZC_cross]);
    DestroyCursor(cursors[ZC_text]);
    */
    
    MSG msg;
    // TODO: handle (ignore) any queued messages up to and including WM_QUIT
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
        print_MSG_QUEUE(msg.message);
        if (msg.message == WM_QUIT) {
            break;
        }
        DispatchMessage(&msg);
    }

    return ZR_SUCCESS;
}

static zgl_LongKeyCode LK_from_MSG(WORD vk, BOOL ext_key, WORD scancode) {
    switch (vk) {
    case VK_CONTROL:
        return (! ext_key) ? LK_LCTRL : LK_RCTRL;
    case VK_MENU:
        return (! ext_key) ? LK_LALT : LK_RALT;
    case VK_SHIFT:
        return (VK_LSHIFT == MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX)) ? LK_LSHIFT : LK_RSHIFT;
    default:
        return LK_from_VK(vk);
    }
}

static void KeyDown_handler(MSG *msg) {
    WORD vk = LOWORD(msg->wParam);
    WORD flags = HIWORD(msg->lParam);
    WORD scancode = LOBYTE(flags);
    BOOL repeat = flags & KF_REPEAT; // technically "was previously down"
    //WORD repeat_count = LOWORD(msg->lParam);
    BOOL ext_key = flags & KF_EXTENDED;
    
    if (repeat) return;
    
    zgl_Event zevt;
    zevt.type = EC_KeyPress;

    zgl_LongKeyCode key = LK_from_MSG(vk, ext_key, scancode);
    zevt.u.key_press.key = key;
    
    if (key != LK_NONE) {
        if (LK_states[key] == false) {
            LK_states[key] = true;
            zgl_PushEvent(&zevt);
        }
    }
}

static void KeyUp_handler(MSG *msg) {
    WORD vk = LOWORD(msg->wParam);
    WORD flags = HIWORD(msg->lParam);
    WORD scancode = LOBYTE(flags);
    //BOOL repeat = flags & KF_REPEAT;
    BOOL ext_key = flags & KF_EXTENDED;

    
    zgl_Event zevt;
    zevt.type = EC_KeyRelease;

    zgl_LongKeyCode key = LK_from_MSG(vk, ext_key, scancode);
    zevt.u.key_release.key = key;
    
    if (key != LK_NONE) {
        if (LK_states[key] == true) {
            LK_states[key] = false;
            zgl_PushEvent(&zevt);
        }
    }
}

static void ButtonDown_handler(MSG *msg, WORD vk) {
    zgl_Event zevt;

    zgl_Pixit x = GET_X_LPARAM(msg->lParam);
    zgl_Pixit y = GET_Y_LPARAM(msg->lParam);

    zevt.type = EC_PointerPress;
    zevt.u.pointer_press.x = x >> fb_info.upscale_shift;
    zevt.u.pointer_press.y = y >> fb_info.upscale_shift;
    last_x = x;
    last_y = y;
    
    switch (vk) {
    case VK_LBUTTON:
        zevt.u.pointer_press.key = LK_POINTER_LEFT;
        break;
    case VK_MBUTTON:
        zevt.u.pointer_press.key = LK_POINTER_MIDDLE;
        break;
    case VK_RBUTTON:
        zevt.u.pointer_press.key = LK_POINTER_RIGHT;
        break;
    case VK_XBUTTON1:
        zevt.u.pointer_press.key = LK_POINTER_X1;
        break;
    case VK_XBUTTON2:
        zevt.u.pointer_press.key = LK_POINTER_X2;
        break;
    default:
        return;
    }

    if (LK_states[zevt.u.pointer_press.key] == false) {
        LK_states[zevt.u.pointer_press.key] = true;
        zgl_PushEvent(&zevt);
    }
}

static void ButtonUp_handler(MSG *msg, WORD vk) {
    zgl_Event zevt;

    zgl_Pixit x = GET_X_LPARAM(msg->lParam);
    zgl_Pixit y = GET_Y_LPARAM(msg->lParam);

    zevt.type = EC_PointerRelease;
    zevt.u.pointer_release.x = x >> fb_info.upscale_shift;
    zevt.u.pointer_release.y = y >> fb_info.upscale_shift;
    last_x = x;
    last_y = y;
    
    switch (vk) {
    case VK_LBUTTON:
        zevt.u.pointer_release.key = LK_POINTER_LEFT;
        break;
    case VK_MBUTTON:
        zevt.u.pointer_release.key = LK_POINTER_MIDDLE;
        break;
    case VK_RBUTTON:
        zevt.u.pointer_release.key = LK_POINTER_RIGHT;
        break;
    case VK_XBUTTON1:
        zevt.u.pointer_release.key = LK_POINTER_X1;
        break;
    case VK_XBUTTON2:
        zevt.u.pointer_release.key = LK_POINTER_X2;
        break;
    default:
        return;
    }

    if (LK_states[zevt.u.pointer_release.key] == true) {
        LK_states[zevt.u.pointer_release.key] = false;
        zgl_PushEvent(&zevt);
    }
}

static void MouseWheel_handler(MSG *msg) {
    zgl_Event zevt;
    int delta = GET_WHEEL_DELTA_WPARAM(msg->wParam);

    POINT p = {GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};
    ScreenToClient(win_info.handle, &p);
    
    zevt.type = EC_PointerPress;
    zevt.u.pointer_press.x = p.x >> fb_info.upscale_shift;
    zevt.u.pointer_press.y = p.y >> fb_info.upscale_shift;
    last_x = p.x;
    last_y = p.y;

    zevt.u.pointer_press.key = delta > 0 ? LK_POINTER_WHEELUP : LK_POINTER_WHEELDOWN;
    
    zgl_PushEvent(&zevt);
}

static void RelativeMouseMove_handler(MSG *msg) {
    curr_x = GET_X_LPARAM(msg->lParam);
    curr_y = GET_Y_LPARAM(msg->lParam);

    zgl_Event zevt = {
        .type = EC_PointerMotion,
        .u.pointer_motion.x  = center_x,
        .u.pointer_motion.y  = center_y,
        .u.pointer_motion.dx = curr_x - center_x,
        .u.pointer_motion.dy = curr_y - center_y,
    };

    if (curr_x == center_x && curr_y == center_y)
        return;

    curr_x = center_x;
    curr_y = center_y;

    POINT pt;
    pt.x = center_x;
    pt.y = center_y;
    ClientToScreen(win_info.handle, &pt);
    SetCursorPos(pt.x, pt.y);

    zgl_PushEvent(&zevt);
}

static void AbsoluteMouseMove_handler(MSG *msg) {
    curr_x = GET_X_LPARAM(msg->lParam);
    curr_y = GET_Y_LPARAM(msg->lParam);

    // TODO: Detect if this is a "enter" event

    zgl_Event zevt = {
        .type = EC_PointerMotion,
        .u.pointer_motion.x  = curr_x >> fb_info.upscale_shift,
        .u.pointer_motion.y  = curr_y >> fb_info.upscale_shift,
        .u.pointer_motion.dx = curr_x - last_x,
        .u.pointer_motion.dy = curr_y - last_y,
    };

    last_x = curr_x;
    last_y = curr_y;
    
    zgl_PushEvent(&zevt);
}

static LRESULT RawInput_handler(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    (void)hWnd, (void)wParam;
    
    UINT dwSize;

    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
    LPBYTE lpb[dwSize];

    if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
        puts("GetRawInputData does not return correct size !\n"); 

    RAWINPUT* raw = (RAWINPUT*)lpb;
    
    if (raw->header.dwType == RIM_TYPEKEYBOARD) {
        /*
        printf("Kbd: make=%04x Flags:%04x Reserved:%04x ExtraInformation:%08lx, msg=%04x VK=%04x \n", 
                 raw->data.keyboard.MakeCode, 
                 raw->data.keyboard.Flags, 
                 raw->data.keyboard.Reserved, 
                 raw->data.keyboard.ExtraInformation, 
                 raw->data.keyboard.Message, 
                 raw->data.keyboard.VKey);
        */
    }
    else if (raw->header.dwType == RIM_TYPEMOUSE) {
        /*
        printf("Mouse: usFlags=%04x ulButtons=%04lx usButtonFlags=%04x usButtonData=%04x ulRawButtons=%04lx lLastX=%04lx lLastY=%04lx ulExtraInformation=%04lx\r\n", 
                 raw->data.mouse.usFlags, 
                 raw->data.mouse.ulButtons, 
                 raw->data.mouse.usButtonFlags, 
                 raw->data.mouse.usButtonData, 
                 raw->data.mouse.ulRawButtons, 
                 raw->data.mouse.lLastX, 
                 raw->data.mouse.lLastY, 
                 raw->data.mouse.ulExtraInformation);
        */
     }

    return 0;
}

static void handle_message(MSG *msg) {
    switch (msg->message) {
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:     KeyDown_handler(msg); break;
    case WM_SYSKEYUP:
    case WM_KEYUP:       KeyUp_handler(msg); break;
    case WM_LBUTTONDOWN: ButtonDown_handler(msg, VK_LBUTTON); break;
    case WM_RBUTTONDOWN: ButtonDown_handler(msg, VK_RBUTTON); break;
    case WM_MBUTTONDOWN: ButtonDown_handler(msg, VK_MBUTTON); break;
    case WM_XBUTTONDOWN:
        if (HIWORD(msg->wParam) == XBUTTON1) {
            ButtonDown_handler(msg, VK_XBUTTON1);
        }
        else if (HIWORD(msg->wParam) == XBUTTON2) {
            ButtonDown_handler(msg, VK_XBUTTON2);
        }
        break;
    case WM_LBUTTONUP: ButtonUp_handler(msg, VK_LBUTTON); break;
    case WM_RBUTTONUP: ButtonUp_handler(msg, VK_RBUTTON); break;
    case WM_MBUTTONUP: ButtonUp_handler(msg, VK_MBUTTON); break;
    case WM_XBUTTONUP:
        if (HIWORD(msg->wParam) == XBUTTON1) {
            ButtonUp_handler(msg, VK_XBUTTON1);
        }
        else if (HIWORD(msg->wParam) == XBUTTON2) {
            ButtonUp_handler(msg, VK_XBUTTON2);
        }
        break;
    case WM_MOUSEMOVE: MouseMove_handler(msg); break;
    case WM_MOUSEWHEEL: MouseWheel_handler(msg); break;
        //case WM_MOUSELEAVE:           LeaveNotify_handler(msg); break;
    default:
        break;
    }

    DispatchMessage(msg);
}

LRESULT CALLBACK MainWndProc
(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // FEAR NOT: The WM_PAINT message is not explicitly handled here, but rather
    // it goes to DefWindowProc which does a blank paint because I do drawing
    // on my own terms.
    
    print_MSG_WNDPROC(hWnd, uMsg, wParam, lParam);
    zgl_Event zevt;
	switch (uMsg) {
    case WM_CREATE:
    case WM_SYSKEYUP:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_KEYDOWN:
        goto ignored;
    case WM_CLOSE:
        zevt.type = EC_CloseRequest;
        zgl_PushEvent(&zevt);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0); // sends WM_QUIT to message queue.
        return 0;
    case WM_SYSCOMMAND:
        if (wParam == (SC_KEYMENU & 0xFFF0)) goto ignored;
        else goto passed;
    case WM_ERASEBKGND:
        return 1; // 0 = remain marked for erasing. We don't want this.
    case WM_INPUT:
        return RawInput_handler(hWnd, wParam, lParam);
    default:
        goto passed;
	}

 ignored:
    return 0;
 passed:
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//  WM_QUIT: Entire application wants to quit.

//  WM_DESTROY: Since zigil is a one-window application, window destruction
//  implies we should immediately quit.

//  WM_CLOSE: (caused by X button, Alt+F4, etc...) User has indicated their
//  intent to close the application. Typically leads to window destruction
//  (possibly after asking the user to confirm their intent).




