#include <X11/Xlib.h>
#include <X11/Xutil.h> // XClassHints and XWMHints
#include <X11/Xatom.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "src/zigil_mem.h"
#include "src/X11/zglx.h"
#include "src/X11/zglx_macro_str.h"
#include "src/X11/zglx_ping.h"
#include "src/X11/zglx_event.h"

//#define X_VERBOSITY
#ifdef X_VERBOSITY
#define VERBOSITY
#endif
// The following must always be included; if X_VERBOSITY is undefined, the
// functions are stripped during preprocessing.
#define INCLUDE_SRC
#include "src/X11/zglx_printstruct.c" /* Printers of X11 structs; for debugging. */
#undef INCLUDE_SRC
#include "src/diblib_local/verbosity.h"


/* -------------------------------------------------------------- */
/* Zigil API functions implemented here; also declared in zigil.h */
/* -------------------------------------------------------------- */

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

zgl_X11WindowInfo x11_dbginfo = {0};
zgl_X11Info x11_info = {0};
Atom WM_DELETE_WINDOW; // We must speak to the window manager via
                       // XClientMessageEvents in order to respond to clicks on
                       // the window's "x" button and such.

/* Subroutine for zgl_LibInit() */
static zgl_Result init_xframebuffer(int width, int height, int shm);
//static zgl_Result create_xframebuffer(zgl_X11WindowInfo *xwi, int width, int height);

/* Subroutine for zgl_VideoUpdate() */
static void video_blit(void);

/* Subroutine for video_blit() when scale is not 1*/
static void upscale_framebuffer_to_ximage 
(unsigned *src, unsigned src_w, unsigned src_h,
 unsigned *dst, unsigned dst_w, unsigned dst_h);

/* Subroutine for zgl_VideoUpdate() */
static void video_sync(void); 

/* Related to X shared memory extension */
static int test_xshm(Display *display);
static int test_xlocal(void);
static long find_shmmax(int *guess);

/* Errors */
typedef int (*fptr_XError_handler)(Display *, XErrorEvent *);
static fptr_XError_handler default_error_handler;
static int XError_handler(Display *display, XErrorEvent *evt);
static int XError_uninstall(void);
static void XError_install(void);






/* ------------------------- */
/* Zigil API implementations */
/* ------------------------- */

