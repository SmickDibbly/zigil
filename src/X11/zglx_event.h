#ifndef ZGLX_EVENT_H
#define ZGLX_EVENT_H

#include "src/X11/zglx.h"
#include <X11/Xlib.h>

extern zgl_Result init_XEvent(void);
extern zgl_Result term_XEvent(void);

#define X_EVENT_MASK_LEN 25
#define X_EVENT_MASK_LEN_STR "25"
#define ZGL_XEVENT_MASK						\
    ( 0								\
      /* | NoEventMask              */ /* N.A.             */	\
      | KeyPressMask	               /* KeyPress         */	\
      | KeyReleaseMask	               /* KeyRelease       */	\
      | ButtonPressMask	               /* ButtonPress      */	\
      | ButtonReleaseMask              /* ButtonRelease    */	\
      | EnterWindowMask 	       /* EnterNotify      */	\
      | LeaveWindowMask 	       /* LeaveNotify      */	\
      | PointerMotionMask	       /* MotionNotify     */	\
      /* | PointerMotionHintMask    */ /* N.A.             */	\
      /* | Button1MotionMask 	    */ /* MotionNotify     */	\
      /* | Button2MotionMask 	    */ /* MotionNotify     */	\
      /* | Button3MotionMask 	    */ /* MotionNotify     */	\
      /* | Button4MotionMask 	    */ /* MotionNotify     */	\
      /* | Button5MotionMask 	    */ /* MotionNotify     */	\
      /* | ButtonMotionMask 	    */ /* MotionNotify     */	\
      /* | KeymapStateMask 	    */ /* KeymapNotify     */	\
      | ExposureMask 	               /* Expose           */	\
      /* | VisibilityChangeMask     */ /* VisibilityNotify */	\
      | StructureNotifyMask	       /* multiple         */	\
      /* | ResizeRedirectMask       */ /* ResizeRequest    */	\
      /* | SubstructureNotifyMask   */ /* multiple         */	\
      /* | SubstructureRedirectMask */ /* multiple         */	\
      | FocusChangeMask	               /* FocusIn FocusOut */	\
      /* | PropertyChangeMask       */ /* PropertyNotify   */	\
      /* | ColormapChangeMask       */ /* ColormapNotify   */	\
      /* | OwnerGrabButtonMask      */ /* N.A.             */	\
      )

#endif /* ZGLX_EVENT_H */
