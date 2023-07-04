#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/zigil.h"
#include "src/zigil_mem.h"

#define STATIC_FRAMEBUFFER
#define STATIC_FRAMEBUFFER_MAX_SIZE                     \
    ((size_t)1920 * (size_t)1080 * sizeof(zgl_Color))

zgl_FrameBufferInfo fb_info;

#ifdef STATIC_FRAMEBUFFER
static zgl_Color static_fb_pixels[STATIC_FRAMEBUFFER_MAX_SIZE];
#endif

typedef struct zgl_PRIV_PixelArray {
    zgl_Color *pixels;
    uint32_t w, h;
    zgl_Color colorkey;
    bool has_colorkey;
} zgl_PRIV_PixelArray;

void zgl_Blit
(zgl_PixelArray *dst,
 zgl_Pixit dst_x, zgl_Pixit dst_y,
 zgl_PixelArray const *src,
 zgl_Pixit src_x, zgl_Pixit src_y,
 zgl_Pixit w, zgl_Pixit h) {
    if (src == NULL || dst == NULL || ! w || ! h)
        return;

    uint32_t
        src_full_w = src->w,
        dst_full_w = dst->w;

    if (src_full_w == (uint32_t)w && dst_full_w == (uint32_t)w) {
        // just a single memcpy
        memcpy(dst->pixels + ((dst_full_w)*(dst_y)) + dst_x,
               src->pixels + ((src_full_w)*(src_y)) + src_x,
               w*sizeof(zgl_Color));
    }
    
    /* row-by-row memcpy */
    size_t num = w*sizeof(zgl_Color);
    zgl_Color *p_src_rowstart =
        src->pixels + ((src_full_w)*(src_y + 0)) + src_x;
    zgl_Color *p_dst_rowstart =
        dst->pixels + ((dst_full_w)*(dst_y + 0)) + dst_x;

    if (w == 1) { // TODO: Really worth it to avoid memcpy in this case?
        // a vertical line or a point
        for (int i = 0; i <= h-1; i++) {
            *p_dst_rowstart = *p_src_rowstart;
            p_src_rowstart += src_full_w;
            p_dst_rowstart += dst_full_w;
        }
    }
    
    for (int i = 0; i <= h-1; i++) {
        memcpy(p_dst_rowstart, p_src_rowstart, num);
        p_src_rowstart += src_full_w;
        p_dst_rowstart += dst_full_w;
    }
}

void zgl_BlitBMPFont
(zgl_PixelArray *dst,
 zgl_Pixit dst_x, zgl_Pixit dst_y,
 zgl_PixelArray const *src,
 zgl_Pixit src_x, zgl_Pixit src_y,
 zgl_Pixit w, zgl_Pixit h,
 zgl_Color color) {
    // assumed black is transparent.
    if (src == NULL || dst == NULL || ! w || ! h)
        return;
    
    int src_w = src->w;
    int dst_w = dst->w;
    zgl_Color *p_src = src->pixels + src_w*src_y + src_x;
    zgl_Color *p_dst = dst->pixels + dst_w*dst_y + dst_x;
    
    int src_inc_per_y = src_w - w;
    int dst_inc_per_y = dst_w - w;
            
    for (int y = 0; y < h; y++, p_src += src_inc_per_y, p_dst += dst_inc_per_y) {
        for (int x = 0; x < w; x++, p_src++, p_dst++) {
            if (*p_src != 0) {
                *p_dst = color;
            }
        }
    }
}