zgl_Result zgl_LibInit(char *title, uint32_t flags) {
    vbs_putchar('\n');
    vbs_puts("+---------------------------+");
    vbs_puts("| ZIGIL: Creating X Display |");
    vbs_puts("+---------------------------+");
    vbs_putchar('\n');

    default_error_handler = XSetErrorHandler(XError_handler);

    // Later we check if the window that gets created actually has this width
    // and height
    x11_info.width = fb_info.upscale_width;
    x11_info.height = fb_info.upscale_height;

    zgl_LibTerm();
    if (x11_info.display != NULL) {
        puts("ZIGIL: X Display already exists where a new one is meant to be created.");
        goto ErrorPreDisplayAlloc;
    }

    vbs_puts("ZIGIL: Opening X Display...");
    x11_info.display = XOpenDisplay(NULL);
    if (x11_info.display == NULL) {
        puts("ZIGIL: Could not open X Display.");
        goto ErrorPreDisplayAlloc;
    }
    vbs_puts("ZIGIL: Opened X Display.");
    vbs_putchar('\n');
    print_Display(x11_info.display);
    vbs_putchar('\n');
    
    Display *dpy = x11_info.display;
    
    x11_info.i_scr = DefaultScreen(dpy);
    print_Display_and_Screen(dpy, x11_info.i_scr);
    vbs_putchar('\n');
    x11_info.scr = ScreenOfDisplay(dpy, x11_info.i_scr);
    print_Screen(x11_info.scr); // scr not actually used after this
    vbs_putchar('\n');
    
    vbs_puts("ZIGIL: X Display and Screen configured.");
    
    vbs_puts("ZIGIL: Creating X Window...");
    x11_info.root_win = DefaultRootWindow(dpy);
    XSetWindowAttributes set_attr = {
        .background_pixmap = None, //default
        .background_pixel = 0xFF4DFF,//BlackPixel(dpy, i_scr),
        .border_pixmap = None, //default
        .border_pixel = BlackPixel(dpy, x11_info.i_scr),
        .bit_gravity = ForgetGravity, //default
        .win_gravity = NorthWestGravity, //default
        .backing_store = NotUseful, //default
        .backing_planes = AllPlanes, //default
        .backing_pixel = 0, //default
        .save_under = False, //default
        .event_mask = ZGL_XEVENT_MASK,
        .do_not_propagate_mask = 0, //default
        .override_redirect = False, //default
        .colormap = CopyFromParent, //default
        .cursor = None //default
    };
    
    unsigned long attr_mask = CWEventMask | CWBackPixel | CWBorderPixel;
    
    x11_info.win_info.win = XCreateWindow //TODO: How do I check for failure?
        (dpy,                    /* display */
         x11_info.root_win,      /* parent */
         0,                      /* x */
         0,                      /* y */
         INIT_SCR_WIDTH,         /* width */ /* we resize later */
         INIT_SCR_HEIGHT,        /* height */ /* we resize later */
         0,                      /* border width */
         CopyFromParent,         /* depth */
         InputOutput,            /* class */
         CopyFromParent,         /* visual */
         attr_mask,              /* valuemask */
         &set_attr               /* attributes */
         );

    vbs_puts("ZIGIL: Created X Window.");
    
    Window win = x11_info.win_info.win; // A variable of type Window is really
                                        // an XID which is really an unsigned
                                        // long. Just a handle.

    {
        /* Get and store various attributes for later comparison to what was
           requested. */
        XWindowAttributes get_attr;
        XGetWindowAttributes(dpy, win, &get_attr);    
        x11_info.depth = get_attr.depth;
        x11_info.win_vis = get_attr.visual;
        Visual *vis = x11_info.win_vis;
        if (vis->class != TrueColor) {
            puts("ZIGIL: True color display required.\n");
            goto ErrorPostWindowAlloc;
        }
        unsigned long r_mask, /* g_mask, */ b_mask;
        r_mask = x11_info.win_vis_red_mask = vis->red_mask & 0xFFFFFF;
        /* g_mask =*/ x11_info.win_vis_green_mask = vis->green_mask & 0xFFFFFF;
        b_mask = x11_info.win_vis_blue_mask = vis->blue_mask & 0xFFFFFF;
        if (b_mask == 0xFF0000 && r_mask == 0x0000FF) {
            puts("ZIGIL: Pixel format BGR detected, but only RGB is supported.");
            goto ErrorPostWindowAlloc;
        }
        print_Visual(vis);
        vbs_putchar('\n');
        vbs_puts("ZIGIL: X Window Visual configured.");
        vbs_putchar('\n');
    }


    {
        /* Begin setup related to Window Manager (which Xlib acts as a
           middleman). */
        WM_DELETE_WINDOW = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
        char *atom_name = XGetAtomName(dpy, WM_DELETE_WINDOW);
        vbs_printf("ZIGIL: Made atom `%s'.\n", atom_name);
        XFree(atom_name);
    
        XSetWMProtocols(dpy, win, &WM_DELETE_WINDOW, 1);
    
        XClassHint class_hint;
        class_hint.res_name = title; // careful
        class_hint.res_class = title; // careful
        XSetClassHint(dpy, win, &class_hint);

        XWMHints wm_hints;
        wm_hints.flags         = InputHint | StateHint | WindowGroupHint;
        wm_hints.input         = True;
        wm_hints.initial_state = NormalState;
        wm_hints.window_group  = win;
        XSetWMHints(dpy, win, &wm_hints);

        XSizeHints size_hints = { 0 };
        XResizeWindow(dpy, win, x11_info.width, x11_info.height);
        /* Don't allow the window to be resized by user. */
        size_hints.flags = PMinSize | PMaxSize | PBaseSize;
        size_hints.min_width = size_hints.max_width = size_hints.base_width = x11_info.width;
        size_hints.min_height = size_hints.max_height = size_hints.base_height = x11_info.height;
        XSetWMNormalHints(dpy, win, &size_hints);
    }
    

    
    /* TODO: Why do we do this? We don't set ANY of the GC values! */
    x11_info.win_gc = XCreateGC(dpy, win, 0, NULL);
    //XGCValues gcv;
    //unsigned long gcv_mask = (1LU << 23)-1;
    //get ALL gcvalues
    //XGetGCValues(dpy, x11_info.gc, gcv_mask, &gcv);
    //print_XGCValues(&gcv);

    XMapRaised(dpy, win);



    
    XEvent evt;
    // Clear all StructureNotifyMask type events from the queue???
    do {
        XMaskEvent(dpy, StructureNotifyMask, &evt);
    } while ((evt.type != MapNotify) || (evt.xmap.event != win));

    // TODO: Isn't the window already given this mask?
    XSelectInput(dpy, win, ZGL_XEVENT_MASK);    
    XStoreName(dpy, win, title);
    XSetIconName(dpy, win, title);

    {
        XWindowAttributes get_attr;
        XGetWindowAttributes(dpy, win, &get_attr);
        if (get_attr.width != x11_info.width
            || get_attr.height != x11_info.height) {
            printf("ZIGIL: Window has the wrong size somehow.\n");
            goto ErrorPostGCAlloc;
        }
    
        print_XWindowAttributes(&get_attr);
        vbs_putchar('\n');
        vbs_printf("ZIGIL: X Window configured.\n");
    }

    
    vbs_putchar('\n');
    vbs_puts("ZIGIL: Creating X Framebuffer...");
    if (!init_xframebuffer(x11_info.width, x11_info.height, flags & ZGL_VFLAG_VIDFAST)) {
        puts("ZIGIL: Couldn't create XImage framebuffer.");
        goto ErrorPostGCAlloc;
    }
    vbs_puts("ZIGIL: Created X Framebuffer.");
    
    print_XImage(x11_info.win_info.ximage);
    vbs_putchar('\n');
    
    x11_info.ximage_width = x11_info.win_info.ximage->width;
    x11_info.ximage_height = x11_info.win_info.ximage->height;
    x11_info.ximage_depth = x11_info.win_info.ximage->depth;
    x11_info.ximage_bitspp = x11_info.win_info.ximage->bits_per_pixel;
    x11_info.ximage_red_mask = x11_info.win_info.ximage->red_mask;
    x11_info.ximage_green_mask = x11_info.win_info.ximage->green_mask;
    x11_info.ximage_blue_mask = x11_info.win_info.ximage->blue_mask;

    vbs_puts("ZIGIL: X Framebuffer configured.");
    vbs_putchar('\n');
    
    print_X11_info();
    vbs_putchar('\n');
    
    init_XEvent();
    
    video_blit();
    video_sync();

    return ZR_SUCCESS;
    
 ErrorPostGCAlloc:
    XFreeGC(dpy, x11_info.win_gc);
 ErrorPostWindowAlloc:
    XDestroyWindow(dpy, win);
    //ErrorPostDisplayAlloc:
    XCloseDisplay(dpy);
 ErrorPreDisplayAlloc:
    return ZR_ERROR;
}

