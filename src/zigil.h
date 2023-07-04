#ifndef ZIGIL_H
#define ZIGIL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "diblib_local/fixpt.h"

#if __linux__
#  define ZGL_PLATFORM_STR "linux"
#  define ZGL_LINUX 1
#elif _WIN32
#  define ZGL_PLATFORM_STR "windows"
#  define ZGL_WINDOWS 1
#else
#error Only Linux and Windows are valid compilation targets.
#endif

//#define ZGL_VERBOSITY

#define ZGL_VFLAG_NONE      000
/* request the fastest graphics context */
#define ZGL_VFLAG_VIDFAST   002

typedef int16_t zgl_Result;
#define ZR_SUCCESS        0
#define ZR_ERROR         -1
#define ZR_CLOSE_REQUEST  1
#define ZR_NOEVENT        2

extern zgl_Result zgl_InitVideo(char *title, uint32_t width, uint32_t height,
                                uint32_t scale, uint32_t flags);
extern void zgl_TermVideo(void);
extern void zgl_VideoUpdate(void);

extern zgl_Result zgl_SetWindowTitle(const char *title);

#define MIN_SCR_WIDTH 400U
#define MAX_SCR_WIDTH 1920U
#define MIN_SCR_HEIGHT 225U
#define MAX_SCR_HEIGHT 1080U
#define SCALE1 0
#define SCALE2 1
#define SCALE4 2
#define SCALE8 3

#define INIT_SCALE SCALE1
#define INIT_SCR_WIDTH 640
#define INIT_SCR_HEIGHT 480
#define INIT_R_MASK 0xFF0000
#define INIT_G_MASK 0x00FF00
#define INIT_B_MASK 0x0000FF
#define INIT_BYTES_PER_PIXEL 4U
#define INIT_BITS_PER_PIXEL (INIT_BYTES_PER_PIXEL * 8)
#define INIT_BYTES_PER_COLOR 3U
#define INIT_BITS_PER_COLOR (INIT_BYTES_PER_COLOR * 8)

#define zgl_Color8 uint8_t
#define zgl_Color16 uint16_t
#define zgl_Color32 uint32_t
#define zgl_Color64 uint64_t

//#define ZGL_COLOR_8
//#define ZGL_COLOR_16
#define ZGL_COLOR_32
//#define ZGL_COLOR_64

#define zgl_Color zgl_Color32

typedef struct zgl_PixelArray {
    zgl_Color *pixels;
    uint32_t w, h;
} zgl_PixelArray;


/* zgl_FrameBufferInfo collects all the data regarding the framebuffer. The framebuffer is what gets sent to the graphics library to be displayed on the screen. Think of it as an expanded pixelarray. */
typedef struct zgl_FrameBufferInfo {
    zgl_PixelArray fb; /* The framebuffer, in zgl_PixelArray form. */
    uint32_t fb_byte_size;
    
    uint32_t width;
    uint32_t height;
    uint32_t upscale_width;
    uint32_t upscale_height;
    uint32_t upscale_shift;
    
    uint32_t pitch;
    // bits per row
    // bytes per row
    uint32_t bits_per_pixel; // corresponds to X bitmap_unit
    uint32_t bytes_per_pixel;
    uint32_t bits_per_color; // corresponds to X depth
    uint32_t bytes_per_color;
    uint32_t r_mask;
    uint32_t g_mask;
    uint32_t b_mask;
} zgl_FrameBufferInfo;

extern zgl_FrameBufferInfo fb_info;

#define RMASK 0xFF000000
#define GMASK 0x00FF0000
#define BMASK 0x0000FF00
#define AMASK 0x000000FF

#define RGB(R,G,B) (0 | ((R)<<16) | ((G)<<8) | (B))
#define GRAY(I)    (0 | ((I)<<16) | ((I)<<8) | (I))

#define a_of(color) (((color)>>24)&0xFF)
#define r_of(color) (((color)>>16)&0xFF)
#define g_of(color) (((color)>>8 )&0xFF)
#define b_of(color) (((color)>>0 )&0xFF)

/* SCREEN GEOMETRY */