void zgl_BlitBMPFont2
(zgl_PixelArray *dst,
 zgl_Pixit dst_x, zgl_Pixit dst_y,
 zgl_PixelArray const *src,
 zgl_Pixit src_x, zgl_Pixit src_y,
 zgl_Pixit w, zgl_Pixit h,
 zgl_Color color) {
    // assumes that w is a multiple of 8
    
    // assumed black is transparent.
    if (src == NULL || dst == NULL || ! w || ! h)
        return;
    
    int src_w = src->w;
    int dst_w = dst->w;
    zgl_Color *p_src = src->pixels + src_w*src_y + src_x;
    zgl_Color *p_dst = dst->pixels + dst_w*dst_y + dst_x;
    
    int src_inc_per_y = src_w - w;
    int dst_inc_per_y = dst_w - w;
            
    for (int y = 0; y < h; y++, p_src += src_inc_per_y, p_dst += dst_inc_per_y) {
        for (int x = 0; x < w; x += 8) {
            if (*(p_src++) != 0) {
                *(p_dst) = color;
            }
            p_dst++;
                
            if (*(p_src++) != 0) {
                *(p_dst) = color;
            }
            p_dst++;
            
            if (*(p_src++) != 0) {
                *(p_dst) = color;
            }
            p_dst++;
            
            if (*(p_src++) != 0) {
                *(p_dst) = color;
            }
            p_dst++;
                        if (*(p_src++) != 0) {
                *(p_dst) = color;
            }
            p_dst++;
                
            if (*(p_src++) != 0) {
                *(p_dst) = color;
            }
            p_dst++;
            
            if (*(p_src++) != 0) {
                *(p_dst) = color;
            }
            p_dst++;
            
            if (*(p_src++) != 0) {
                *(p_dst) = color;
            }
            p_dst++;
        }
    }
}

void zgl_BlitTransparent
(zgl_PixelArray *dst,
 zgl_Pixit dst_x, zgl_Pixit dst_y,
 zgl_PixelArray const *src,
 zgl_Pixit src_x, zgl_Pixit src_y,
 zgl_Pixit w, zgl_Pixit h,
 zgl_Color transparent) {
    if (src == NULL || dst == NULL || ! w || ! h)
        return;
    
    int src_w = src->w;
    int dst_w = dst->w;
    zgl_Color *p_src = src->pixels + src_w*src_y + src_x;
    zgl_Color *p_dst = dst->pixels + dst_w*dst_y + dst_x;
    
    int src_inc_per_y = src_w - w;
    int dst_inc_per_y = dst_w - w;
            
    for (int y = 0; y < h; y++, p_src += src_inc_per_y, p_dst += dst_inc_per_y) {
        for (int x = 0; x < w; x++, p_src++, p_dst++) {
            zgl_Color color = *p_src;
            if (color != transparent) {
                *p_dst = color;
            }
        }
    }
    
    return;
}

void zgl_ZeroEntire(zgl_PixelArray *const dst) {
    if (dst == NULL)
        return;

    /* one big memory copy */
    size_t num = dst->w * dst->h * sizeof(zgl_Color);
    memset(dst->pixels, 0, num);
}

void zgl_GrayEntire(zgl_PixelArray *const dst, uint8_t gray) {
    if (dst == NULL)
        return;

    /* one big memory copy */
    size_t num = dst->w * dst->h * sizeof(zgl_Color);
    memset(dst->pixels, gray, num);
}

void zgl_ZeroRect
(zgl_PixelArray *const dst,
 zgl_Pixit const x, zgl_Pixit const y,
 zgl_Pixit const w, zgl_Pixit const h) {
    if (dst == NULL)
        return;

    /* row-by-row literal memory copy */
    size_t num = w*sizeof(zgl_Color);
    uint32_t inc = dst->w;
    zgl_Color *rowstart_p = dst->pixels + ((inc)*(y + 0)) + x;
    
    for (int i = 0; i <= h-1; i++) {
        memset(rowstart_p, 0, num);
        rowstart_p += inc;
    }
}

void zgl_GrayRect
(zgl_PixelArray *const dst,
 zgl_Pixit const x, zgl_Pixit const y,
 zgl_Pixit const w, zgl_Pixit const h, uint8_t gray) {
    if (dst == NULL)
        return;

    /* row-by-row literal memory copy */
    size_t num = w*sizeof(zgl_Color);
    uint32_t inc = dst->w;
    zgl_Color *rowstart_p = dst->pixels + ((inc)*(y + 0)) + x;
    
    for (int i = 0; i <= h-1; i++) {
        memset(rowstart_p, gray, num);
        rowstart_p += inc;
    }
}


void zgl_TempFillRect
(zgl_PixelArray *const dst,
 zgl_Pixit const x, zgl_Pixit const y,
 zgl_Pixit const w, zgl_Pixit const h,
 zgl_Color const color) {       
    if (dst == NULL)
        return;

    uint32_t inc = dst->w;
    zgl_Color *p_rowstart = dst->pixels + (inc*(y + 0)) + x;
    zgl_Color *p = p_rowstart; 
    
    for (int i = 0; i <= h-1; i++) {
        for (int j = 0; j <= w-1; j++) {
            *p = color;
            p++;
        }
        p_rowstart += inc;
        p = p_rowstart;
    }
    
}

