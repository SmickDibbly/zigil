#ifndef ZIGIL_INPUT_H
#define ZIGIL_INPUT_H

#include "zigil_keycodes.h"
#include "zigil.h"

typedef enum zgl_CursorCode {
    ZC_arrow,
    ZC_drag,
    ZC_crosshair,
    ZC_plus,
    ZC_cross,
    ZC_text,

    ZC_empty, // ZC_empty must be second-last, before NUM_ZC

    NUM_ZC
} zgl_CursorCode;

extern zgl_Pixel zgl_GetPointerPos();
extern void zgl_SetPointerPos(int x, int y);
extern void zgl_SetCursor(zgl_CursorCode code);

extern void zgl_SetPointerCapture(void);
extern void zgl_ReleasePointerCapture(void);

extern void zgl_SetRelativePointer(void);
extern void zgl_ReleaseRelativePointer(zgl_Pixel *pos);



#endif