zgl_Result zgl_LibValidate(void) {
    /* Sanity check: ensure that the data given to X matches that of the created
       X Window and the XImage, where applicable. */

    /* X Window checks */
    if (fb_info.upscale_width != (uint32_t)x11_info.width) {
        return ZR_ERROR;
    }
    
    if (fb_info.upscale_height != (uint32_t)x11_info.height) {
        return ZR_ERROR;
    }

    if (fb_info.bits_per_color != (uint32_t)x11_info.depth) {
        return ZR_ERROR;
    }
    
    if (fb_info.r_mask != x11_info.win_vis_red_mask) {
        return ZR_ERROR;
    }

    if (fb_info.g_mask != x11_info.win_vis_green_mask) {
        return ZR_ERROR;
    }

    if (fb_info.b_mask != x11_info.win_vis_blue_mask) {
        return ZR_ERROR;
    }


    /* XImage checks */
    if (fb_info.upscale_width != (uint32_t)x11_info.ximage_width) {
        return ZR_ERROR;
    }
    
    if (fb_info.upscale_height != (uint32_t)x11_info.ximage_height) {
        return ZR_ERROR;
    }

    if (fb_info.bits_per_color != (uint32_t)x11_info.ximage_depth) {
        return ZR_ERROR;
    }

    if (fb_info.bits_per_pixel != (uint32_t)x11_info.ximage_bitspp) {
        return ZR_ERROR;
    }
    
    if (fb_info.r_mask != x11_info.ximage_red_mask) {
        return ZR_ERROR;
    }

    if (fb_info.g_mask != x11_info.ximage_green_mask) {
        return ZR_ERROR;
    }

    if (fb_info.b_mask != x11_info.ximage_blue_mask) {
        return ZR_ERROR;
    }

    return ZR_SUCCESS;;
}

