/* Clipping algorithms */

#include "src/zigil.h"
#include "src/diblib_local/dibassert.h"

extern bool zgl_ClipSegToRect
(zgl_mPixelSeg seg,
 zgl_mPixelRect rect,
 zgl_mPixelSeg *clipped_seg);

extern bool zgl_ClipSegToPoly
(zgl_mPixelSeg seg,
 zgl_mPixelPolygon poly,
 zgl_mPixelSeg *clipped_seg);



static uint32_t compute_outcode
(zgl_mPixit x, zgl_mPixit y,
 zgl_mPixit xmin, zgl_mPixit xmax,
 zgl_mPixit ymin, zgl_mPixit ymax);
static bool zgl_CohenSutherland
(zgl_mPixit x0, zgl_mPixit y0,
 zgl_mPixit x1, zgl_mPixit y1,
 zgl_mPixit *cx0, zgl_mPixit *cy0,
 zgl_mPixit *cx1, zgl_mPixit *cy1,
 zgl_mPixit xmin, zgl_mPixit xmax,
 zgl_mPixit ymin, zgl_mPixit ymax);
/*
static bool zgl_LiangBarsky
(Fix32Point_2D pt0, Fix32Point_2D pt1,
 Fix32Point_2D *c0, Fix32Point_2D *c1,
 fix32_t _xmin, fix32_t _xmax,
 fix32_t _ymin, fix32_t _ymax);
*/

static int crossing_number
(zgl_mPixel P,
 zgl_mPixelPolygon poly,
 int n);
static Fix32 magic_function
(zgl_mPixel EV,
 zgl_mPixel SV,
 zgl_mPixel LV,
 int8_t *collide_type,
 Fix32 *t);
static bool clip_cyrus_beck
(zgl_mPixel p0,
 zgl_mPixel p1,
 zgl_mPixelPolygon poly,
 zgl_mPixel *ip0,
 zgl_mPixel *ip1);


// Function definitions.


bool zgl_ClipSegToRect
(zgl_mPixelSeg seg,
 zgl_mPixelRect rect,
 zgl_mPixelSeg *clipped_seg) {
    return zgl_CohenSutherland
        (seg.pt0.x, seg.pt0.y,
         seg.pt1.x, seg.pt1.y,
         &clipped_seg->pt0.x, &clipped_seg->pt0.y,
         &clipped_seg->pt1.x, &clipped_seg->pt1.y,
         rect.x, rect.x+rect.w-1,
         rect.y, rect.y+rect.h-1);
}


bool zgl_ClipSegToPoly
(zgl_mPixelSeg seg,
 zgl_mPixelPolygon poly,
 zgl_mPixelSeg *clipped_seg) {
    return clip_cyrus_beck
        (seg.pt0, seg.pt1, poly, &clipped_seg->pt0, &clipped_seg->pt1);
}




#define INSIDE 0
#define LEFT   1
#define RIGHT  2
#define BOTTOM 4
#define TOP    8

// Technically Pixel/mPixel generic
static uint32_t compute_outcode
(zgl_mPixit x,    zgl_mPixit y,
 zgl_mPixit xmin, zgl_mPixit xmax,
 zgl_mPixit ymin, zgl_mPixit ymax) {   
    uint32_t code = INSIDE;

    if (x < xmin)
        code |= LEFT;
    else if (x > xmax)
        code |= RIGHT;
    
    if (y < ymin)
        code |= BOTTOM;
    else if (y > ymax)
        code |= TOP;

    return code;
}

static bool zgl_CohenSutherland
(zgl_mPixit x0,   zgl_mPixit y0,
 zgl_mPixit x1,   zgl_mPixit y1,
 zgl_mPixit *cx0, zgl_mPixit *cy0,
 zgl_mPixit *cx1, zgl_mPixit *cy1,
 zgl_mPixit xmin, zgl_mPixit xmax,
 zgl_mPixit ymin, zgl_mPixit ymax) {
    // https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
    
    uint32_t outcode0 = compute_outcode(x0, y0, xmin, xmax, ymin, ymax);
    uint32_t outcode1 = compute_outcode(x1, y1, xmin, xmax, ymin, ymax);
    bool accept = false;
    
    while (true) {
        if ( ! (outcode0 | outcode1)) { // both endpoints inside; visible and no
                                        // clipping required
            accept = true;
            break;
        }
        else if (outcode0 & outcode1) { // both endpoints in SAME outside zone;
                                        // not visible
            break;
        }
        else {
            zgl_mPixit x, y;

            // pick an endpoint which is outside (at least one exists)
            uint32_t outcode_out = (outcode1 > outcode0) ? outcode1 : outcode0;

            if (outcode_out & TOP) {
                x = x0 + (int32_t)FIX_DIV(FIX_MUL(x1-x0, ymax-y0), y1-y0);
                y = ymax;
            }
            else if (outcode_out & BOTTOM) {
                x = x0 + (int32_t)FIX_DIV(FIX_MUL(x1-x0, ymin-y0), y1-y0);
                y = ymin;
            }
            else if (outcode_out & RIGHT) {
                y = y0 + (int32_t)FIX_DIV(FIX_MUL(y1-y0, xmax-x0), x1-x0);
                x = xmax;
            }
            else if (outcode_out & LEFT) {
                y = y0 + (int32_t)FIX_DIV(FIX_MUL(y1-y0, xmin-x0), x1-x0);
                x = xmin;
            }
            else {
                dibassert(false);
            }

            
            if (outcode_out == outcode0) {
                x0 = x;
                y0 = y;
                outcode0 = compute_outcode(x0, y0, xmin, xmax, ymin, ymax);
            }
            else {
                x1 = x;
                y1 = y;
                outcode1 = compute_outcode(x1, y1, xmin, xmax, ymin, ymax);
            }
        }
    }

    *cx0 = x0;
    *cy0 = y0;
    *cx1 = x1;
    *cy1 = y1;
    
    return accept;
}

