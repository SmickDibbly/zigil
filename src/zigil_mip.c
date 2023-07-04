#include "src/zigil_mip.h"
#include "src/zigil_mem.h"

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

zgl_MipMap *zgl_MipifyPixelArray
(zgl_PixelArray const *pixarr) {
    // TODO: forbid non-powers-of-two
    // TODO: forbid width greater than width 256

    zgl_Color *src;
    zgl_Color *dst;
    zgl_Pixit src_w;
    zgl_Pixit dst_w;
    int level;
    
    zgl_MipMap *mip = zgl_Malloc(sizeof(*mip));
    mip->w[0] = pixarr->w;
    // W + (1/4)W + (1/8)W + ... + (1/W)W = (4W-1)/3
    size_t mipsize = (4*(pixarr->w)*(pixarr->w) - 1)/3;
    mip->pixels = zgl_Calloc(mipsize, sizeof(zgl_Color));
    src = pixarr->pixels;
    src_w = (zgl_Pixit)pixarr->w;
    mip->num_levels = 0;
    
    level = 0;
    mip->level_offset[0] = 0;
    dst = mip->pixels;
    dst_w = (zgl_Pixit)mip->w[0];
    while (dst_w >= 1) {
        mip->num_levels++;
        mip->w[level] = dst_w;
        for (int dst_y = 0; dst_y < dst_w; dst_y++) {
            for (int dst_x = 0; dst_x < dst_w; dst_x++) {
                // downsample. TODO: Use function pointer to allow easy
                // customization of downsample technique (here we have box blur
                // hard-coded)
                zgl_Color r, g, b;
                r = g = b = 0;

                int box_x0 = dst_x<<level;
                int box_y0 = dst_y<<level;
                int box_w = 1<<level;
                int box_h = 1<<level;
                for (int box_dy = 0; box_dy < box_h; box_dy++) {
                    for (int box_dx = 0; box_dx < box_w; box_dx++) {
                        int src_color = src[(box_x0 + box_dx) +
                                            (box_y0 + box_dy)*src_w];
                        r += r_of(src_color);
                        g += g_of(src_color);
                        b += b_of(src_color);
                    }
                }
                r /= box_w*box_h;
                g /= box_w*box_h;
                b /= box_w*box_h;
                                
                dst[dst_x + dst_y*dst_w] = (r << 16) | (g << 8) | (b << 0);
            }        
        }

        level++;
        mip->level_offset[level] = mip->level_offset[level-1] + dst_w*dst_w;
        dst += dst_w*dst_w;
        dst_w /= 2;
    }

    /*
    printf("(levels %zu)\n", mip->num_levels);
    for (size_t i = 0; i < mip->num_levels; i++) {
        printf("(level %zu) (offset %zu) (width %zu)\n", i, mip->level_offset[i], mip->w[i]);
    }
    */
    
    return mip;
}

zgl_Result zgl_DestroyMipMap
(zgl_MipMap *const mipmap) {
    zgl_Free(mipmap->pixels);
    zgl_Free(mipmap);

    return ZR_SUCCESS;
}



void zgl_BlitEntireMipMap
(zgl_PixelArray *dst,
 zgl_mPixelRect dst_rect,
 zgl_MipMap const *src,
 zgl_mPixelRect *bounds) {   
    zgl_Color *sam = src->pixels;
    size_t sam_lw = src->w[0];
    int div;
    int mod;    

    //fix32_t density = (fix32_t)(src->w[0]<<16 / dst_rect.w);
    
    if (dst_rect.w >= 16<<16) {
        sam = sam + src->level_offset[0];
        sam_lw = src->w[0];
        mod = 31;
        div = 1;
    }
    else if (dst_rect.w >= 8<<16) {
        sam = sam + src->level_offset[1];
        sam_lw = src->w[1];
        mod = 15;
        div = 2;
    }
    else if (dst_rect.w >= 4<<16) {
        sam = sam + src->level_offset[2];
        sam_lw = src->w[2];
        mod = 7;
        div = 4;
    }
    else if (dst_rect.w >= 2<<16) {
        sam = sam + src->level_offset[3];
        sam_lw = src->w[3];
        mod = 3;
        div = 8;
    }
    else if (dst_rect.w >= 1<<16) {
        sam = sam + src->level_offset[4];
        sam_lw = src->w[4];
        mod = 1;
        div = 16;
    }
    else {
        sam = sam + src->level_offset[5];
        sam_lw = src->w[5];
        mod = 0;
        div = 32;
    }

    int32_t x0 = (MAX(dst_rect.x, bounds->x)>>16);
    int32_t y0 = (MAX(dst_rect.y, bounds->y)>>16);
    int32_t x1 = (MIN(dst_rect.x + dst_rect.w,
                      bounds->x + bounds->w)>>16) - 1;
    int32_t y1 = (MIN(dst_rect.y + dst_rect.h,
                      bounds->y + bounds->h)>>16) - 1;
        
    for (int32_t y = y0; y <= y1; y++) {
        for (int32_t x = x0; x <= x1; x++) {
            int32_t iconX, iconY;
            iconX = (((x-((dst_rect.x)>>16))*32)/(div*(dst_rect.w>>16)));
            iconY = (((y-((dst_rect.y)>>16))*32)/(div*(dst_rect.h>>16)));
            dst->pixels[x + y*dst->w] =
                sam[(iconX & mod) + (iconY & mod)*sam_lw];
        }
    }
}
