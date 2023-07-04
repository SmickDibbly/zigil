#include <stdio.h>

#include "src/Windows/zglwin.h"
#include <mmsystem.h> // windows time functions

#define INCLUDE_SRC
#include "src/zigil_time.c"
#undef INCLUDE_SRC

/* API functions implemented here. */
extern zgl_Result zgl_ClockInit(void);
extern utime_t zgl_ClockQuery(void);
extern ntime_t zgl_ClockQueryNano(void);
extern zgl_Result zgl_ClockSleep(utime_t duration);
    
/* Internals */
/* Hires data */
static LONGLONG QPC_start = 0; // ticks
static LONGLONG QPC_freq  = 0; // ticks per second
/* Lores data */
static DWORD TGT_start = 0;

zgl_Result zgl_ClockInit(void) {
    static bool clock_init = false;
    
    if (!clock_init) {
        LARGE_INTEGER query;
        if (QueryPerformanceFrequency(&query)) {
            clock_hires_supported = true;
            
            QPC_freq = query.QuadPart;
            if (QPC_freq == 0) {
                QPC_freq = 1;
            }
            QueryPerformanceCounter(&query);
            QPC_start = query.QuadPart;
        }
        else {
            clock_hires_supported = false;
        }
    
        timeBeginPeriod(1); // TODO: Where is the required call to timeEndPeriod(1)?
        TGT_start = timeGetTime();
        clock_init = true;
    }

    return ZR_SUCCESS;
}

utime_t zgl_ClockQuery(void) {
    LARGE_INTEGER query;

    switch (clock_mode) {
    case ZGL_CLOCK_MODE_LORES:
        return timeGetTime() - TGT_start;
    case ZGL_CLOCK_MODE_HIRES:
        QueryPerformanceCounter(&query);
        return (utime_t)((1000 * (query.QuadPart - QPC_start)) / QPC_freq);
    default:
        printf("How???\n%s : %i\n", __FILE__, __LINE__);
        exit(0);
    }
       
}

ntime_t zgl_ClockQueryNano(void) {
    LARGE_INTEGER query;

    switch (clock_mode) {
    case ZGL_CLOCK_MODE_LORES:
        return timeGetTime() - TGT_start;
    case ZGL_CLOCK_MODE_HIRES:
        QueryPerformanceCounter(&query);
        return (ntime_t)((1000000000LL * (query.QuadPart - QPC_start)) / QPC_freq);
    default:
        printf("How???\n%s : %i\n", __FILE__, __LINE__);
        exit(0);
    }
}

zgl_Result zgl_ClockSleep(utime_t duration) {
    Sleep(duration); // sleep has no return value

    return ZR_SUCCESS;
}
