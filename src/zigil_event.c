#include <stdint.h>
#include <stdlib.h>

#include "src/zigil_event.h"

//must be power of 2 so that "% QUEUE_LEN" becomes "& (QUEUE_LEN - 1)"
#define QUEUE_LEN 256

typedef struct zgl_EventQueue {
    zgl_Event ring[QUEUE_LEN];
    uint32_t i_head; // whence the first popped event comes
    uint32_t i_tail; // where the next pushed event will go
    uint32_t size;
} zgl_EventQueue;

static zgl_EventQueue queue = {0};

zgl_Result zgl_PopEvent(zgl_Event *evt) {
    if (queue.i_head == queue.i_tail) {
        evt = NULL;
        return ZR_NOEVENT;
    }

    *evt = queue.ring[queue.i_head];
    queue.i_head = (queue.i_head + 1) & (QUEUE_LEN - 1);
    queue.size--;

    return ZR_SUCCESS;
}

zgl_Result zgl_PushEvent(zgl_Event *evt) {
    if (evt == NULL) {
        return ZR_ERROR;
    }
    
    queue.ring[queue.i_tail] = *evt;
    queue.i_tail = (queue.i_tail + 1) & (QUEUE_LEN -1);
    queue.size++;

    return ZR_SUCCESS;
}

extern zgl_Result _zgl_WaitUpdateEventQueue(void);

zgl_Result zgl_WaitPopEvent(zgl_Event *evt) {
    if (queue.i_head == queue.i_tail) {
        _zgl_WaitUpdateEventQueue();
    }

    *evt = queue.ring[queue.i_head];
    queue.i_head = (queue.i_head + 1) & (QUEUE_LEN - 1);
    queue.size--;
    
    return ZR_SUCCESS;
}

zgl_Result zgl_PushCloseEvent(void) {
    zgl_Event evt;
    evt.type = EC_CloseRequest;
    zgl_PushEvent(&evt);

    return ZR_SUCCESS;
}