void zgl_VideoUpdate(void) {
    video_blit();
    video_sync();
}

void zgl_LibTerm(void) {
    term_XEvent();
    
    if (x11_info.display == NULL) {
        return;
    }
    if (x11_info.win_info.ximage != NULL) {
#if X11_HAS_SHM_EXT
        if (x11_info.use_shm) {
            XShmDetach(x11_info.display, &x11_info.shminfo);
            shmdt (x11_info.shminfo.shmaddr);
            shmctl(x11_info.shminfo.shmid, IPC_RMID, 0);
            XSync(x11_info.display, False);
        }
#endif
        XDestroyImage(x11_info.win_info.ximage);
        x11_info.win_info.ximage = NULL;
    }

    XUnmapWindow(x11_info.display, x11_info.win_info.win);

    x11_info.win_vis = NULL; // TODO is this okay?

    if (x11_info.win_gc != None) {
        XFreeGC(x11_info.display, x11_info.win_gc);
        x11_info.win_gc = None;
    }

    if (x11_info.win_info.win != None) {
        XUnmapWindow  (x11_info.display, x11_info.win_info.win);
        XDestroyWindow(x11_info.display, x11_info.win_info.win);
        x11_info.win_info.win = None;
    }

    if (x11_info.win_info.win != None) { // wtf
        XDestroyWindow(x11_info.display, x11_info.win_info.win);
        x11_info.win_info.win = None;
    }

    XCloseDisplay(x11_info.display);
    x11_info.display = NULL;
}

void zgl_BlitToFramebuffer
(zgl_Pixit fb_x, zgl_Pixit fb_y,
 zgl_PixelArray const *const src,
 zgl_Pixit src_x, zgl_Pixit src_y,
 zgl_Pixit w, zgl_Pixit h) {
    if (src == NULL)
        return;
    
    zgl_Color *dst_pixels = fb_info.fb.pixels;
    uint32_t src_w = src->w;
    uint32_t dst_w = fb_info.width;
    //uint32_t dst_h = fb_info.height;
    size_t num = w*sizeof(zgl_Color);

    zgl_Color *p_src_rowstart =
        src->pixels + ((src->w)*(src_y + 0)) + src_x;
    zgl_Color *p_dst_rowstart =
        dst_pixels + ((dst_w)*(fb_y + 0)) + fb_x;
    
    for (int i = 0; i <= h-1; i++) {
        memcpy(p_dst_rowstart, p_src_rowstart, num);
        p_src_rowstart += src_w;
        p_dst_rowstart += dst_w;
    }
    
    return;
}

zgl_Result zgl_SetWindowTitle(char const *title) {
    XChangeProperty(x11_info.display, x11_info.win_info.win, XA_WM_NAME, XA_STRING, 8, PropModeReplace, (unsigned char *)title, (int)strlen(title));
    return 0;
}




/* ----------------------------- */
/* Internal function definitions */
/* ----------------------------- */

