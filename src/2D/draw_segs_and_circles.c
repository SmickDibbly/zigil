#include "src/zigil.h"
#include "src/diblib_local/dibassert.h"

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

// internal function declarations

static int circle_bresenham
(zgl_PixelArray *screen,
 const zgl_Pixit cx,
 const zgl_Pixit cy,
 const zgl_Pixit r,
 const zgl_Pixit xmin,
 const zgl_Pixit xmax,
 const zgl_Pixit ymin,
 const zgl_Pixit ymax,
 const zgl_Color color);

static int seg_high_bresenham
(zgl_PixelArray *screen,
 const zgl_Pixit x0,
 const zgl_Pixit y0,
 const zgl_Pixit x1,
 const zgl_Pixit y1,
 const zgl_Color color);

static int seg_low_bresenham
(zgl_PixelArray *screen,
 const zgl_Pixit x0,
 const zgl_Pixit y0,
 const zgl_Pixit x1,
 const zgl_Pixit y1,
 const zgl_Color color);

static int dotted_seg_high_bresenham
(zgl_PixelArray *screen,
 const zgl_Pixit x0,
 const zgl_Pixit y0,
 const zgl_Pixit x1,
 const zgl_Pixit y1,
 const zgl_Color color);

static int dotted_seg_low_bresenham
(zgl_PixelArray *screen,
 const zgl_Pixit x0,
 const zgl_Pixit y0,
 const zgl_Pixit x1,
 const zgl_Pixit y1,
 const zgl_Color color);

// function definitions


static bool in_rect_pixel
(zgl_Pixel pt,
 zgl_PixelRect rect) {
    return ((pt.x >= rect.x) && (pt.y >= rect.y)
            && (pt.x < rect.x+(zgl_Pixit)rect.w) && (pt.y < rect.y+(zgl_Pixit)rect.h));
}


int zglDraw_mPixelDot
(zgl_PixelArray *screen,
 const zgl_mPixelRect *bounds,
 const zgl_mPixel *mpix,
 const zgl_Color color) {
    return zglDraw_PixelDot(screen,
                            &pixelrect_from_mpixelrect(*bounds),
                            &pixel_from_mpixel(*mpix),
                            color);
}

int zglDraw_mPixelCircle
(zgl_PixelArray *screen,
 const zgl_mPixelRect *bounds,
 const zgl_mPixelCircle *circle,
 const zgl_Color color) {
    return zglDraw_PixelCircle(screen,
                               &pixelrect_from_mpixelrect(*bounds),
                               &pixelcircle_from_mpixelcircle(*circle),
                               color);
}

#define ABS_MACRO(N) ((N) >= 0 ? (N) : -(N))

extern int zglDraw_mPixelSeg
(zgl_PixelArray *screen,
 const zgl_mPixelRect *bounds,
 const zgl_mPixel pixel0,
 const zgl_mPixel pixel1,
 const zgl_Color color) {
    zgl_mPixelSeg clipped_seg;
    if ( ! zgl_ClipSegToRect((zgl_mPixelSeg){pixel0, pixel1}, *bounds, &clipped_seg)) {
        return 1; //nothing to draw, seg is entirely outside boundary
    }
    
    zgl_PixelSeg pix_seg = pixelseg_from_mpixelseg(clipped_seg);
    
    if (pix_seg.pt0.x == pix_seg.pt1.x) {
        zglDraw_PixelSeg_Vertical(screen,
                                  pix_seg.pt0.x,
                                  pix_seg.pt0.y,
                                  pix_seg.pt1.y,
                                  color);
        return 2;
    }
    if (pix_seg.pt0.y == pix_seg.pt1.y) {
        zglDraw_PixelSeg_Horizontal(screen,
                              pix_seg.pt0.x,
                              pix_seg.pt1.x,
                              pix_seg.pt0.y,
                              color);
        return 2;
    }

    if (ABS_MACRO((int64_t)pix_seg.pt1.y - (int64_t)pix_seg.pt0.y)
        <
        ABS_MACRO((int64_t)pix_seg.pt1.x - (int64_t)pix_seg.pt0.x)) {
        if (pix_seg.pt0.x > pix_seg.pt1.x) {
            seg_low_bresenham(screen,
                              pix_seg.pt1.x,
                              pix_seg.pt1.y,
                              pix_seg.pt0.x,
                              pix_seg.pt0.y,
                              color);
        }
        else {
            seg_low_bresenham(screen,
                              pix_seg.pt0.x,
                              pix_seg.pt0.y,
                              pix_seg.pt1.x,
                              pix_seg.pt1.y,
                              color);
        }
    }
    else {
        if (pix_seg.pt0.y > pix_seg.pt1.y) {
            seg_high_bresenham(screen,
                               pix_seg.pt1.x,
                               pix_seg.pt1.y,
                               pix_seg.pt0.x,
                               pix_seg.pt0.y,
                               color);
        }
        else {
            seg_high_bresenham(screen,
                               pix_seg.pt0.x,
                               pix_seg.pt0.y,
                               pix_seg.pt1.x,
                               pix_seg.pt1.y,
                               color);
        }
    }
    
    return 0;
}

