#ifndef ZGLWIN_H
#define ZGLWIN_H

#include "src/zigil.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOCRYPT
#define NOMENUS
#define NODRAWTEXT
#define NOMEMMGR
#define NOCOMM
#define NOKANJI
#define NONLS
#define NOKERNEL
#define OEMRESOURCE
#undef UNICODE
#define WINVER 0x0605
#include <windows.h>

//#define WIN_VERBOSITY

typedef struct zgl_WindowsInfo {
    HWND handle;
    HINSTANCE inst;
    HBITMAP bitmap;
    DWORD style;
    WNDCLASSEX class;
    ATOM atom;
    LPCSTR atom_lpcstr; // do not print
    RAWINPUTDEVICE rid[2];
    
    int width,
        height,
        depth;
        
} zgl_WindowsInfo;
extern zgl_WindowsInfo win_info;

#endif /* ZGLWIN_H */