static zgl_Result init_xframebuffer(int width, int height, int shm) {
    XImage  *img = NULL;
    Display *dpy = x11_info.display;
    Visual  *vis = x11_info.win_vis;
    int    depth = x11_info.depth;
   
    if (dpy == NULL) {
        return 0;
    }

#if X11_HAS_SHM_EXT
    x11_info.use_shm = shm && test_xshm(dpy) && test_xlocal();
#else
    x11_info.use_shm = test_xshm(dpy);
#endif


#if X11_HAS_SHM_EXT
    long shmmx;
    int shmsz;
    int guess, success;
    
    if (x11_info.use_shm) {
        shmmx = find_shmmax(&guess);
        XError_install();
        vbs_printf("[xvid] max shared memory segment size [%s]: %ld\n",
                       guess ? "guessed" : "actual",
                       shmmx);
        img = XShmCreateImage(dpy, vis, depth, ZPixmap, 0, &x11_info.shminfo, width, height);
        if (XError_uninstall() || img == NULL) {
            printf("[xvid] Error unable to create SHM image\n");
            goto shm_err_nullimg;
        }
        shmsz = img->bytes_per_line * img->height;
        if (shmsz > shmmx) {
            vbs_printf("[xvid] [WARNING] videobuf > than max shared mem segment:\n");
            vbs_printf("[xvid] %d > %ld\n", shmsz, shmmx);
        }
        x11_info.shminfo.shmid = shmget(IPC_PRIVATE, shmsz, IPC_CREAT | 0777);
        if (x11_info.shminfo.shmid < 0) {
            printf("[xvid] error shmget of size %d failed\n", shmsz);
            goto shm_err_failshmget;
        }
        
        img->data = shmat(x11_info.shminfo.shmid, NULL, 0);
        x11_info.shminfo.shmaddr = img->data;
        if (x11_info.shminfo.shmaddr == (char*) -1) {
            printf("[xvid] error shmat failed\n");
            goto shm_err_failshmat;
        }
        
        x11_info.shminfo.readOnly = False;
        XError_install();
        success = XShmAttach(dpy, &x11_info.shminfo);
        if (XError_uninstall() || !success) {
            printf("[xvid] error attaching shared memory info to XDisplay\n");
            goto shm_err_failattach;
        }
        
        XError_install();
        /* test SHM put */
        GC gc = XCreateGC(dpy, x11_info.win_info.win, 0, NULL);
        XShmPutImage(dpy, x11_info.win_info.win, gc, img, 0, 0, 0, 0, 1, 1, False);
        XSync(dpy, False);
        XFreeGC(dpy, gc);
        if (XError_uninstall() || !success) {
            printf("[xvid] error testing SHM put onto XDisplay\n");
            goto shm_err_failattach; /* same goto as attach failure */
        }
        
        XSync(dpy, False);
        goto shm_success;
        
    shm_err_failattach: shmdt (x11_info.shminfo.shmaddr);
    shm_err_failshmat:  shmctl(x11_info.shminfo.shmid, IPC_RMID, 0);
    shm_err_failshmget: XDestroyImage(img); img = NULL;
    shm_err_nullimg:    x11_info.use_shm = 0;
        /* marks shared mem for deletion after X server+client detach */
    shm_success:        shmctl(x11_info.shminfo.shmid, IPC_RMID, 0);
        vbs_printf("[xvid] using X-Windows SHM extension: %d\n", x11_info.use_shm);
    }
#endif
    
    if (img == NULL) {
        img = XCreateImage
            (dpy,     /* display */
             vis,     /* visual  */
             depth,   /* depth   */
             ZPixmap, /* format  */
             0,       /* offset  */
             NULL,    /* data    */
             width,   /* width   */
             height,  /* height  */
             XBitmapPad(dpy), /* bitmap_pad */
             0 /* bytes_per_line (since offset is 0, this value is ignored)*/
             ); 
        if (img != NULL) {
            // This is freed by XDestroyImage, and so we DON'T use zgl_Malloc.
            img->data = zgl_Malloc(img->bytes_per_line * img->height);
            if (img->data == NULL) {
                XDestroyImage(img);
                img = NULL;
            }
        }
    }
    x11_info.win_info.ximage = img;
    XError_install();
    return (img != NULL);
}

