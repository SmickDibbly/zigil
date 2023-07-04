/* This file is only to be included by "zglx.c". */

//#define EDITING

/* Prevent including this file while its EDITING token is defined. */
#ifdef INCLUDE_SRC
# ifdef EDITING
#  error Undefine EDITING in source file before including it.
# endif
#endif


/* Editing-specific includes to satisfy Emacs flycheck. Meant to be ignored when
   another source file includes this one. */
#ifdef EDITING
# define X_VERBOSITY
# include <stdio.h>
# include <X11/Xlib.h>
# include "src/X11/zglx.h"
# include "src/X11/zglx_event.h"
# define STATIC
#else
# define STATIC static
#endif



#ifndef X_VERBOSITY
# define print_Display(ignored) ((void)0)
# define print_Display_and_Screen(ignored, ignored2) ((void)0)
# define print_Screen(ignored) ((void)0)
# define print_XWindowAttributes(ignored) ((void)0)
# define print_Visual(ignored) ((void)0)
# define print_XGCValues(ignored) ((void)0)
# define print_XImage(ignored) ((void)0)
# define print_X11_info(ignored) ((void)0)
#else

#include "src/X11/zglx_macro_str.h"

STATIC void print_Display(Display *const dpy) {
    puts(  "              X Display Data");
    puts(  " ---------------------------");
    lputf( "                Display (*)", "p", (void *)dpy);
    lputf( "          Connection Number", "i", ConnectionNumber(dpy));
    lputf( "             Display String", "s", DisplayString(dpy));
    lputf( "              Server Vendor", "s", ServerVendor(dpy));
    lputf( "             Vendor Release", "i", VendorRelease(dpy));
    lputf( "           Protocol Version", "i", ProtocolVersion(dpy));
    lputf( "          Protocol Revision", "i", ProtocolRevision(dpy));
    lputf( "                Bitmap Unit", "i", BitmapUnit(dpy));
    lputf( "           Bitmap Bit Order", "s", Byte_Order_strs[BitmapBitOrder(dpy)]);
    lputf( "                 Bitmap Pad", "i", BitmapPad(dpy));
    lputf( "           Image Byte Order", "s", Byte_Order_strs[ImageByteOrder(dpy)]);
    lputf( "  Event Queue Length (TODO)", "i", QLength(dpy));
    lputf( "  Extended Max Request Size", "li", XExtendedMaxRequestSize(dpy));
    lputf( "           Max Request Size", "li", XMaxRequestSize(dpy));
    lputf( "               Next Request", "lu", NextRequest(dpy));
    lputf( "               Last Request", "lu", LastKnownRequestProcessed(dpy));
    lputf( " Default Root Window (hndl)", "lu", DefaultRootWindow(dpy));
    lputf( "     Default Screen (index)", "i", DefaultScreen(dpy));
    lputf( "               Screen Count", "i", ScreenCount(dpy));
}

STATIC void print_Display_and_Screen(Display *const dpy, int i_scr) {
    puts(       "  X Display & Screen Data");
    puts(       "-------------------------");
    lputf(      "   Screen of Display (*)", "p", (void *)ScreenOfDisplay(dpy, i_scr));
    lputf(     "              BlackPixel", "#.6lX", BlackPixel(dpy,i_scr));
    lputf(     "              WhitePixel", "#.6lX", WhitePixel(dpy,i_scr));
    lputf(      "      Display Width (px)", "i", DisplayWidth(dpy, i_scr));
    lputf(      "     Display Height (px)", "i", DisplayHeight(dpy, i_scr));
    lputf(      "      Display Width (mm)", "i", DisplayWidthMM(dpy, i_scr));
    lputf(      "     Display Height (mm)", "i", DisplayHeightMM(dpy, i_scr));
    lputf(      "          Display Planes", "i", DisplayPlanes(dpy, i_scr));
    lputf(      "           Display Cells", "i", DisplayCells(dpy, i_scr));
    int count;
    int *depth_arr = XListDepths(dpy, i_scr, &count);
    lputf_arr(  "        Available Depths", "i", depth_arr, count);
    XFree(depth_arr);
    lputf(      "           Default Depth", "i", DefaultDepth(dpy, i_scr));
    lputf(      "      Default Visual (*)", "p", (void *)DefaultVisual(dpy,i_scr));
    lputf(      "          Default GC (*)", "p", (void *)&DefaultGC(dpy,i_scr));
    lputf(      "Default Color Map (hndl)", "lu", DefaultColormap(dpy, i_scr));

    XPixmapFormatValues *pfv_arr;
    pfv_arr = XListPixmapFormats(dpy, &count);
    puts("Supported Pixmap Formats:");
    puts("[");
    for (int i = 0; i < count; i++) {
        printf("    { [.depth] = %i, [.bits_per_pixel] = %i, [.scanline_pad] = %i },\n", pfv_arr[i].depth, pfv_arr[i].bits_per_pixel, pfv_arr[i].scanline_pad);
    }
    puts("]");
    XFree(pfv_arr);
}

