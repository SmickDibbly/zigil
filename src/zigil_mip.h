#ifndef ZIGIL_MIP_H
#define ZIGIL_MIP_H

#include "zigil.h"

#define MAX_MIP_LEVELS 16
// maximum size for a mipmapped texture is 256 = 2^8, so max mip levels is 8

typedef struct zgl_MipMap {
    zgl_Color *pixels;
    size_t num_levels;
    size_t level_offset[MAX_MIP_LEVELS];
    size_t w[MAX_MIP_LEVELS];
} zgl_MipMap;

extern zgl_MipMap *zgl_MipifyPixelArray
(zgl_PixelArray const *pixarr);

extern zgl_Result zgl_DestroyMipMap
(zgl_MipMap *const mipmap);

extern void zgl_BlitEntireMipMap
(zgl_PixelArray *dst,
 zgl_mPixelRect dst_rect,
 zgl_MipMap const *src,
 zgl_mPixelRect *bounds);

#endif /* ZIGIL_MIP_H */