static void upscale_framebuffer_to_ximage
(unsigned *src, unsigned src_w, unsigned src_h,
 unsigned *dst, unsigned dst_w, unsigned dst_h)
{
    register unsigned *p_dst, *p_src;
    int ratio_x, ratio_y, acc;
    unsigned x, y;
        
    ratio_x = ((src_w << 16) / dst_w) + 1; // ratio src_w to dst_w, in FP .16 form
    ratio_y = ((src_h << 16) / dst_h) + 1; // ratio src_h to dst_w, in FP .16 form
    for (y = 0; y < dst_h; y++) { // for each row
        p_dst = dst + (y * dst_w); // first location of y'th row of dst
        p_src = src + ((y * ratio_y) >> 16) * src_w;
        acc = 0;
        for (x = 0; x < dst_w; x++) { // for each column
            *p_dst++ = p_src[acc >> 16];
            acc += ratio_x;
        }
    }
}

static void video_sync(void) {
    if (x11_info.display == NULL) {
        return;
    }
    XSync(x11_info.display, False);
}

static void video_blit(void) {
    /* Internal blit function specifically designed for blitting the framebuffer
       to the screen using X library. Other zgl_PixelArray objects may blit
       amongst themselves using zgl_BlitPixelArray(), which is
       library-independent */

    // Basic pattern:
    // fb_info.fb --> x11_info.ximage --> x11_info.win
    
    Display *dpy = x11_info.display;
    Window win = x11_info.win_info.win;
    
    unsigned *vid = (unsigned *)fb_info.fb.pixels;
    unsigned src_w, src_h;
    unsigned dst_w, dst_h;

    if ((vid == NULL) || (dpy == NULL)) {
        return;
    }
    src_w = fb_info.width;
    src_h = fb_info.height;
    dst_w = fb_info.upscale_width;
    dst_h = fb_info.upscale_height;

    if (fb_info.upscale_shift == 0) {
        memcpy
            (x11_info.win_info.ximage->data,   /* dest */
             vid,                       /* source */
             dst_w * dst_h * fb_info.bytes_per_pixel
             );
    } else {
        upscale_framebuffer_to_ximage(vid, src_w, src_h, (unsigned *)x11_info.win_info.ximage->data, dst_w, dst_h);
    }

    if (x11_info.use_shm) {
#if X11_HAS_SHM_EXT
        XShmPutImage
            (dpy, /* display */
             win, /* dest drawable */ 
             x11_info.win_gc, /* graphics context */
             x11_info.win_info.ximage, /* source image */
             0, /* source x */
             0, /* source y */
             0, /* dest x */
             0, /* dest y */
             dst_w, /* width */
             dst_h,  /* height */
             False
             );
#endif
    } else {
        XPutImage
            (dpy, /* display */
             win, /* dest drawable */ 
             x11_info.win_gc, /* graphics context */
             x11_info.win_info.ximage, /* source image */
             0, /* source x */
             0, /* source y */
             0, /* dest x */
             0, /* dest y */
             dst_w, /* width */
             dst_h  /* height */
             );
    }
}









static int test_xlocal(void)
{
    int loc;
    char *name;
    
    loc = 0;
    name = XDisplayName(NULL);
    loc |= (name == NULL);
    if (name != NULL) {
        loc |= (name[0] == ':');
        loc |= (strncmp(name, "unix:", 5) == 0);
        loc |= (strstr (name, "xquartz:") != NULL); /* xquartz on mac */
    }
    return loc;
}

static int test_xshm(Display *display) {
#if X11_HAS_SHM_EXT
    int  major;
    int  minor;
    Bool b;
#endif
    int n;
    
    if (XQueryExtension(display, "MIT-SHM", &n, &n, &n) == False) {
        return 0;
    }
#if X11_HAS_SHM_EXT
    if (XShmQueryVersion(display, &major, &minor, &b) == False) {
        return 0;
    }
    return 1;
#else
    return 0;
#endif
}

static long find_shmmax(int *guess) {
    long  maxseg = (1 << 22); /* 4MB */
    
    /* unused variable handling suggested by https://www.reddit.com/user/skeeto/ */
    (void) guess;
    
    return maxseg;
}