STATIC void print_Screen(Screen *const s) {
    puts(       "           X Screen Data");
    puts(       "------------------------");    
    lputf(      "             BlackPixel", "#.6lX", BlackPixelOfScreen(s));
    lputf(      "             WhitePixel", "#.6lX", WhitePixelOfScreen(s));
    lputf(      "                  Cells", "i",  CellsOfScreen(s));
    lputf(      "Default Colormap (hndl)", "lu", DefaultColormapOfScreen(s));
    lputf(      "          Default Depth", "i",  DefaultDepthOfScreen(s));
    lputf(      "         Default GC (*)", "p", (void *)&DefaultGCOfScreen(s));
    lputf(      "     Default Visual (*)", "p", (void *)DefaultVisualOfScreen(s));
    lputf(      "     Does Backing Store", "s", Backing_Store_strs[DoesBackingStore(s)]);
    lput_bool(   "       Does Save Unders", DoesSaveUnders(s));
    lputf(      "  Display of Screen (*)", "p", (void *)DisplayOfScreen(s));
    lputf(      "        Index of Screen", "i", XScreenNumberOfScreen(s));
    lputdf(      "             Event Mask", "#=."X_EVENT_MASK_LEN_STR"bu", EventMaskOfScreen(s));
    lputf(      "      Screen Width (px)", "i", WidthOfScreen(s));
    lputf(      "     Screen Height (px)", "i", HeightOfScreen(s));
    lputf(      "      Screen Width (mm)", "i", WidthMMOfScreen(s));
    lputf(      "     Screen Height (mm)", "i", HeightMMOfScreen(s));
    lputf(      "          Max Colormaps", "i", MaxCmapsOfScreen(s));
    lputf(      "          Min Colormaps", "i", MinCmapsOfScreen(s));
    lputf(      "                 Planes", "i", PlanesOfScreen(s));
    lputf(      "     Root Window (hndl)", "lu", RootWindowOfScreen(s));
}

void print_XWindowAttributes(XWindowAttributes const *const atr) {
    puts(       "    X Window Attributes");
    puts(       "-----------------------");
    lputf(      "                    .x", "i", atr->x);
    lputf(      "                    .y", "i", atr->y);
    lputf(      "                .width", "i", atr->width);
    lputf(      "               .height", "i", atr->height);
    lputf(      "         .border_width", "i", atr->border_width);
    lputf(      "                .depth", "i", atr->depth);
    lputf(      "           .visual (*)", "p", (void *)&atr->visual);
    lputf(      "          .root (hndl)", "lu", atr->root);
    lputf(      "                .class", "s", Class_strs[atr->class]);
    lputf(      "          .bit_gravity", "s", Bit_Gravity_strs[atr->bit_gravity]);
    lputf(      "          .win_gravity", "s", Window_Gravity_strs[atr->win_gravity]);
    lputf(      "        .backing_store", "s", Backing_Store_strs[atr->backing_store]);
    lputdf(     "       .backing_planes", "#=.32bu", atr->backing_planes);
    lputdf(     "        .backing_pixel", "#=.6Xu", atr->backing_pixel);
    lput_bool(     "           .save_under", atr->save_under);
    lputdf(      "      .colormap (hndl)", "du", atr->colormap);
    lput_bool(     "        .map_installed", atr->map_installed);
    lputf(      "            .map_state", "s", Map_State_strs[atr->map_state]);
    lputdf( "      .all_event_masks", "#=."X_EVENT_MASK_LEN_STR"bu", atr->all_event_masks);
    lputdf( "      .your_event_mask", "#=."X_EVENT_MASK_LEN_STR"bu", atr->your_event_mask);
    lputdf( ".do_not_propagate_mask", "#=."X_EVENT_MASK_LEN_STR"bu", atr->do_not_propagate_mask);
    lput_bool(     "    .override_redirect", atr->override_redirect);
    lputf(      "           .screen (*)", "p", (void *)atr->screen);
}

STATIC void print_Visual(Visual *const v) {
    puts(       "X Visual Data");
    puts(       "-------------");
    lputf(      "    VisualID", "lu", XVisualIDFromVisual(v));
    lputf(      "       Class", "s", Display_Class_strs[v->class]);
    lputdf("    Red Mask", "#=.6Xu", v->red_mask);
    lputdf("  Green Mask", "#=.6Xu", v->green_mask);
    lputdf("   Blue Mask", "#=.6Xu", v->blue_mask);
    lputf(      "Bits Per RGB", "i", v->bits_per_rgb);
    lputf(      " Map Entries", "i", v->map_entries);
}