typedef struct Int32Point_2D {
    int32_t x;
    int32_t y;
} Int32Point_2D;
typedef struct Int32Seg_2D {
    Int32Point_2D pt0;
    Int32Point_2D pt1;
} Int32Seg_2D;
typedef struct Int32Circle_2D {
    Int32Point_2D center;
    int32_t radius;
} Int32Circle_2D;
#define MAX_POINTS_PER_POLYGON 100
typedef struct Int32Polygon_2D {
    uint32_t num_points;
    Int32Point_2D pt[MAX_POINTS_PER_POLYGON];
} Int32Polygon_2D;
typedef struct Int32Rect_2D {
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
} Int32Rect_2D;
typedef struct Int32Line_2D {
    Int32Point_2D origin; // just some chosen point on the line
    Int32Point_2D normal; // the "A" and "B" in Ax + By = C
    int32_t constant;  // the "C" in Ax + By = C
} Int32Line_2D;

typedef int32_t zgl_Pixit;
typedef Int32Point_2D zgl_Pixel;
typedef Int32Seg_2D zgl_PixelSeg;
typedef Int32Circle_2D zgl_PixelCircle;
typedef Int32Rect_2D zgl_PixelRect;
typedef Int32Polygon_2D zgl_PixelPolygon;
typedef Int32Line_2D zgl_PixelLine;

typedef struct zgl_Texel {
    int32_t u;
    int32_t v;
} zgl_Texel;

typedef struct zgl_Lumel {
    int32_t u;
    int32_t v;
} zgl_Lumel;



/* subpixel precision */
typedef Fix32 zgl_mPixit;
typedef Fix32Vec_2D zgl_mPixel;
typedef Fix32Seg_2D zgl_mPixelSeg;
typedef Fix32Circle_2D zgl_mPixelCircle;
typedef Fix32Rect_2D zgl_mPixelRect;
typedef Fix32Polygon_2D zgl_mPixelPolygon;
typedef Fix32Line_2D zgl_mPixelLine;

typedef struct zgl_mTexel {
    Fix32 u;
    Fix32 v;
} zgl_mTexel;

typedef struct zgl_mLumel {
    Fix32 u;
    Fix32 v;
} zgl_mLumel;

#define pixit_from_mpixit(mp) ((zgl_Pixit)intify(mp))
#define pixel_from_mpixel(MP) ((zgl_Pixel){(zgl_Pixit)intify((MP).x), (zgl_Pixit)intify((MP).y)})
#define pixelseg_from_mpixelseg(MPS)            \
    ((zgl_PixelSeg)                             \
     {pixel_from_mpixel((MPS).pt0),             \
      pixel_from_mpixel((MPS).pt1)})
#define pixelcircle_from_mpixelcircle(MPC)      \
    ((zgl_PixelCircle)                          \
     {pixel_from_mpixel((MPC).center),          \
      pixit_from_mpixit((MPC).radius)})
#define pixelrect_from_mpixelrect(MPR)          \
    ((zgl_PixelRect)                            \
     {intify((MPR).x),                          \
      intify((MPR).y),                          \
      intify((MPR).w),                          \
      intify((MPR).h)})
#define mpixit_from_pixit(p) ((zgl_mPixit)fixify(p))
#define mpixel_from_pixel(P) ((zgl_mPixel){fixify((P).x), fixify((P).y)})
#define mpixelseg_from_pixelseg(PS)             \
    ((zgl_mPixelSeg)                            \
     {mpixel_from_pixel((PS).pt0),              \
      mpixel_from_pixel((PS).pt1)})
#define mpixelcircle_from_pixelcircle(PC)       \
    ((zgl_mPixelCircle)                         \
     {mpixel_from_pixel((PC).center),           \
      mpixit_from_pixit((PC).radius)})
#define mpixelrect_from_pixelrect(PR)           \
    ((zgl_mPixelRect)                           \
     {fixify((PR).x),                           \
      fixify((PR).y),                           \
      fixify((PR).w),                           \
      fixify((PR).h)})

/* Graphics functions; maybe NOT dependent on external library */
#define ZGL_NOERROR 0
#define ZGL_ERROR_NULL_PIXELARRAY -1
#define ZGL_ERROR_GET_UNSET_COLORKEY -2

extern void zgl_Blit
(zgl_PixelArray *dst,
 zgl_Pixit dst_x, zgl_Pixit dst_y,
 zgl_PixelArray const *src,
 zgl_Pixit src_x, zgl_Pixit src_y,
 zgl_Pixit w, zgl_Pixit h);

#define zgl_BlitEntire(dst, dst_x, dst_y, src) zgl_Blit((dst), (dst_x), (dst_y), (src), 0, 0, (src)->w, (src)->h)