static volatile bool xerrored = false;
static int XError_handler(Display *display, XErrorEvent *err) {
    char err_str[100] = {0};

    xerrored = true;
    XGetErrorText(display, err->error_code, err_str, sizeof (err_str));
    printf("ZIGIL: XErrorEvent: \n%s\n", err_str);
    
    puts("ZIGIL: Proceeding to default XError handler.");
    exit(0); // TODO: Remove exit
    default_error_handler(display, err);
    
    return 0;
}
static void XError_install(void)
{
    xerrored = false;
    
    XSetErrorHandler(XError_handler);
}
static int XError_uninstall(void)
{
    if (x11_info.display) {
        XSync(x11_info.display, False);
    }
    
    XSetErrorHandler(NULL);
    
    return xerrored;
}



#if 0
static zgl_Result create_xframebuffer(zgl_X11WindowInfo *xwi, int width, int height) {
    XImage  *img = NULL;
    Display *dpy = x11_info.display;
    Visual  *vis = x11_info.win_vis;
    int    depth = x11_info.depth;
   
    if (dpy == NULL) {
        return 0;
    }

#if X11_HAS_SHM_EXT
    int shm_segment_size;
    int success;
    
    if (x11_info.use_shm) {
        /* create shared memory ximage structure */
        img = XShmCreateImage(dpy,
                              vis,
                              depth,
                              ZPixmap,
                              0,
                              &xwi->shminfo,
                              width,
                              height);

        /* create shared memory segment */
        shm_segment_size = img->bytes_per_line * img->height;
        xwi->shminfo.shmid = shmget(IPC_PRIVATE, shm_segment_size, IPC_CREAT | 0777);
        if (xwi->shminfo.shmid < 0) {
            printf("[xvid] error shmget of size %d failed\n", shm_segment_size);
            goto shm_err_failshmget;
        }


        /* attached shared memory segment to process */
        xwi->shminfo.shmaddr = img->data = shmat(xwi->shminfo.shmid, NULL, 0);
        if (xwi->shminfo.shmaddr == (char*) -1) {
            printf("[xvid] error shmat failed\n");
            goto shm_err_failshmat;
        }
        xwi->shminfo.readOnly = False;
        success = XShmAttach(dpy, &xwi->shminfo);
        if (XError_uninstall() || !success) {
            printf("[xvid] error attaching shared memory info to XDisplay\n");
            goto shm_err_failattach;
        }
        
        /* test SHM put */
        GC gc = XCreateGC(dpy, xwi->win, 0, NULL);
        XShmPutImage(dpy, xwi->win, gc, img, 0, 0, 0, 0, 1, 1, False);
        XSync(dpy, False);
        XFreeGC(dpy, gc);
        if (!success) {
            printf("[xvid] error testing SHM put onto XDisplay\n");
            goto shm_err_failattach; /* same goto as attach failure */
        }
        
        XSync(dpy, False);
        goto shm_success;
        
    shm_err_failattach:
        shmdt (xwi->shminfo.shmaddr);
    shm_err_failshmat:
        shmctl(xwi->shminfo.shmid, IPC_RMID, 0);
    shm_err_failshmget:
        XDestroyImage(img); img = NULL;
    shm_success:
        shmctl(xwi->shminfo.shmid, IPC_RMID, 0); /* marks shared memory for
                                                    deletion after X
                                                    server+client detach */
    }
#endif
    
    if (img == NULL) {
        img = XCreateImage
            (dpy,     /* display */
             vis,     /* visual  */
             depth,   /* depth   */
             ZPixmap, /* format  */
             0,       /* offset  */
             NULL,    /* data    */
             width,   /* width   */
             height,  /* height  */
             XBitmapPad(dpy), /* bitmap_pad */
             0 /* bytes_per_line (since offset is 0, this value is ignored)*/
             ); 
        if (img != NULL) {
            // This is freed by XDestroyImage, and so we DON'T use zgl_Malloc.
            img->data = malloc(img->bytes_per_line * img->height);
            if (img->data == NULL) {
                XDestroyImage(img);
                img = NULL;
            }
        }
    }
    
    xwi->ximage = img;
    return (img != NULL);    
}