int zglDraw_mPixelSeg_Vertical
(zgl_PixelArray *screen,
 const zgl_mPixit x,
 const zgl_mPixit y0,
 const zgl_mPixit y1,
 const zgl_Color color) {
    return zglDraw_PixelSeg_Vertical(screen,
                                     pixit_from_mpixit(x),
                                     pixit_from_mpixit(y0),
                                     pixit_from_mpixit(y1),
                                     color);
}

int zglDraw_mPixelSeg_Horizontal
(zgl_PixelArray *screen,
 const zgl_mPixit x0,
 const zgl_mPixit x1,
 const zgl_mPixit y,
 const zgl_Color color) {    
    return zglDraw_PixelSeg_Horizontal(screen,
                                       pixit_from_mpixit(x0),
                                       pixit_from_mpixit(x1),
                                       pixit_from_mpixit(y),
                                       color);
}



extern int zglDraw_mPixelSeg_Dotted
(zgl_PixelArray *screen,
 const zgl_mPixelRect *bounds,
 const zgl_mPixel pixel0,
 const zgl_mPixel pixel1,
 const zgl_Color color) {
    zgl_mPixelSeg clipped_seg;
    if ( ! zgl_ClipSegToRect((zgl_mPixelSeg){pixel0, pixel1}, *bounds, &clipped_seg)) {
        return 1; //nothing to draw, seg is entirely outside boundary
    }
    
    zgl_PixelSeg pix_seg = pixelseg_from_mpixelseg(clipped_seg);
    
    if (pix_seg.pt0.x == pix_seg.pt1.x) {
        zglDraw_PixelSeg_Vertical_Dotted(screen,
                                  pix_seg.pt0.x,
                                  pix_seg.pt0.y,
                                  pix_seg.pt1.y,
                                  color);
        return 2;
    }
    if (pix_seg.pt0.y == pix_seg.pt1.y) {
        zglDraw_PixelSeg_Horizontal_Dotted(screen,
                              pix_seg.pt0.x,
                              pix_seg.pt1.x,
                              pix_seg.pt0.y,
                              color);
        return 2;
    }

    if (ABS_MACRO((int64_t)pix_seg.pt1.y - (int64_t)pix_seg.pt0.y)
        <
        ABS_MACRO((int64_t)pix_seg.pt1.x - (int64_t)pix_seg.pt0.x)) {
        if (pix_seg.pt0.x > pix_seg.pt1.x) {
            dotted_seg_low_bresenham(screen,
                              pix_seg.pt1.x,
                              pix_seg.pt1.y,
                              pix_seg.pt0.x,
                              pix_seg.pt0.y,
                              color);
        }
        else {
            dotted_seg_low_bresenham(screen,
                              pix_seg.pt0.x,
                              pix_seg.pt0.y,
                              pix_seg.pt1.x,
                              pix_seg.pt1.y,
                              color);
        }
    }
    else {
        if (pix_seg.pt0.y > pix_seg.pt1.y) {
            dotted_seg_high_bresenham(screen,
                               pix_seg.pt1.x,
                               pix_seg.pt1.y,
                               pix_seg.pt0.x,
                               pix_seg.pt0.y,
                               color);
        }
        else {
            dotted_seg_high_bresenham(screen,
                               pix_seg.pt0.x,
                               pix_seg.pt0.y,
                               pix_seg.pt1.x,
                               pix_seg.pt1.y,
                               color);
        }
    }
    
    return 0;
    
}