extern void zgl_BlitTransparent
(zgl_PixelArray *dst,
 zgl_Pixit dst_x, zgl_Pixit dst_y,
 zgl_PixelArray const *src,
 zgl_Pixit src_x, zgl_Pixit src_y,
 zgl_Pixit w, zgl_Pixit h,
 zgl_Color trans);

extern void zgl_BlitBMPFont
(zgl_PixelArray *dst,
 zgl_Pixit dst_x, zgl_Pixit dst_y,
 zgl_PixelArray const *src,
 zgl_Pixit src_x, zgl_Pixit src_y,
 zgl_Pixit w, zgl_Pixit h,
 zgl_Color color);

extern void zgl_BlitBMPFont2
(zgl_PixelArray *dst,
 zgl_Pixit dst_x, zgl_Pixit dst_y,
 zgl_PixelArray const *src,
 zgl_Pixit src_x, zgl_Pixit src_y,
 zgl_Pixit w, zgl_Pixit h,
 zgl_Color color);

extern void zgl_BlitToFramebuffer
(zgl_Pixit fb_x, zgl_Pixit fb_y,
 zgl_PixelArray const *const src,
 zgl_Pixit src_x, zgl_Pixit src_y,
 zgl_Pixit w, zgl_Pixit h);

extern void zgl_ZeroEntire
(zgl_PixelArray *const pixarr);

extern void zgl_GrayEntire
(zgl_PixelArray *const pixarr, uint8_t gray);

extern void zgl_ZeroRect
(zgl_PixelArray *const pixarr,
 zgl_Pixit x, zgl_Pixit y,
 zgl_Pixit w, zgl_Pixit h);

extern void zgl_GrayRect
(zgl_PixelArray *const pixarr,
 zgl_Pixit x, zgl_Pixit y,
 zgl_Pixit w, zgl_Pixit h, uint8_t gray);

extern void zgl_TempFillRect
(zgl_PixelArray *const pixarr,
 zgl_Pixit x, zgl_Pixit y,
 zgl_Pixit w, zgl_Pixit h,
 zgl_Color color);

extern void zgl_FillRect
(zgl_PixelArray *const pixarr,
 zgl_Pixit x, zgl_Pixit y,
 zgl_Pixit w, zgl_Pixit h,
 zgl_Color color);



extern zgl_PixelArray *zgl_ReadBMP
(char const *const filename);

extern void zgl_WriteBMP
(char const *const filename, zgl_PixelArray *pixarr);

extern zgl_PixelArray *zgl_ReadQOI
(char const *const filename);

extern void zgl_WriteQOI(char const *const filename, zgl_PixelArray *pixarr);

extern zgl_PixelArray *zgl_CreatePixelArray
(uint32_t const width, uint32_t const height);

extern zgl_Result zgl_DestroyPixelArray
(zgl_PixelArray *const pixarr);

extern zgl_Result zgl_SetColorKey
(zgl_PixelArray *const pixarr,
 zgl_Color const colorkey);

extern zgl_Result zgl_GetColorKey
(zgl_PixelArray const *const pixarr,
 zgl_Color *const colorkey);

extern zgl_Result zgl_UnsetColorKey
(zgl_PixelArray *const pixarr);

#define zgl_LinearBlend(color1, color2, factor) \
    (((((((1<<8) - 1 - factor)*((color1 & 0xFF0000)>>16))>>8) + (((factor)*((color2 & 0xFF0000)>>16))>>8)) << 16) \
     | ((((((1<<8) - 1 - factor)*((color1 & 0x00FF00)>>8))>>8) + (((factor)*((color2 & 0x00FF00)>>8))>>8)) << 8) \
     | (((((1<<8) - 1 - factor)*(color1 & 0x0000FF))>>8) + (((factor)*(color2 & 0x0000FF))>>8)) )



// 2D Line and Circle Drawing functions
// defined in 2d/draw_circles_and_segs.h

extern int zglDraw_mPixelDot
(zgl_PixelArray *screen,
 const zgl_mPixelRect *bounds,
 const zgl_mPixel *pt,
 const zgl_Color color);
extern int zglDraw_mPixelCircle
(zgl_PixelArray *screen,
 const zgl_mPixelRect *bounds,
 const zgl_mPixelCircle *circle,
 const zgl_Color color);
extern int zglDraw_mPixelSeg
(zgl_PixelArray *screen,
 const zgl_mPixelRect *bounds,
 const zgl_mPixel pixel0,
 const zgl_mPixel pixel1,
 const zgl_Color color);