/* EXTREMELY SLOW
static void memset32(void * dest, uint64_t value, uintptr_t size)
{
    uintptr_t i;
    for (i = 0; i < (size & (~3)); i += 4) {
        memcpy(((char*)dest) + i, &value, 4);
    }  
    for ( ; i < size; i++) {
        ((char*)dest)[i] = ((char*)&value)[i&3];
    }  
}
*/
/*
static void memset64(void * dest, uint64_t value, uintptr_t size)
{
    uintptr_t i;
    for (i = 0; i < (size & (~7)); i += 8) {
        memcpy( ((char*)dest) + i, &value, 8 );
    }  
    for ( ; i < size; i++) {
        ((char*)dest)[i] = ((char*)&value)[i&7];
    }  
}
*/


void zgl_FillRect
(zgl_PixelArray *const dst,
 zgl_Pixit const x, zgl_Pixit const y,
 zgl_Pixit const w, zgl_Pixit const h,
 zgl_Color const color) {
    if ( ! dst) return;

    uint32_t inc = dst->w;
    zgl_Color *p_rowstart = dst->pixels + (inc*(y + 0)) + x;
    zgl_Color *p = p_rowstart; 
    
    for (int i = 0; i <= h-1; i++) {
        for (int j = 0; j <= w-1; j++) {
            *p = color;
            p++;
        }
        p_rowstart += inc;
        p = p_rowstart;
    }
    
}

zgl_PixelArray *zgl_CreatePixelArray
(uint32_t const w, uint32_t const h) {
    zgl_PRIV_PixelArray *_pixarr = zgl_Calloc(1, sizeof(zgl_PRIV_PixelArray));
    if ( ! _pixarr) {
        goto Error;
    }
    _pixarr->pixels = zgl_Calloc(w*h, sizeof(zgl_Color));
    if ( ! _pixarr->pixels) {
        zgl_Free(_pixarr);
        goto Error;
    }

    _pixarr->w = w;
    _pixarr->h = h;

    _pixarr->colorkey = 0x00000000;
    _pixarr->has_colorkey = false;

    return (zgl_PixelArray *)_pixarr;

 Error:
    return NULL;
}

zgl_Result zgl_DestroyPixelArray
(zgl_PixelArray *const pixarr) {
    if (pixarr != NULL) {
        if (pixarr->pixels != NULL) {
            zgl_Free(pixarr->pixels);
        }
        zgl_Free(pixarr);
    }

    return ZGL_NOERROR;
}

zgl_Result zgl_SetColorKey
(zgl_PixelArray *const pixarr,
 zgl_Color const colorkey) {
    if (pixarr == NULL) {
        return ZGL_ERROR_NULL_PIXELARRAY;
    }

    ((zgl_PRIV_PixelArray *)pixarr)->has_colorkey = true;
    ((zgl_PRIV_PixelArray *)pixarr)->colorkey = colorkey;

    return ZGL_NOERROR;
}

zgl_Result zgl_GetColorKey
(zgl_PixelArray const *const pixarr,
 zgl_Color *const colorkey) {
    if (pixarr == NULL) {
        return ZGL_ERROR_NULL_PIXELARRAY;
    }
    
    if (((zgl_PRIV_PixelArray *)pixarr)->has_colorkey == false) {
        return ZGL_ERROR_GET_UNSET_COLORKEY;
    }

    *colorkey = ((zgl_PRIV_PixelArray *)pixarr)->colorkey;
    
    return ZGL_NOERROR;
    
}

zgl_Result zgl_UnsetColorKey
(zgl_PixelArray *const pixarr) {
    if (pixarr == NULL) {
        return ZGL_ERROR_NULL_PIXELARRAY;
    }

    ((zgl_PRIV_PixelArray *)pixarr)->has_colorkey = false;

    return ZGL_NOERROR;
}

