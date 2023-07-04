#include "src/X11/zglx.h"
#include <stdio.h>

static int64_t clock_ticks_per_sec;
static int64_t pagesize;
static int64_t mem;
static int64_t size;
static int64_t resident;
static int64_t share;
static int64_t data;

#include <unistd.h> // for sysconf(_SC_PAGESIZE)
#define STATM_PATH "/proc/self/statm"

int64_t zgl_GetMemoryUsage(void) {
    static bool _init = false;
    if ( ! _init) {
        pagesize = sysconf(_SC_PAGESIZE);
        clock_ticks_per_sec = sysconf(_SC_CLK_TCK);
        _init = true;
    }

    int64_t ignored;
    FILE *fp = fopen(STATM_PATH, "r");
    if ( ! fp) {
        printf("ZIGIL.WARN: Could not get memory usage data; failed to access " STATM_PATH);
        size     = -2;
        resident = -2;
        share    = -2;
        data     = -2;
        mem      = -2;
        return mem;
    }

    if (7 != fscanf(fp, "%li %li %li %li %li %li %li",
                    &size,
                    &resident, // "inaccurate" according to statm docs
                    &share, // "inaccurate" according to statm docs
                    &ignored,
                    &ignored,
                    &data,
                    &ignored)) {
        printf("ZIGIL.WARN: Could not get memory usage data; accessed but failed to parse " STATM_PATH);
        size     = -1;
        resident = -1;
        share    = -1;
        data     = -1;
        mem      = -1;
        fclose(fp);
        return mem;
    }
    else {
        // The multiplication by state.sys.pagesize yields a value in bytes. The right shift by 20 yields a value in "mebibytes" ie. MiB, which is similar to megabytes.
        // NB: Chances are that state.sys.pagesize is 4096, but this cannot be guaranteed.
        size     = (size     * pagesize);
        resident = (resident * pagesize);
        share    = (share    * pagesize);
        data     = (data     * pagesize);
        mem      = (resident - share);
    }
    fclose(fp);
    
    return mem;
}