extern int zglDraw_mPixelSeg_Vertical_Dotted
(zgl_PixelArray *screen,
 const zgl_mPixit x,
 const zgl_mPixit y0,
 const zgl_mPixit y1,
 const zgl_Color color) {
    return zglDraw_PixelSeg_Vertical_Dotted(screen,
                                            pixit_from_mpixit(x),
                                            pixit_from_mpixit(y0),
                                            pixit_from_mpixit(y1),
                                            color);    
}

extern int zglDraw_mPixelSeg_Horizontal_Dotted
(zgl_PixelArray *screen,
 const zgl_mPixit x0,
 const zgl_mPixit x1,
 const zgl_mPixit y,
 const zgl_Color color) {
    return zglDraw_PixelSeg_Horizontal_Dotted(screen,
                                       pixit_from_mpixit(x0),
                                       pixit_from_mpixit(x1),
                                       pixit_from_mpixit(y),
                                       color);
}


/********************************************
 *  Drawing functions with Pixel arguments  *
 ********************************************/

int zglDraw_PixelDot
(zgl_PixelArray *screen,
 const zgl_PixelRect *bounds,
 const zgl_Pixel *pt,
 const zgl_Color color) {
    /* OLD
    if (!in_rect_pixel(*pt, *bounds)) {
        return 1;
    }
    
    zgl_PixelRect pixbox;
    pixbox.x = pt->x-1;
    pixbox.y = pt->y-1;
    pixbox.w = 3;
    pixbox.h = 3;
    zgl_FillRect(screen, pixbox.x, pixbox.y, pixbox.w, pixbox.h, color);
    */
    
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
    
    zgl_Pixel min_pt, max_pt;
    min_pt.x = MAX(bounds->x, (pt->x - 1));
    min_pt.y = MAX(bounds->y, (pt->y - 1));
    max_pt.x = MIN(bounds->x + bounds->w, (pt->x + 1));
    max_pt.y = MIN(bounds->y + bounds->h, (pt->y + 1));
    int w = screen->w;

    for (zgl_Pixit y = min_pt.y; y <= max_pt.y; y++) {
        for (zgl_Pixit x = min_pt.x; x <= max_pt.x; x++) {
            screen->pixels[x + y*w] = color;
        }   
    }

    return 0;
}

int zglDraw_PixelCircle
(zgl_PixelArray *screen,
 const zgl_PixelRect *_bounds,
 const zgl_PixelCircle *_circle,
 const zgl_Color color) {
    zgl_PixelRect bounds = *_bounds;
    zgl_PixelCircle circle = *_circle;
    
    if (circle.radius >= 2*bounds.w || circle.radius >= 2*bounds.h)
        return 1; // circle is completely outside view
    
    circle_bresenham(screen,
                     circle.center.x,
                     circle.center.y,
                     circle.radius,
                     bounds.x,
                     bounds.x + bounds.w - 1,
                     bounds.y,
                     bounds.y + bounds.h - 1,
                     color);

    return 0;
}


