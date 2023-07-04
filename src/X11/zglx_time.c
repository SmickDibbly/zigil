#define __USE_POSIX199309 1
#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define INCLUDE_SRC
#include "src/zigil_time.c"
#undef INCLUDE_SRC

/* API functions implemented here. */
extern zgl_Result zgl_ClockInit(void);
extern utime_t    zgl_ClockQuery(void);
extern ntime_t    zgl_ClockQueryNano(void);
extern zgl_Result zgl_ClockSleep(utime_t duration);

/* Internals */
#define USED_CLOCK CLOCK_REALTIME
#define NSEC_PER_SEC 1000000000LL

/* Hires data */
static uint64_t start = 0; // nanoseconds
/* Lores data */
// TODO ???

zgl_Result zgl_ClockInit(void) {
    static bool clock_init = false;
    
    if (!clock_init) {
        clock_hires_supported = true;

        struct timespec ts;
        if (-1 == clock_gettime(USED_CLOCK, &ts)) {
            puts("ZIGIL: Could not start the clock.");
            return ZR_ERROR;
        }
        start = (ts.tv_sec)*NSEC_PER_SEC + (ts.tv_nsec);
        clock_init = true;
    }

    return ZR_SUCCESS;
}

utime_t zgl_ClockQuery(void) {
    struct timespec query;
    uint64_t nanoquery;
    
    switch (clock_mode) {
    case ZGL_CLOCK_MODE_LORES:
    case ZGL_CLOCK_MODE_HIRES:
        if (-1 == clock_gettime(USED_CLOCK, &query)) {
            puts("ZIGIL: Failed to sample the time.");
            return ZR_ERROR;
        }
        nanoquery = (query.tv_sec)*NSEC_PER_SEC + (query.tv_nsec);
        return (utime_t)((1000 * (nanoquery - start))/NSEC_PER_SEC);
    default:
        printf("How???\n%s : %i\n", __FILE__, __LINE__);
        exit(0);
    }
       
}

ntime_t zgl_ClockQueryNano(void) {
    struct timespec query;
    uint64_t nanoquery;

    switch (clock_mode) {
    case ZGL_CLOCK_MODE_LORES:
    case ZGL_CLOCK_MODE_HIRES:
        if (-1 == clock_gettime(USED_CLOCK, &query)) {
            puts("ZIGIL: Failed to sample the time.");
            return ZR_ERROR;
        }
        nanoquery = (query.tv_sec)*NSEC_PER_SEC + (query.tv_nsec);
        return (ntime_t)(nanoquery - start);
    default:
        printf("How???\n%s : %i\n", __FILE__, __LINE__);
        exit(0);
    }
       
}

zgl_Result zgl_ClockSleep(utime_t duration) {
    struct timespec ts;

    ts.tv_sec = duration/1000;
    ts.tv_nsec = (duration % 1000) * 1000000;
    
    if (-1 == nanosleep(&ts, NULL)) {
        return ZR_ERROR;
    }

    return ZR_SUCCESS;
}
