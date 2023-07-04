#include <stdio.h>

#include "src/Windows/zglwin.h"
#include "src/Windows/zglwin_MSG.h"

/* Zigil API functions implemented here; also declared in zigil.h */
extern void zgl_VideoUpdate(void);
extern void zgl_BlitToFramebuffer
(zgl_Pixit fb_x, zgl_Pixit fb_y,
 zgl_PixelArray const *const src,
 zgl_Pixit src_x, zgl_Pixit src_y,
 zgl_Pixit w, zgl_Pixit h);
extern zgl_Result zgl_SetWindowTitle(char const *title);
extern zgl_Result zgl_LibInit(char *const title, uint32_t flags);
extern zgl_Result zgl_LibValidate(void);
extern void zgl_LibTerm(void);


/* --------- */
/* Internals */
/* --------- */

#ifdef WIN_VERBOSITY
#define VERBOSITY
#endif
#include "src/diblib_local/verbosity.h"

/* Subroutine for zgl_VideoUpdate() */
static void video_blit(void);

/* Subroutine for zgl_VideoUpdate() */
static void video_sync(void);

zgl_WindowsInfo win_info = {0};
#define WINCLASSNAME "ZIGIL_WINDOW"
extern LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // defined in zglwin_event.c

    
#define FW_CALC_PITCH(w, bpp)    ((((w) * (bpp)) + 3) & ~3)
#define FW_BYTE_ALIGN(w, bpp)    (FW_CALC_PITCH(w, bpp) / (bpp))


#define WINDOW_STYLE_FLAGS              \
    {                                   \
    Z(WS_BORDER);                       \
    Z(WS_CAPTION);                      \
    Z(WS_CHILD);                        \
    Z(WS_CHILDWINDOW);                  \
    Z(WS_CLIPCHILDREN);                 \
    Z(WS_CLIPSIBLINGS);                 \
    Z(WS_DISABLED);                     \
    Z(WS_DLGFRAME);                     \
    Z(WS_GROUP);                        \
    Z(WS_HSCROLL);                      \
    Z(WS_ICONIC);                       \
    Z(WS_MAXIMIZE);                     \
    Z(WS_MAXIMIZEBOX);                  \
    Z(WS_MINIMIZE);                     \
    Z(WS_MINIMIZEBOX);                  \
    Z(WS_OVERLAPPED);                   \
    /*Z(WS_OVERLAPPEDWINDOW);*/         \
    Z(WS_POPUP);                        \
    /*Z(WS_POPUPWINDOW);*/              \
    Z(WS_SIZEBOX);                      \
    Z(WS_SYSMENU);                      \
    Z(WS_TABSTOP);                      \
    Z(WS_THICKFRAME);                   \
    Z(WS_TILED);                        \
    /*Z(WS_TILEDWINDOW);*/              \
    Z(WS_VISIBLE);                      \
    Z(WS_VSCROLL);                      \
    }

#define EXTENDED_WINDOW_STYLE_FLAGS             \
    {                                           \
    Z(WS_EX_ACCEPTFILES);                       \
    Z(WS_EX_APPWINDOW);                         \
    Z(WS_EX_CLIENTEDGE);                        \
    Z(WS_EX_COMPOSITED);                        \
    Z(WS_EX_CONTEXTHELP);                       \
    Z(WS_EX_CONTROLPARENT);                     \
    Z(WS_EX_DLGMODALFRAME);                     \
    Z(WS_EX_LAYERED);                           \
    Z(WS_EX_LAYOUTRTL);                         \
    Z(WS_EX_LEFT);                              \
    Z(WS_EX_LEFTSCROLLBAR);                     \
    Z(WS_EX_LTRREADING);                        \
    Z(WS_EX_MDICHILD);                          \
    Z(WS_EX_NOACTIVATE);                        \
    Z(WS_EX_NOINHERITLAYOUT);                   \
    Z(WS_EX_NOPARENTNOTIFY);                    \
    Z(WS_EX_NOREDIRECTIONBITMAP);               \
    Z(WS_EX_RIGHT);                             \
    Z(WS_EX_RIGHTSCROLLBAR);                    \
    Z(WS_EX_RTLREADING);                        \
    Z(WS_EX_STATICEDGE);                        \
    Z(WS_EX_TOOLWINDOW);                        \
    Z(WS_EX_TOPMOST);                           \
    Z(WS_EX_TRANSPARENT);                       \
    Z(WS_EX_WINDOWEDGE);                        \
    }