int zglDraw_PixelSeg
(zgl_PixelArray *screen,
 const zgl_PixelRect *_bounds,
 const zgl_PixelSeg *_seg,
 const zgl_Color color) {
    zgl_PixelRect bounds = *_bounds;
    zgl_PixelSeg seg = *_seg;

    zgl_mPixelSeg clipped_seg;
    if ( ! zgl_ClipSegToRect(mpixelseg_from_pixelseg(seg),
                             mpixelrect_from_pixelrect(bounds),
                             &clipped_seg)) {
        return 1; //nothing to draw, seg is entirely outside boundary
    }
    zgl_PixelSeg pix_seg = pixelseg_from_mpixelseg(clipped_seg);
    
    if (pix_seg.pt0.x == pix_seg.pt1.x) {
        zglDraw_PixelSeg_Vertical(screen,
                                  pix_seg.pt0.x,
                                  pix_seg.pt0.y,
                                  pix_seg.pt1.y,
                                  color);
        return 2;
    }
    if (pix_seg.pt0.y == pix_seg.pt1.y) {
        zglDraw_PixelSeg_Horizontal(screen,
                              pix_seg.pt0.x,
                              pix_seg.pt1.x,
                              pix_seg.pt0.y,
                              color);
        return 2;
    }

    // if change in y is less than change in x, then low_bresenham
    if (ABS_MACRO((int64_t)pix_seg.pt1.y - (int64_t)pix_seg.pt0.y)
        <
        ABS_MACRO((int64_t)pix_seg.pt1.x - (int64_t)pix_seg.pt0.x)) {
        if (pix_seg.pt0.x > pix_seg.pt1.x) {
            seg_low_bresenham(screen,
                              pix_seg.pt1.x,
                              pix_seg.pt1.y,
                              pix_seg.pt0.x,
                              pix_seg.pt0.y,
                              color);
        }
        else {
            seg_low_bresenham(screen,
                              pix_seg.pt0.x,
                              pix_seg.pt0.y,
                              pix_seg.pt1.x,
                              pix_seg.pt1.y,
                              color);
        }
    }
    else {
        if (pix_seg.pt0.y > pix_seg.pt1.y) {
            seg_high_bresenham(screen,
                               pix_seg.pt1.x,
                               pix_seg.pt1.y,
                               pix_seg.pt0.x,
                               pix_seg.pt0.y,
                               color);
        }
        else {
            seg_high_bresenham(screen,
                               pix_seg.pt0.x,
                               pix_seg.pt0.y,
                               pix_seg.pt1.x,
                               pix_seg.pt1.y,
                               color);
        }
    }
    
    return 0;
}

int zglDraw_PixelSeg_Vertical
(zgl_PixelArray *screen,
 const zgl_Pixit x,
 const zgl_Pixit y0,
 const zgl_Pixit y1,
 const zgl_Color color) {
    int inc, count, index;
    zgl_Color *pixels = screen->pixels;

    inc = screen->w;
    
    if (y0 > y1) {
        count = y0 - y1;
        index = y1*screen->w + x;
    }
    else {
        count = y1 - y0;
        index = y0*screen->w + x;
    }

    for (int i = 0; i <= count; i++) {
	    pixels[index] = color;
	    index += inc;
    }

    return 0;
}

int zglDraw_PixelSeg_Horizontal
(zgl_PixelArray *screen,
 const zgl_Pixit x0,
 const zgl_Pixit x1,
 const zgl_Pixit y,
 const zgl_Color color) {
    int inc, count, index;
    zgl_Color *pixels = screen->pixels;

    inc = 1;
    
    if (x0 > x1) {
        count = x0 - x1;
        index = y*screen->w + x1;
    }
    else {
        count = x1 - x0;
        index = y*screen->w + x0;
    }

    for (int i = 0; i <= count; i++) {
	    pixels[index] = color;
	    index += inc;
    }
    
    return 0;
}


extern int zglDraw_PixelSeg_Dotted
(zgl_PixelArray *screen,
 const zgl_PixelRect *_bounds,
 const zgl_PixelSeg *_seg,
 const zgl_Color color) {
    zgl_PixelRect bounds = *_bounds;
    zgl_PixelSeg seg = *_seg;

    zgl_mPixelSeg clipped_seg;
    if ( ! zgl_ClipSegToRect(mpixelseg_from_pixelseg(seg),
                             mpixelrect_from_pixelrect(bounds),
                             &clipped_seg)) {
        return 1; //nothing to draw, seg is entirely outside boundary
    }
    zgl_PixelSeg pix_seg = pixelseg_from_mpixelseg(clipped_seg);
    
    if (pix_seg.pt0.x == pix_seg.pt1.x) {
        zglDraw_PixelSeg_Vertical_Dotted(screen,
                                  pix_seg.pt0.x,
                                  pix_seg.pt0.y,
                                  pix_seg.pt1.y,
                                  color);
        return 2;
    }
    if (pix_seg.pt0.y == pix_seg.pt1.y) {
        zglDraw_PixelSeg_Horizontal_Dotted(screen,
                              pix_seg.pt0.x,
                              pix_seg.pt1.x,
                              pix_seg.pt0.y,
                              color);
        return 2;
    }

    // if change in y is less than change in x, then low_bresenham
    if (ABS_MACRO((int64_t)pix_seg.pt1.y - (int64_t)pix_seg.pt0.y)
        <
        ABS_MACRO((int64_t)pix_seg.pt1.x - (int64_t)pix_seg.pt0.x)) {
        if (pix_seg.pt0.x > pix_seg.pt1.x) {
            dotted_seg_low_bresenham(screen,
                              pix_seg.pt1.x,
                              pix_seg.pt1.y,
                              pix_seg.pt0.x,
                              pix_seg.pt0.y,
                              color);
        }
        else {
            dotted_seg_low_bresenham(screen,
                              pix_seg.pt0.x,
                              pix_seg.pt0.y,
                              pix_seg.pt1.x,
                              pix_seg.pt1.y,
                              color);
        }
    }
    else {
        if (pix_seg.pt0.y > pix_seg.pt1.y) {
            dotted_seg_high_bresenham(screen,
                               pix_seg.pt1.x,
                               pix_seg.pt1.y,
                               pix_seg.pt0.x,
                               pix_seg.pt0.y,
                               color);
        }
        else {
            dotted_seg_high_bresenham(screen,
                               pix_seg.pt0.x,
                               pix_seg.pt0.y,
                               pix_seg.pt1.x,
                               pix_seg.pt1.y,
                               color);
        }
    }
    
    return 0;
    
}

