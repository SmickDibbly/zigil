#ifndef ZIGIL_TIME_H
#define ZIGIL_TIME_H

#include "zigil.h"

typedef uint32_t utime_t;
typedef uint64_t ntime_t;

#define ZGL_CLOCK_MODE_LORES   0 /* low resolution clock (default) */
#define ZGL_CLOCK_MODE_HIRES   1 /* high resolution clock */

extern zgl_Result zgl_ClockInit(void);
extern zgl_Result zgl_ClockMode(int mode);
extern utime_t    zgl_ClockQuery(void);
extern ntime_t    zgl_ClockQueryNano(void);
extern zgl_Result zgl_ClockSleep(utime_t duration);

#endif
