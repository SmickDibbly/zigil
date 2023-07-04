#ifndef ZGLX_H
#define ZGLX_H

#include "src/zigil.h"
#include <X11/Xlib.h>

#define X11_HAS_SHM_EXT 1

#if X11_HAS_SHM_EXT
//#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#endif

//#define X_VERBOSITY

extern Atom WM_DELETE_WINDOW;

typedef struct zgl_X11WindowInfo {
    Window win;
    int width, height;
    XImage *ximage;
#if X11_HAS_SHM_EXT
    XShmSegmentInfo shminfo;
#endif
} zgl_X11WindowInfo;


typedef struct zgl_X11Info {
    Display *display;

    Screen *scr; // not really needed
    int   i_scr;
    
    Window root_win;
    zgl_X11WindowInfo win_info;

    int width, height;
    int depth;
    
    Visual *win_vis;
    unsigned long win_vis_red_mask,
        win_vis_green_mask,
        win_vis_blue_mask;

    GC win_gc;
    
    int ximage_width,
        ximage_height,
        ximage_depth,
        ximage_bitspp;
    unsigned long ximage_red_mask,
        ximage_green_mask,
        ximage_blue_mask;

#if X11_HAS_SHM_EXT
    XShmSegmentInfo shminfo;
#endif
    bool use_shm;
} zgl_X11Info;
extern zgl_X11Info x11_info;






extern zgl_X11WindowInfo x11_dbginfo;

#endif /* ZGLX_H */