extern int zglDraw_PixelSeg_Vertical_Dotted
(zgl_PixelArray *screen,
 const zgl_Pixit x,
 const zgl_Pixit y0,
 const zgl_Pixit y1,
 const zgl_Color color) {
    int inc, count, index;
    zgl_Color *pixels = screen->pixels;

    inc = screen->w;
    
    if (y0 > y1) {
        count = y0 - y1;
        index = y1*screen->w + x;
    }
    else {
        count = y1 - y0;
        index = y0*screen->w + x;
    }

    bool even = true;
    for (int i = 0; i <= count; i++) {
        if (even) pixels[index] = color;
	    index += inc;
        even = !even;
    }

    return 0;    
}

extern int zglDraw_PixelSeg_Horizontal_Dotted
(zgl_PixelArray *screen,
 const zgl_Pixit x0,
 const zgl_Pixit x1,
 const zgl_Pixit y,
 const zgl_Color color) {
    int inc, count, index;
    zgl_Color *pixels = screen->pixels;

    inc = 1;
    
    if (x0 > x1) {
        count = x0 - x1;
        index = y*screen->w + x1;
    }
    else {
        count = x1 - x0;
        index = y*screen->w + x0;
    }

    bool even = true;
    for (int i = 0; i <= count; i++) {
        if (even) pixels[index] = color;
	    index += inc;
        even = !even;
    }
    
    return 0;
    
}



static int circle_bresenham
(zgl_PixelArray *screen,
 const zgl_Pixit cx,
 const zgl_Pixit cy,
 const zgl_Pixit r,
 const zgl_Pixit xmin,
 const zgl_Pixit xmax,
 const zgl_Pixit ymin,
 const zgl_Pixit ymax,
 const zgl_Color color) {
    
    //int r_squared = r*r;
    zgl_Pixit x, y;
    zgl_Pixit sx, sy;
    int32_t D; // needs signed since it acts as an offset which could be +/-
    zgl_Color *pixels = (zgl_Color*)screen->pixels;

    D = 3 - (2*r);
    x = r, y = 0;
    do {
        for (uint32_t i = 0; i < 8; i++) {
            switch (i) {
            case 0:
                sx =  x + cx;
                sy =  y + cy;
                break;
            case 1:
                sx =  y +  cx;
                sy =  x +  cy; 
                break;
            case 2:
                sx =  x +  cx;
                sy = -y +  cy;
                break;
            case 3:
                sx =  y +  cx;
                sy = -x +  cy;
                break;
            case 4:
                sx = -x +  cx;
                sy =  y +  cy;
                break;
            case 5:
                sx = -y +  cx;
                sy =  x +  cy;
                break;
            case 6:
                sx = -x +  cx;
                sy = -y +  cy;
                break;
            case 7:
                sx = -y +  cx;
                sy = -x +  cy;
                break;
            }
            if ((sx >= xmin)&&(sx <= xmax)&&(sy >= ymin)&&(sy <= ymax))
                pixels[(sy)*screen->w + (sx)] = color;
        }
	
	
        y++;
        if (D > 0) {
            x--;
            D = D + ((y-x) << 2) + 10;
        }
        else
            D = D + (y << 2) + 6;
    } while (x >= y);

		
    return 0;
}