#define zglDraw_mPixelSeg2(SCREEN_P, BOUNDS_P, SEG_P, COLOR)             \
    zglDraw_mPixelSeg((SCREEN_P), (BOUNDS_P), (SEG_P)->pt0, (SEG_P)->pt1, (COLOR))
extern int zglDraw_mPixelSeg_Vertical
(zgl_PixelArray *screen,
 const zgl_mPixit x,
 const zgl_mPixit y0,
 const zgl_mPixit y1,
 const zgl_Color color);
extern int zglDraw_mPixelSeg_Horizontal
(zgl_PixelArray *screen,
 const zgl_mPixit x0,
 const zgl_mPixit x1,
 const zgl_mPixit y,
 const zgl_Color color);
extern int zglDraw_mPixelSeg_Dotted
(zgl_PixelArray *screen,
 const zgl_mPixelRect *bounds,
 const zgl_mPixel pixel0,
 const zgl_mPixel pixel1,
 const zgl_Color color);
#define zglDraw_mPixelSeg_Dotted2(SCREEN_P, BOUNDS_P, SEG_P, COLOR, FLAGS)    \
    zglDraw_mPixelSeg_Dotted((SCREEN_P), (BOUNDS_P), (SEG_P)->pt0, (SEG_P)->pt1, (COLOR))
extern int zglDraw_mPixelSeg_Vertical_Dotted
(zgl_PixelArray *screen,
 const zgl_mPixit x,
 const zgl_mPixit y0,
 const zgl_mPixit y1,
 const zgl_Color color);
extern int zglDraw_mPixelSeg_Horizontal_Dotted
(zgl_PixelArray *screen,
 const zgl_mPixit x0,
 const zgl_mPixit x1,
 const zgl_mPixit y,
 const zgl_Color color);


extern int zglDraw_PixelDot
(zgl_PixelArray *screen,
 const zgl_PixelRect *bounds,
 const zgl_Pixel *pt,
 const zgl_Color color);
extern int zglDraw_PixelCircle
(zgl_PixelArray *screen,
 const zgl_PixelRect *bounds,
 const zgl_PixelCircle *circle,
 const zgl_Color color);
extern int zglDraw_PixelSeg
(zgl_PixelArray *screen,
 const zgl_PixelRect *bounds,
 const zgl_PixelSeg *seg,
 const zgl_Color color);
extern int zglDraw_PixelSeg_Vertical
(zgl_PixelArray *screen,
 const zgl_Pixit x,
 const zgl_Pixit y0,
 const zgl_Pixit y1,
 const zgl_Color color);
extern int zglDraw_PixelSeg_Horizontal
(zgl_PixelArray *screen,
 const zgl_Pixit x0,
 const zgl_Pixit x1,
 const zgl_Pixit y,
 const zgl_Color color);
extern int zglDraw_PixelSeg_Dotted
(zgl_PixelArray *screen,
 const zgl_PixelRect *bounds,
 const zgl_PixelSeg *seg,
 const zgl_Color color);
extern int zglDraw_PixelSeg_Vertical_Dotted
(zgl_PixelArray *screen,
 const zgl_Pixit x,
 const zgl_Pixit y0,
 const zgl_Pixit y1,
 const zgl_Color color);

extern int zglDraw_PixelSeg_Horizontal_Dotted
(zgl_PixelArray *screen,
 const zgl_Pixit x0,
 const zgl_Pixit x1,
 const zgl_Pixit y,
 const zgl_Color color);

extern int zglDraw_PixelRect_Outline
(zgl_PixelArray *screen,
 zgl_PixelRect *rect,
 zgl_Color color);


// 2D Misc
extern bool zgl_in_mPixelRect(zgl_mPixel pt, zgl_mPixelRect rect);
extern bool zgl_in_PixelRect(zgl_Pixel pt, zgl_PixelRect rect);

// 2D Clipping Functions

extern bool zgl_ClipSegToRect
(zgl_mPixelSeg seg,
 zgl_mPixelRect rect,
 zgl_mPixelSeg *clipped_seg);

extern bool zgl_ClipSegToPoly
(zgl_mPixelSeg seg,
 zgl_mPixelPolygon poly,
 zgl_mPixelSeg *clipped_seg);

int64_t zgl_GetMemoryUsage(void);

#endif /* ZIGIL_H */