void print_WINDOWINFO(WINDOWINFO *wi) {
    /*
    typedef struct tagWINDOWINFO {
        DWORD cbSize;
        RECT  rcWindow;
        RECT  rcClient;
        DWORD dwStyle;
        DWORD dwExStyle;
        DWORD dwWindowStatus;
        UINT  cxWindowBorders;
        UINT  cyWindowBorders;
        ATOM  atomWindowType;
        WORD  wCreatorVersion;
    } WINDOWINFO;
    */

    lputf("cbSize", "lu", wi->cbSize);
    lputf("Win x", "li", wi->rcWindow.left);
    lputf("Win y", "li", wi->rcWindow.top);
    lputf("Win w", "li", wi->rcWindow.right - wi->rcWindow.left);
    lputf("Win h", "li", wi->rcWindow.bottom - wi->rcWindow.top);
    lputf("CWin x", "li", wi->rcClient.left);
    lputf("CWin y", "li", wi->rcClient.top);
    lputf("CWin w", "li", wi->rcClient.right - wi->rcClient.left);
    lputf("CWin h", "li", wi->rcClient.bottom - wi->rcClient.top);
    puts("Style:");
#define Z(VAL)                                              \
    printf("%20s: %u\n", #VAL, (wi->dwStyle & VAL) == VAL)
    WINDOW_STYLE_FLAGS;
#undef Z
    puts("Extended Style:");
#define Z(VAL)                                                  \
    printf("%26s: %u\n", #VAL, (wi->dwExStyle & VAL) == VAL)
    EXTENDED_WINDOW_STYLE_FLAGS;
#undef Z    
    lputf("Window Status", "lu", wi->dwWindowStatus);
    lputf("Border Width", "u", wi->cxWindowBorders);
    lputf("Border Height", "u", wi->cyWindowBorders);
    lputf("Atom Window Type", "i", wi->atomWindowType);
    lputf("Creator Version", "i", wi->wCreatorVersion);
}

#define CLASS_STYLE_FLAGS                       \
    {                                           \
    Z(CS_BYTEALIGNCLIENT);                      \
    Z(CS_BYTEALIGNWINDOW);                      \
    Z(CS_CLASSDC);                              \
    Z(CS_DBLCLKS);                              \
    Z(CS_DROPSHADOW);                           \
    Z(CS_GLOBALCLASS);                          \
    Z(CS_HREDRAW);                              \
    Z(CS_NOCLOSE);                              \
    Z(CS_OWNDC);                                \
    Z(CS_PARENTDC);                             \
    Z(CS_SAVEBITS);                             \
    Z(CS_VREDRAW);                              \
    }

void print_WNDCLASSEX(WNDCLASSEX *wc) {
    /*
      typedef struct tagWNDCLASSEXA {
          UINT      cbSize;
          UINT      style;
          WNDPROC   lpfnWndProc;
          int       cbClsExtra;
          int       cbWndExtra;
          HINSTANCE hInstance;
          HICON     hIcon;
          HCURSOR   hCursor;
          HBRUSH    hbrBackground;
          LPCSTR    lpszMenuName;
          LPCSTR    lpszClassName;
          HICON     hIconSm;
      } WNDCLASSEX;
    */

    fputs("WNDCLASSEX\n", stdout);
    fputs("----------\n", stdout);
    lputf("cbSize", "u", wc->cbSize);
    printf("style:\n");
#define Z(VAL) \
    printf("%20s: %u\n", #VAL, (wc->style & VAL) == VAL)
    CLASS_STYLE_FLAGS;
#undef Z    
    //lputf("lpfnWndProc", "p", (void *)wc->lpfnWndProc);
    lputf("cbClsExtra", "i", wc->cbClsExtra);
    lputf("cbWndExtra", "i", wc->cbWndExtra);
    lputf("hInstance", "p", (void *)wc->hInstance);
    lputf("hIcon", "p", (void *)wc->hIcon);
    lputf("hIconSm", "p", (void *)wc->hIconSm);
    lputf("hCursor", "p", (void *)wc->hCursor);
    lputf("hbrBackground", "p", (void *)wc->hbrBackground);
    lputf("lpszMenuName", "s", wc->lpszMenuName);
    lputf("lpszClassName", "s", wc->lpszClassName);
}

zgl_Result zgl_LibInit(char *title, uint32_t flags) {
    (void)flags;
    
    vbs_putchar('\n');
    vbs_fputs("----------------------------------\n", stdout);
    vbs_fputs("- ZIGIL: Creating Windows Window -\n", stdout);
    vbs_fputs("----------------------------------\n", stdout);
    vbs_putchar('\n');
    
    //default_error_handler = XSetErrorHandler(XError_handler);

    // Later we check if the window that gets created actually has this width
    // and height
    win_info.width = fb_info.upscale_width;
    win_info.height = fb_info.upscale_height;

    if (win_info.bitmap) {
        DeleteObject(win_info.bitmap);
    }
    
    win_info.style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    static bool created = false;
    if (!created) { /* register window class and create window */
        WNDCLASSEX *wc = &win_info.class;
        
        SetProcessDPIAware(); // This API call must absolutely be done before
                              // any window HWND is created
		wc->cbSize = sizeof(win_info.class);
		wc->style =
            CS_OWNDC | CS_HREDRAW | CS_VREDRAW |
            CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
		wc->lpfnWndProc = MainWndProc;
        wc->cbClsExtra = 0;
        wc->cbWndExtra = 0;
		win_info.inst = GetModuleHandle(NULL);
		wc->hInstance = GetModuleHandle(NULL);
		wc->hIcon =
            LoadIcon(wc->hInstance, IDI_APPLICATION);
		wc->hIconSm = LoadIcon(NULL, IDI_APPLICATION);
        wc->hCursor = NULL;
		wc->hbrBackground =
            (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc->lpszMenuName = NULL; // name menu resource
		wc->lpszClassName = WINCLASSNAME; // name of window class


        printf("ZIGIL: WNDCLASSEX to be sent to RegisterClassExA:\n");
        print_WNDCLASSEX(&win_info.class);
        putchar('\n');
        putchar('\n');
        puts("ZIGIL: Registering window class...");
        // TODO: Should I just use the RegisterClassEx alias which automatically
        // decides between ANSI (suffix A) or Unicode (suffix W) based on the
        // UNICODE preprocessor directive in zglwin.h?
        win_info.atom = RegisterClassExA(&win_info.class);
		if (!win_info.atom) {
            printf("ZIGIL: Could not register window class.\n");
			return ZR_ERROR;
		}

        lputdf("ZIGIL: Window Class Atom", "du", win_info.atom);
        win_info.atom_lpcstr = MAKEINTATOM(win_info.atom);
        
        WNDCLASSEX ret;
        if (!GetClassInfoExA(win_info.inst, WINCLASSNAME, &ret)) {
            puts("ZIGIL: Could not get class info for verification.");
        }
        
        puts("ZIGIL: WNDCLASSEX received from GetClassInfoExA:");
        print_WNDCLASSEX(&ret);
        putchar('\n');
        putchar('\n');
        
        puts("ZIGIL: Creating window...");
		win_info.handle = CreateWindowExA
            (0,              // DWORD extended window style
             WINCLASSNAME,   // LPCSTR class name
             WINCLASSNAME,   // LPCSTR window name
             win_info.style, // DWORD window style
             0, 0,           // int x and y position
             320, 200,       // int width and height
             NULL,           // HWND parent or owner (NULL = pop-up window)
             NULL,           // HMENU menu (NULL = use class menu)
             win_info.inst,  // HINSTANCE instance of module assoc. to window
             NULL            // LPVOID extra data (NULL = no extra data)
             );
        
        if (!win_info.handle) {
            puts("ZIGIL: Could not create window.\n");
            return ZR_ERROR;
        }
    
		created = true;
    }

    { /* Set window to desired size and position. */
        // TODO: Could this be done as WM_CREATE handler?
        puts("ZIGIL: Setting window size and position...");
        RECT r = (RECT){.left = 0,
                        .top = 0,
                        .right = fb_info.upscale_width,
                        .bottom = fb_info.upscale_height};
        // r now has intended client rectangle (ie. desired window size, minus
        // borders/bars)

        AdjustWindowRectExForDpi
            (&r, // rect, initially intended client size
             win_info.style, // window style
             0, // whether or not window has a menu
             0, // extended window style
             GetDpiForWindow(win_info.handle));
        // r now has the window size corresponding to the intended client
        // rectangle
        
        SetWindowPos(win_info.handle,
                     HWND_TOP, // place window at top
                     0, 0, // x and y
                     r.right - r.left, r.bottom - r.top, // width and height
                     SWP_NOMOVE |
                     SWP_NOOWNERZORDER |
                     SWP_NOZORDER | // doesn't this then ignore HWND_TOP
                     SWP_SHOWWINDOW);
    }
    
    { /* GDI stuff; see Wingdi.h */
        puts("ZIGIL: Creating framebuffer data...");
        BITMAPINFO *binfo = calloc(1, sizeof(*binfo));
        if (!binfo) {
            printf("ZIGIL: Out of memory.\n");
            return ZR_ERROR;
        }

        // setup the bitmap header data for the bitmap which will be the
        // framebuffer.
        memset(&binfo->bmiHeader, 0, sizeof(BITMAPINFOHEADER));
        binfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        binfo->bmiHeader.biWidth = fb_info.width;
        binfo->bmiHeader.biHeight = -fb_info.height;
        binfo->bmiHeader.biPlanes = 1;
        binfo->bmiHeader.biSizeImage = fb_info.pitch * fb_info.height;
        binfo->bmiHeader.biXPelsPerMeter = 0;
        binfo->bmiHeader.biYPelsPerMeter = 0;
        binfo->bmiHeader.biClrUsed = 0;
        binfo->bmiHeader.biClrImportant = 0;
        binfo->bmiHeader.biBitCount = fb_info.bytes_per_pixel * 8;

        {
            HDC hdc = GetDC(win_info.handle);
            // create a DIB (ie. bitmap) to write to directly
            win_info.bitmap =
                CreateDIBSection(hdc, //device context
                                 binfo, // attributes of the DIB
                                 DIB_RGB_COLORS, // array of literal RGB values
                                 (VOID **)(&fb_info.fb.pixels),
                                 NULL,
                                 0);
            ReleaseDC(win_info.handle, hdc);

            // NOTE that CreateDIBSection will allocate memory for the framebuffer and set the fb_info.fb.pixels pointer to point at that memory.
        }
    
        free(binfo);
        binfo = NULL;
        if (win_info.bitmap == NULL) {
            printf("ZIGIL: Unable to create display DIB/framebuffer.\n");
            return ZR_ERROR;
        }
    }

	SetWindowText(win_info.handle, title);
	ShowWindow(win_info.handle, SW_SHOW);
	UpdateWindow(win_info.handle);
	SetForegroundWindow(win_info.handle);
	SetFocus(win_info.handle);

    WINDOWINFO wi;
    wi.cbSize = sizeof(WINDOWINFO);
    if (!GetWindowInfo(win_info.handle, &wi)) {
        puts("ZIGIL: Couldn't get window info for verification.");
    }

    puts("ZIGIL: WINDOWINFO received from GetWindowInfo:");
    print_WINDOWINFO(&wi);
    putchar('\n');
    putchar('\n');

    // Demand that the thread run on one core only (CPU 0)
    SetThreadAffinityMask(GetCurrentThread(), 1);

    if (ZR_ERROR == init_MSG()) {
        return ZR_ERROR;
    }
    
	/* refresh newly created display */
	video_blit();
	video_sync();
    
    return ZR_SUCCESS;
}

zgl_Result zgl_LibValidate(void) {
    return ZR_SUCCESS;
}

void zgl_VideoUpdate(void) {
    video_blit();
    video_sync();
}

void zgl_LibTerm(void) {
    WNDCLASSEX w;

	DeleteObject(win_info.bitmap); // DIB section
	DestroyWindow(win_info.handle);
	if (GetClassInfoExA(win_info.inst, WINCLASSNAME, &w)) {
		UnregisterClass(WINCLASSNAME, win_info.inst);
	}

    term_MSG();
}

static void video_blit(void) {
    if (!fb_info.fb.pixels) {
		return;
	}

    // TODO: Can I get DC's during init and hold onto them?
    HDC hdc, mdc;
    int src_w, src_h, dst_w, dst_h;
	
	hdc = GetDC(win_info.handle);
	mdc = CreateCompatibleDC(hdc);
	SelectObject(mdc, win_info.bitmap);
	src_w = fb_info.width;
	src_h = fb_info.height;
	dst_w = fb_info.upscale_width;
	dst_h = fb_info.upscale_height;
    
	switch (fb_info.upscale_shift == 0) {
    case 1:
        BitBlt(hdc, 0, 0, dst_w, dst_h, mdc, 0, 0, SRCCOPY);
        break;
    default:
        StretchBlt(hdc, 0, 0, dst_w, dst_h, mdc, 0, 0, src_w, src_h, SRCCOPY);
        break;
	}
    
	DeleteDC(mdc);
	ReleaseDC(win_info.handle, hdc);
}

static void video_sync(void) {
    // not entirely sure about how GdiFlush works and when it should be called.
    // presumably we call it because we want to update the screen by forcing gdi
    // to make any queued changes
    GdiFlush();
}