static int seg_low_bresenham
(zgl_PixelArray *screen,
 const zgl_Pixit x0,
 const zgl_Pixit y0,
 const zgl_Pixit x1,
 const zgl_Pixit y1,
 const zgl_Color color) {
    dibassert(x1 < (zgl_Pixit)screen->w
             && x0 < (zgl_Pixit)screen->w
             && y1 < (zgl_Pixit)screen->h
             && y0 < (zgl_Pixit)screen->h);
    dibassert(x0 <= x1);
    
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;
    int32_t yi = 1;

    dibassert(dx >= 0);
    
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }

    int32_t D = (dy << 1) - dx;
    
    zgl_Color *pixels = (zgl_Color*)screen->pixels;
    int32_t x = x0, y = y0;
    
    for ( ; x <= x1; x++) {
        pixels[y*screen->w + x] = color;

        if (D > 0) {
            y = y + yi;
            D = D + ((dy - dx) << 1);
        }
        else {
            D = D + (dy << 1);
        }
    }

    return 0;
}

static int dotted_seg_low_bresenham
(zgl_PixelArray *screen,
 const zgl_Pixit x0,
 const zgl_Pixit y0,
 const zgl_Pixit x1,
 const zgl_Pixit y1,
 const zgl_Color color) {
    dibassert(x1 < (zgl_Pixit)screen->w
             && x0 < (zgl_Pixit)screen->w
             && y1 < (zgl_Pixit)screen->h
             && y0 < (zgl_Pixit)screen->h);
    dibassert(x0 <= x1);
    
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;
    int32_t yi = 1;

    dibassert(dx >= 0);
    
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }

    int32_t D = (dy << 1) - dx;
    
    zgl_Color *pixels = (zgl_Color*)screen->pixels;
    int32_t x = x0, y = y0;
    bool even = true;
    
    for ( ; x <= x1; x++) {
        if (even)
            pixels[y*screen->w + x] = color;

        if (D > 0) {
            y = y + yi;
            D = D + ((dy - dx) << 1);
        }
        else {
            D = D + (dy << 1);
        }

        even = !even;
    }

    return 0;
}


static int seg_high_bresenham
(zgl_PixelArray *screen,
 const zgl_Pixit x0,
 const zgl_Pixit y0,
 const zgl_Pixit x1,
 const zgl_Pixit y1,
 const zgl_Color color) {
    dibassert(x1 < (zgl_Pixit)screen->w
             && x0 < (zgl_Pixit)screen->w
             && y1 < (zgl_Pixit)screen->h
             && y0 < (zgl_Pixit)screen->h);
    dibassert(y0 <= y1);
    
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;
    int32_t xi = 1;
        
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }

    int32_t D = (dx << 1) - dy;
    
    zgl_Color *pixels = (zgl_Color*)screen->pixels;
    
    int32_t x = x0, y = y0;
    for ( ; y <= y1; y++) {
        pixels[y*screen->w + x] = color;
	
        if (D > 0) {
            x = x + xi;
            D = D + ((dx - dy) << 1);
        }
        else {
            D = D + (dx << 1);
        }
    }

    return 0;
}

static int dotted_seg_high_bresenham
(zgl_PixelArray *screen,
 const zgl_Pixit x0,
 const zgl_Pixit y0,
 const zgl_Pixit x1,
 const zgl_Pixit y1,
 const zgl_Color color) {
    dibassert(x1 < (zgl_Pixit)screen->w
             && x0 < (zgl_Pixit)screen->w
             && y1 < (zgl_Pixit)screen->h
             && y0 < (zgl_Pixit)screen->h);
    dibassert(y0 <= y1);
    
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;
    int32_t xi = 1;
        
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }

    int32_t D = (dx << 1) - dy;
    
    zgl_Color *pixels = (zgl_Color*)screen->pixels;
    bool even = true;
    int32_t x = x0, y = y0;
    for ( ; y <= y1; y++) {
        if (even)
            pixels[y*screen->w + x] = color;
	
        if (D > 0) {
            x = x + xi;
            D = D + ((dx - dy) << 1);
        }
        else {
            D = D + (dx << 1);
        }
        
        even = !even;
    }

    return 0;
}


