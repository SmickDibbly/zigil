#include <stdio.h>

#include "src/zigil_time.h"

static bool clock_hires_supported = false;
static int clock_mode = ZGL_CLOCK_MODE_LORES;

zgl_Result zgl_ClockMode(int mode) {
    zgl_ClockInit();

    if (!clock_hires_supported) {
        clock_mode = ZGL_CLOCK_MODE_LORES;
        return ZR_SUCCESS;
    }
    
    switch (mode) {
    case ZGL_CLOCK_MODE_LORES:
    case ZGL_CLOCK_MODE_HIRES:
        clock_mode = mode;
        return ZR_SUCCESS;
    default:
        puts("ZIGIL: Invalid clock mode.");
        return ZR_ERROR;
    }
}

/* zglx_time.c continues below */