#if 0
STATIC void print_GCValues(XGCValues const *const gcv) {
    puts(        "  X Graphics Context Data");
    puts(        "-------------------------");
    lputf(      "               .function", "di", gcv->function);
    lputf(      "             .plane_mask", "#=.32bu", gcv->plane_mask);
    lputf(      "             .foreground", "#=.6Xu", gcv->foreground);
    lputf(      "             .background", "#=.6Xu", gcv->background);
    lputf(      "             .line_width", "di", gcv->line_width);
    lputf(      "             .line_style", "s", Line_Style_str[gcv->line_style]);
    lputf(      "              .cap_style", "s", Cap_Style_str[gcv->cap_style]);
    lputf(      "             .join_style", "s", Join_Style_str[gcv->join_style]);
    lputf(      "             .fill_style", "s", Fill_Style_str[gcv->fill_style]);
    lputf(      "              .fill_rule", "s", Fill_Rule_str[gcv->fill_rule]);
    lputf(      "               .arc_mode", "s", Arc_Mode_str[gcv->arc_mode]);
    lputf(      "     .tile (Pixmap hndl)", "du", gcv->tile);
    lputf(      "  .stipple (Pixmap hndl)", "du", gcv->stipple);
    lputf(      "            .ts_x_origin", "di", gcv->ts_x_origin);
    lputf(      "            .ts_y_origin", "di", gcv->ts_y_origin);
    lputf(      "            .font (hndl)", "du", gcv->font);
    lputf(      "         .subwindow_mode", "s", Subwindow_Mode_str[gcv->subwindow_mode]);
    put_l_bool(     "     .graphics_exposures", gcv->graphics_exposures);
    lputf(      "          .clip_x_origin", "di", gcv->clip_x_origin);
    lputf(      "          .clip_y_origin", "di", gcv->clip_y_origin);
    lputf(      ".clip_mask (Pixmap hndl)", "du", gcv->clip_mask);
    lputf(      "            .dash_offset", "di", gcv->dash_offset);
    // TODO lputf(      "                 .dashes", "c", gcv->dashes);
}
#endif


STATIC void print_XImage(XImage const *const ximg) {
    puts(       "       XImage Data");
    puts(       "------------------");
    lputf(      "           .width", "i", ximg->width);
    lputf(      "          .height", "i", ximg->height);
    lputf(      "          .format", "s", Image_Format_strs[ximg->format]);
    lputf(      "        .data (*)", "p", ximg->data);
    lputf(      "      .byte_order", "s", Byte_Order_strs[ximg->byte_order]);
    lputf(      "     .bitmap_unit", "i", ximg->bitmap_unit);
    lputf(      ".bitmap_bit_order", "s", Byte_Order_strs[ximg->bitmap_bit_order]);
    lputf(      "      .bitmap_pad", "i", ximg->bitmap_pad);
    lputf(      "           .depth", "i", ximg->depth);
    lputf(      "  .bytes_per_line", "i", ximg->bytes_per_line);
    lputf(      "  .bits_per_pixel", "i", ximg->bits_per_pixel);
    lputdf("        .red_mask", "#=.6Xu", ximg->red_mask);
    lputdf("      .green_mask", "#=.6Xu", ximg->green_mask);
    lputdf("       .blue_mask", "#=.6Xu", ximg->blue_mask);
}

STATIC void print_X11_info(void) {
    printf(         "x11_info struct\n");
    printf(         "--------------------\n");
    lputf(      "       .display (*)", "p", (void *)x11_info.display);
    lputf(      "           .scr (*)", "p", (void *)x11_info.scr);
    lputf(      "             .i_scr", "i", x11_info.i_scr);
    putchar('\n');
    lputf(      "   .root_win (hndl)", "lu", x11_info.root_win);
    lputf(      "        .win (hndl)", "lu", x11_info.win_info.win);
    lputf(      "         .win_width", "i", x11_info.width);
    lputf(      "        .win_height", "i", x11_info.height);
    lputf(      "         .win_depth", "i", x11_info.depth);
    lputf(      "       .win_vis (*)", "p", (void *)x11_info.win_vis);
    lputdf("  .win_vis_red_mask", "#=.6Xu", x11_info.win_vis_red_mask);
    lputdf(".win_vis_green_mask", "#=.6Xu", x11_info.win_vis_green_mask);
    lputdf(" .win_vis_blue_mask", "#=.6Xu", x11_info.win_vis_blue_mask);
    lputdf(      "        .win_gc (*)", "p", (void *)x11_info.win_gc);
    putchar('\n');
    lputf(      "        .ximage (*)", "p", (void *)x11_info.win_info.ximage);
    lputf(      "      .ximage_width", "i", x11_info.ximage_width);
    lputf(      "     .ximage_height", "i", x11_info.ximage_height);
    lputf(      "      .ximage_depth", "i", x11_info.ximage_depth);
    lputf(      "     .ximage_bitspp", "i", x11_info.ximage_bitspp);
    lputdf("   .ximage_red_mask", "#=.6Xu", x11_info.ximage_red_mask);
    lputdf(" .ximage_green_mask", "#=.6Xu", x11_info.ximage_green_mask);
    lputdf("  .ximage_blue_mask", "#=.6Xu", x11_info.ximage_blue_mask);
    putchar('\n');
    lput_bool(     "           .use_shm", x11_info.use_shm);
#if X11_HAS_SHM_EXT
    lputf(      "    .shminfo.shmseg", "lu", x11_info.shminfo.shmseg);
    lputf(      "     .shminfo.shmid", "i", x11_info.shminfo.shmid);
    lputf(      "   .shminfo.shmaddr", "s", x11_info.shminfo.shmaddr);
    lput_bool(     "  .shminfo.readOnly", x11_info.shminfo.readOnly);
#endif
}
#endif