static void cvb_low_bresenham
(zgl_Pixit x0,
 zgl_Pixit y0,
 zgl_Pixit x1,
 zgl_Pixit y1,
 uint32_t TorB,
 zgl_Pixit arr[3000][2],
 uint32_t span) {
    (void)span;
    
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;
    int32_t yi = 1;
    
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }

    int32_t D = (dy << 1) - dx;
    
    int x = x0, y = y0;
    
    for ( ; x <= (int)x1; x++) {

	    arr[x-x0][TorB] = y;
	    
        if (D > 0) {
            y = y + yi;
            D = D + ((dy - dx) << 1);
        }
        else {
            D = D + (dy << 1);
        }
    }

}

static void cvb_high_bresenham
(zgl_Pixit x0,
 zgl_Pixit y0,
 zgl_Pixit x1,
 zgl_Pixit y1,
 uint32_t TorB,
 zgl_Pixit arr[3000][2],
 uint32_t span) {
    (void)span;
    
    int32_t dx;
    int32_t dy = y1 - y0;
    int32_t xi;
    int32_t t_x;
    
    if (x1 < x0) {
        t_x = x1;
        xi = -1;
        dx = x0 - x1;
    }
    else {
        t_x = x0;
        xi = 1;
        dx = x1 - x0;
    }

    int32_t D = (dx << 1) - dy;
    
    zgl_Pixit x = x0, y = y0;
    bool x_was_incremented = true;
    for ( ; y <= y1; y++) {
        if (x_was_incremented) {
            arr[x-t_x][TorB] = y;
        }
	    
	
        if (D > 0) {
            x = x + xi;
            x_was_incremented = true;
            D = D + ((dx - dy) << 1);
        }
        else {
            x_was_incremented = false;
            D = D + (dx << 1);
        }
    }
}

void compute_vertical_bounds(zgl_Pixel LT, zgl_Pixel RT, zgl_Pixel LB, zgl_Pixel RB, zgl_Pixit arr[3000][2], uint32_t span) {
    if (ABS_MACRO(LT.y - RT.y) < ABS_MACRO(LT.x - RT.x)) {
        if (LT.x > RT.x) {
            cvb_low_bresenham(RT.x, RT.y, LT.x, LT.y, 0, arr, span);
        }
        else {
            cvb_low_bresenham(LT.x, LT.y, RT.x, RT.y, 0, arr, span);
        }
    }
    else {
        if (LT.y > RT.y) {
            cvb_high_bresenham(RT.x, RT.y, LT.x, LT.y, 0, arr, span);
        }
        else {
            cvb_high_bresenham(LT.x, LT.y, RT.x, RT.y, 0, arr, span);
        }
    }

    
    if (ABS_MACRO(LB.y - RB.y) < ABS_MACRO(LB.x - RB.x)) {
        if (LB.x > RB.x) {
            cvb_low_bresenham(RB.x, RB.y, LB.x, LB.y, 1, arr, span);
        }
        else {
            cvb_low_bresenham(LB.x, LB.y, RB.x, RB.y, 1, arr, span);
        }
    }
    else {
        if (LB.y > RB.y) {
            cvb_high_bresenham(RB.x, RB.y, LB.x, LB.y, 1, arr, span);
        }
        else {
            cvb_high_bresenham(LB.x, LB.y, RB.x, RB.y, 1, arr, span);
        }
    }
}






extern int zglDraw_PixelRect_Outline
(zgl_PixelArray *screen,
 zgl_PixelRect *_rect,
 zgl_Color color) {
    zgl_PixelRect rect = *_rect;
    
    zglDraw_PixelSeg_Horizontal(screen, rect.x, rect.x + rect.w - 1, rect.y,
                                color);
    zglDraw_PixelSeg_Horizontal(screen, rect.x, rect.x + rect.w - 1, rect.y + rect.h - 1,
                                color);

    zglDraw_PixelSeg_Vertical(screen, rect.x, rect.y, rect.y + rect.h - 1,
                              color);
    zglDraw_PixelSeg_Vertical(screen, rect.x + rect.w - 1, rect.y, rect.y + rect.h - 1,
                              color);


    return 0;
}