#undef INSIDE
#undef LEFT
#undef RIGHT
#undef BOTTOM
#undef TOP



static int crossing_number
(zgl_mPixel P,
 zgl_mPixelPolygon poly,
 int n) {
    int cn = 0;
    
    for (int i = 0; i < n; i++) {
        if (((poly.pt[i].y <= P.y) && (poly.pt[i+1].y > P.y))
            || ((poly.pt[i].y > P.y) && (poly.pt[i+1].y <= P.y))) {
            Fix32 vt = (Fix32)FIX_DIV(P.y - poly.pt[i].y, poly.pt[i+1].y - poly.pt[i].y);
            if (P.x < poly.pt[i].x + (int32_t)FIX_MUL(vt, poly.pt[i+1].x - poly.pt[i].x))
                ++cn;
        }
    }
    return (cn & 1);

}

#define POLY_ENTER 0
#define POLY_EXIT 1

static Fix32 magic_function
(zgl_mPixel EV,
 zgl_mPixel SV,
 zgl_mPixel LV,
 int8_t *collide_type,
 Fix32 *t) {
    double N, D;
    // TODO: No more doubles!
    N = (FIX_dbl(EV.x)*FIX_dbl(LV.y) - FIX_dbl(EV.y)*FIX_dbl(LV.x));
    D = ((FIX_dbl(EV.x)*FIX_dbl(SV.y) - FIX_dbl(EV.y)*FIX_dbl(SV.x)));

    //  N = 0 means edge_vec and link_vec are parallel (and therefore point 0 of seg_vec is ON edge line)
    //  N < 0 means that seg_vec starts outside current polygon edge
    //  N > 0 means that seg_vec starts inside current polygon edge
    
    //  D = 0 means edge_vec and  seg_vec are parallel
    //  D > 0 means seg_vec points left from the persepctive of the edge
    //  D < 0 means seg_vec points right from the perspective of the edge
    //  note that if both are 0, then seg_vec and edge_vec define the very same (infinitely extended) line, but it still must be checked exactly where, if anywhere, they overlap.

    // sign tells us whether the returned time value t is an enter time or exit time
    
#define LEFTWARD < 0
#define RIGHTWARD > 0
#define CENTER == 0

    if (D CENTER) {
        if (N RIGHTWARD) {
            return -2;
        }
        else {
            return -1;
        }
    }
    else if (D LEFTWARD) {
        *collide_type = POLY_ENTER;
        if (N LEFTWARD || N CENTER) {
            *t = 0;
            return 0;
        }
        else { //(N RIGHTWARD)
            if (-N - D LEFTWARD) {
                return -2;
            }
            else {
                *t = (Fix32)dbl_FIX(-N/D);
                return 0;
            }
        }
    }
    else { //(D RIGHTWARD)
        *collide_type = POLY_EXIT;
        if (N RIGHTWARD || N CENTER) {
            return -2;
        }
        else { //(N LEFTWARD)
            if (-N - D RIGHTWARD) {
                *t = fixify(1);
                return 0;
            }
            else {
                *t = (Fix32)dbl_FIX(-N/D);
                return 0;
            }
        }
    }

#undef LEFTWARD 
#undef RIGHTWARD
#undef CENTER	

} /* magic_function */