zgl_Result create_OS_window(char *title, int width, int height) {
    vbs_putchar('\n');
    vbs_puts("------------------------------------");
    vbs_puts("- ZIGIL: Creating Top-Level Window -");
    vbs_puts("------------------------------------");
    vbs_putchar('\n');

    // Later we check if the window that gets created actually has this width
    // and height
    x11_dbginfo.width = width;
    x11_dbginfo.height = height;
    
    Display *dpy = x11_info.display;
    
    XSetWindowAttributes set_attr = {
        .background_pixmap = None, //default
        .background_pixel = BlackPixel(dpy, x11_info.i_scr),
        .border_pixmap = None, //default
        .border_pixel = BlackPixel(dpy, x11_info.i_scr),
        .bit_gravity = ForgetGravity, //default
        .win_gravity = NorthWestGravity, //default
        .backing_store = NotUseful, //default
        .backing_planes = AllPlanes, //default
        .backing_pixel = 0, //default
        .save_under = False, //default
        .event_mask = NoEventMask,
        .do_not_propagate_mask = 0, //default
        .override_redirect = False, //default
        .colormap = CopyFromParent, //default
        .cursor = None //default
    };
    
    unsigned long attr_mask = CWBackPixel | CWBorderPixel;
    
    x11_dbginfo.win = XCreateWindow //TODO: How do I check for failure?
        (dpy,                    /* display */
         x11_info.root_win,  /* parent */
         0,                      /* x */
         0,                      /* y */
         256,         /* width */ /* we resize later */
         512,        /* height */ /* we resize later */
         0,                      /* border width */
         CopyFromParent,         /* depth */
         InputOutput,            /* class */
         CopyFromParent,         /* visual */
         attr_mask,              /* valuemask */
         &set_attr               /* attributes */
         );

    vbs_puts("ZIGIL: Created X Window.");
    
    Window win = x11_dbginfo.win;
    // A variable of type Window is really an XID
    // which is really an unsigned long. Just a handle.

    XSetWMProtocols(dpy, win, &WM_DELETE_WINDOW, 1);
    
    {
        /* Begin setup related to Window Manager (which Xlib acts as a
           middleman). */
      
        XClassHint class_hint;
        class_hint.res_name = title; // careful
        class_hint.res_class = title; // careful
        XSetClassHint(dpy, win, &class_hint);

        XWMHints wm_hints;
        wm_hints.flags         = InputHint | StateHint | WindowGroupHint;
        wm_hints.input         = False;
        wm_hints.initial_state = NormalState;
        wm_hints.window_group  = win;
        XSetWMHints(dpy, win, &wm_hints);

        XSizeHints size_hints = { 0 };
        XResizeWindow(dpy, win, x11_dbginfo.width, x11_dbginfo.height);
        /* Don't allow the window to be resized by user. */
        size_hints.flags = PMinSize | PMaxSize | PBaseSize;
        size_hints.min_width = size_hints.max_width = size_hints.base_width = x11_dbginfo.width;
        size_hints.min_height = size_hints.max_height = size_hints.base_height = x11_dbginfo.height;
        XSetWMNormalHints(dpy, win, &size_hints);
    }

    XMapRaised(dpy, win);

    // TODO: Isn't the window already given this mask?
    XStoreName(dpy, win, title);
    XSetIconName(dpy, win, title);
    
    vbs_putchar('\n');
    vbs_puts("ZIGIL: Creating X Framebuffer...");
    if (!create_xframebuffer(&x11_dbginfo, x11_dbginfo.width, x11_dbginfo.height)) {
        puts("ZIGIL: Couldn't create XImage framebuffer.");
        goto Error;
    }
    vbs_puts("ZIGIL: Created X Framebuffer.");
    
    video_blit();
    video_sync();

    return ZR_SUCCESS;
    
 Error:
    XDestroyWindow(dpy, win);
    //ErrorPostDisplayAlloc:
    XCloseDisplay(dpy);
    return ZR_ERROR;
}
#endif