#ifdef ZGL_VERBOSITY
static void print_fb_info(void) {
    printf(      "   fb_info struct\n");
    printf(      "-----------------\n");
    lputf(  "  .fb.pixels (*)", "p", (void *)fb_info.fb.pixels);
    lputf(  "   .fb_byte_size", "u", fb_info.fb_byte_size);
    lputf(  "          .width", "u", fb_info.width);
    lputf(  "         .height", "u", fb_info.height);
    lputf(  "          .width", "u", fb_info.upscale_width);
    lputf(  "         .height", "u", fb_info.upscale_height);
    lputf(  "       .scaleEXP", "u", fb_info.upscale_shift);
    lputf(  "          .pitch", "u", fb_info.pitch);
    lputf(  " .bits_per_pixel", "u", fb_info.bits_per_pixel);
    lputf(  ".bytes_per_pixel", "u", fb_info.bytes_per_pixel);
    lputf(  " .bits_per_color", "u", fb_info.bits_per_color);
    lputf(  ".bytes_per_color", "u", fb_info.bytes_per_color);
    lputdf("         .r_mask", "=.6Xu", fb_info.r_mask);
    lputdf("         .g_mask", "=.6Xu", fb_info.g_mask);
    lputdf("         .b_mask", "=.6Xu", fb_info.b_mask);
}
#endif

#define CALC_PITCH(w, bytes_per_pixel) ((((w) * (bytes_per_pixel)) + 3U) & ~3U)
#define BYTE_ALIGN(w, bytes_per_pixel) (CALC_PITCH((w), (bytes_per_pixel)) / (bytes_per_pixel))
// PITCH is the smallest multiple of 4 which is greater than or equal to w*bytes_per_pixel, because the PITCH must be a multiple of 4 bytes
//  3 = 00000011
// ~3 = 11111100
// Say we have a number P. Then P + 3 > P
// Then         (P + 3) & ~3 is smaller than (P + 3) by either 0, 1, 2, or 3.
// Hence  P <= ((P + 3) & ~3) <= P + 3
// And clearly ((P + 3) & ~3) is a multiple of 4 (two lowest bits 0)
extern zgl_Result zgl_LibInit(char *const title, uint32_t flags);
extern zgl_Result zgl_LibValidate(void);
extern void zgl_LibTerm(void);