static bool clip_cyrus_beck
(zgl_mPixel p0,
 zgl_mPixel p1,
 zgl_mPixelPolygon poly,
 zgl_mPixel *ip0,
 zgl_mPixel *ip1) {
    /* Clip a segment to a polygon. */
    dibassert(poly.pt[0].x == poly.pt[poly.num_points].x);
    dibassert(poly.pt[0].y == poly.pt[poly.num_points].y);
    
    if (p0.x == p1.x && p0.y == p1.y) {
        ip0->x = p0.x;
        ip0->y = p0.y;
        ip1->x = p1.x;
        ip1->y = p1.y;
        return crossing_number((zgl_mPixel){p0.x, p0.y}, poly, poly.num_points);
    }
    
    Fix32 t_in = fixify(0);
    Fix32 t_out = fixify(1);
    Fix32 t;
    zgl_mPixel SV;
    zgl_mPixel LV;
    zgl_mPixel EV;

    // vector from p0 to p1
    SV.x = p1.x - p0.x;
    SV.y = p1.y - p0.y;
    
    for (uint32_t i = 0; i < poly.num_points; i++) {
        // vector from one polygon vertex to the next
        EV.x = (poly.pt[i+1].x - poly.pt[i].x);
        EV.y = (poly.pt[i+1].y - poly.pt[i].y);

        // vector from polygon first polygon vertex (of two above) to p0
        LV.x = (p0.x - poly.pt[i].x);
        LV.y = (p0.y - poly.pt[i].y);
        int8_t collide_type;
	
        int return_value = magic_function(EV, SV, LV, &collide_type, &t);
	    
        if (return_value == -1) {
            continue;
        }
        if (return_value == -2) {
            return false;
        }
		
        if (collide_type == POLY_ENTER) {
            if (t > t_in) {
                t_in = t;
                if (t_out < t_in) {
                    return false;
                }
            }
        }
        else {
            if (t < t_out) {
                t_out = t;
                if (t_out < t_in) {
                    return false;
                }
            }
        }
    }

    ip0->x = (p0.x) + (Fix32)FIX_MUL(t_in, SV.x);
    ip0->y = (p0.y) + (Fix32)FIX_MUL(t_in, SV.y);
    ip1->x = (p0.x) + (Fix32)FIX_MUL(t_out, SV.x);
    ip1->y = (p0.y) + (Fix32)FIX_MUL(t_out, SV.y);

    return true;
}

#undef POLY_ENTER
#undef POLY_EXIT








/* In theory Liang-Barsky is more efficient than Cohen-Sutherland, but in
   practice it always seems to be 10-20 *times* slower */
/*
static bool zgl_LiangBarsky
(Fix32Point_2D pt0, Fix32Point_2D pt1,
 Fix32Point_2D *c0, Fix32Point_2D *c1,
 Fix32 xmin, Fix32 xmax,
 Fix32 ymin, Fix32 ymax) {
    *c0 = pt0;
    *c1 = pt1;
    
    // function originally written for -z
    Fix32 x0   = pt0.x;
    Fix32 y0   = pt0.y;
    Fix32 x1   = pt1.x;
    Fix32 y1   = pt1.y;
    
    // defining variables
    Fix32 neg_dx = -(x1 - x0);
    Fix32 pos_dx = -neg_dx;
    Fix32 neg_dy = -(y1 - y0);
    Fix32 pos_dy = -neg_dy;

    Fix32 q0 = x0 - xmin;
    Fix32 q1 = xmax - x0;
    Fix32 q2 = y0 - ymin;
    Fix32 q3 = ymax - y0;

    Fix32 posarr[5], negarr[5];
    int posind = 1, negind = 1;
    posarr[0] = fixify(1);
    negarr[0] = fixify(0);

    if ((neg_dx == 0 && q0 < 0) || (pos_dx == 0 && q1 < 0) ||
        (neg_dy == 0 && q2 < 0) || (pos_dy == 0 && q3 < 0)) {
        //printf("Line is parallel to clipping window!\n");
        return false;
    }
    
    if (neg_dx != 0) {
        Fix32 r0 = FIX_DIV(q0, neg_dx);
        Fix32 r1 = FIX_DIV(q1, pos_dx);
        if (neg_dx < 0) {
            negarr[negind++] = r0; // for negative pos_dx, add it to negative array
            posarr[posind++] = r1; // and add neg_dy to positive array
        } else {
            negarr[negind++] = r1;
            posarr[posind++] = r0;
        }
    }
    
    if (neg_dy != 0) {
        Fix32 r2 = FIX_DIV(q2, neg_dy);
        Fix32 r3 = FIX_DIV(q3, pos_dy);
        if (neg_dy < 0) {
            negarr[negind++] = r2;
            posarr[posind++] = r3;
        } else {
            negarr[negind++] = r3;
            posarr[posind++] = r2;
        }
    }

    Fix32 T0 = fixify(0);
    for (int i = 0; i < negind; ++i)
        if (T0 < negarr[i])
            T0 = negarr[i];

    Fix32 T1 = fixify(1);
    for (int i = 0; i < posind; ++i)
        if (T1 > posarr[i])
            T1 = posarr[i];
    
    if (T0 > T1)  { // reject
        //printf("Line is outside the clipping window!\n");
        return false;
    }

    // computing new points
    c0->x = x0 + FIX_MUL(T0, pos_dx);
    c0->y = y0 + FIX_MUL(T0, pos_dy);
    c1->x = x0 + FIX_MUL(T1, pos_dx);
    c1->y = y0 + FIX_MUL(T1, pos_dy);
    
    return true;
}
*/