zgl_Result zgl_InitVideo
(char * title,
 uint32_t const width,
 uint32_t const height,
 uint32_t const scale,
 uint32_t const flags) {
    /* Validate parameters. */
    if (title == NULL) {
        title = "Untitled.";
    }
    if (width < MIN_SCR_WIDTH || width > MAX_SCR_WIDTH) {
        printf("ZIGIL: Screen width must be between %d and %d; value of %d was given.\n", MIN_SCR_WIDTH, MAX_SCR_WIDTH, width);
        goto Error;
    }
    if (height < MIN_SCR_HEIGHT || height > MAX_SCR_HEIGHT) {
        printf("ZIGIL: Screen height must be between %d and %d; value of %d was given.\n", MIN_SCR_HEIGHT, MAX_SCR_HEIGHT, height);
        goto Error;
    }
    if (scale != 0 && scale != 1 && scale != 2 && scale != 4) {
        printf("ZIGIL: Screen scale must be 0 (1x), 1 (2x), 2 (4x), or 3 (8x); value of %d was given.\n", scale);
        goto Error;
    }

    /* Summary:
       
       1) Configure framebuffer (fb_info)
       2) Configure scaling parameters (scaled_info)
       3) Configure X Display       (xdisplay_info)
       4) Configure X Screen        (xdisplay_info.screen)
       5) Configure X Window        (xdisplay_info.win)
       6) Configure X Window Visual (xdisplay_info.win_vis)
       7) Configure X Window GC     (xdisplay_info.win_gc)

    */

#ifdef ZGL_VERBOSITY
    /* Prepare the framebuffer struct */
    fputs("-------------------------------\n", stdout);
    fputs("- ZIGIL: Creating Framebuffer -\n", stdout);
    fputs("-------------------------------\n", stdout);
    putchar('\n');
#endif
    /* NOTE: All BYTE_ALIGN and CALC_PITCH are pointless when bytes_per_pixel is 4, which it should always be for me, but I keep it just in case I want to get wet and wild some day */

    /* For now, the following fields are hardcoded. */
    fb_info.bits_per_pixel  = INIT_BITS_PER_PIXEL;
    fb_info.bytes_per_pixel = INIT_BYTES_PER_PIXEL;
    fb_info.bits_per_color  = INIT_BITS_PER_COLOR;
    fb_info.bytes_per_color = INIT_BYTES_PER_COLOR;
    fb_info.r_mask          = INIT_R_MASK;
    fb_info.g_mask          = INIT_G_MASK;
    fb_info.b_mask          = INIT_B_MASK;
    
    uint32_t bytes_per_pixel = fb_info.bytes_per_pixel;
    fb_info.width = BYTE_ALIGN(width, bytes_per_pixel);
    fb_info.fb.w = fb_info.width;
    fb_info.height = BYTE_ALIGN(height, bytes_per_pixel);
    fb_info.fb.h = fb_info.height;
    uint32_t fb_w = fb_info.width;
    uint32_t fb_h = fb_info.height;
    fb_info.pitch = CALC_PITCH(fb_w, bytes_per_pixel);

    if (fb_info.fb.pixels != NULL) {
        printf("ZIGIL: A framebuffer already exists where a new one was meant to be created. Freeing the old.\n");
#ifdef STATIC_FRAMEBUFFER
        if (fb_info.fb.pixels == static_fb_pixels)
            memset(fb_info.fb.pixels, 0, STATIC_FRAMEBUFFER_MAX_SIZE);
        else {
#endif
            zgl_Free(fb_info.fb.pixels);
#ifdef STATIC_FRAMEBUFFER
        }
#endif
        fb_info.fb.pixels = NULL;
    }
    
    fb_info.fb_byte_size = fb_w * fb_h * bytes_per_pixel;
#ifdef STATIC_FRAMEBUFFER
    if (fb_info.fb_byte_size <= STATIC_FRAMEBUFFER_MAX_SIZE) {
        fb_info.fb.pixels = static_fb_pixels;//zgl_Malloc(fb_info.fb_byte_size);
    }
    else {
#endif
        fb_info.fb.pixels = zgl_Malloc(fb_info.fb_byte_size);
#ifdef STATIC_FRAMEBUFFER
    }
#endif

    
    memset(fb_info.fb.pixels, 0x000000, fb_info.fb_byte_size);
    if (fb_info.fb.pixels == NULL) {
        printf("ZIGIL: Could not create new framebuffer (no memory?).\n");
        goto Error;
    }
    fb_info.upscale_shift = scale;
    fb_info.upscale_width = BYTE_ALIGN(fb_w << fb_info.upscale_shift, bytes_per_pixel);
    fb_info.upscale_height = BYTE_ALIGN(fb_h << fb_info.upscale_shift, bytes_per_pixel);
#ifdef ZGL_VERBOSITY
    printf("ZIGIL: Created framebuffer %i x %i. Scaled video will be %i x %i.\n", fb_w, fb_h, fb_info.upscale_width, fb_info.upscale_height);
    putchar('\n');
    print_fb_info();
    putchar('\n');
#endif
    
    if (0 != zgl_LibInit(title, flags)) {
        printf("ZIGIL: Could not initialize GUI backend.\n");
        goto Error;
    }

#ifdef ZGL_VERBOSITY
    fputs("----------------------------------\n", stdout);
    fputs("ZIGIL: Validating Framebuffer Data\n", stdout);
    fputs("----------------------------------\n", stdout);
    putchar('\n');
#endif
    if (0 != zgl_LibValidate()) {
        printf("ZIGIL: Somehow the information given to the external library doesn't match what it give back.\n");
        goto ErrorPostLibInit;
    }
#ifdef ZGL_VERBOSITY
    fputs("ZIGIL: Validated framebuffer data.\n", stdout);
    putchar('\n');
#endif








    extern void zgl_InitKeyCodes(void);
    zgl_InitKeyCodes();
    
    return ZR_SUCCESS;

 ErrorPostLibInit:
    zgl_LibTerm();
 Error:
    return ZR_ERROR;
}

void zgl_TermVideo(void) {
    zgl_LibTerm();
    
#ifdef STATIC_FRAMEBUFFER
    // do nothing
#else
    zgl_Free(fb_info.fb.pixels);
#endif
}



bool zgl_in_mPixelRect(zgl_mPixel pt, zgl_mPixelRect rect) {
    return ((pt.x >= rect.x) &&
            (pt.y >= rect.y) &&
            (pt.x <= rect.x + rect.w - 1) &&
            (pt.y <= rect.y + rect.h - 1));
}

bool zgl_in_PixelRect(zgl_Pixel pt, zgl_PixelRect rect) {
    return ((pt.x >= rect.x) &&
            (pt.y >= rect.y) &&
            (pt.x <= rect.x + rect.w - 1) &&
            (pt.y <= rect.y + rect.h - 1));
}

