#include <X11/Xutil.h>
#include <stdlib.h>

#include "src/diblib_local/dibassert.h"
#include "src/diblib_local/dibhash.h"
#include "src/X11/zglx_ZK_from_XK.h"

#define NUM_SUPPORTED_XK 82

/*
  A hash table that maps X keycodes to Zigil keycodes.

  This hash table should be *fast* so as to reduce input latency. Keys and
  values are stored in table entries (as opposed to stored by reference) to
  minimize indirection.

  Once the hash table is set up upon program initialization, it shall not be
  changed until it is destroyed upon program termination. Thus, speed of
  insertion and deletion is not a concern provided the initial creation and
  eventual destruction are sufficiently fast, which they are likely to be
  regardless of table type.

  With these requirements, the hash table shall use open-addressing.
  
 */

static UIntMap32 *ZK_list;

UInt32Pair inputs[NUM_SUPPORTED_XK] = {
    {XK_Pointer_Button1, ZK_POINTER_LEFT},
    {XK_Pointer_Button2, ZK_POINTER_MIDDLE},
    {XK_Pointer_Button3, ZK_POINTER_RIGHT},
    {XK_Pointer_Button4, ZK_POINTER_WHEELUP},
    {XK_Pointer_Button5, ZK_POINTER_WHEELDOWN},

    {XK_0, ZK_0},
    {XK_1, ZK_1},
    {XK_2, ZK_2},
    {XK_3, ZK_3},
    {XK_4, ZK_4},
    {XK_5, ZK_5},
    {XK_6, ZK_6},
    {XK_7, ZK_7},
    {XK_8, ZK_8},
    {XK_9, ZK_9},

    {XK_a, ZK_a},
    {XK_b, ZK_b},
    {XK_c, ZK_c},
    {XK_d, ZK_d},
    {XK_e, ZK_e},
    {XK_f, ZK_f},
    {XK_g, ZK_g},
    {XK_h, ZK_h},
    {XK_i, ZK_i},
    {XK_j, ZK_j},
    {XK_k, ZK_k},
    {XK_l, ZK_l},
    {XK_m, ZK_m},
    {XK_n, ZK_n},
    {XK_o, ZK_o},
    {XK_p, ZK_p},
    {XK_q, ZK_q},
    {XK_r, ZK_r},
    {XK_s, ZK_s},
    {XK_t, ZK_t},
    {XK_u, ZK_u},
    {XK_v, ZK_v},
    {XK_w, ZK_w},
    {XK_x, ZK_x},
    {XK_y, ZK_y},
    {XK_z, ZK_z},

    {XK_F1, ZK_F1},
    {XK_F2, ZK_F2},
    {XK_F3, ZK_F3},
    {XK_F4, ZK_F4},
    {XK_F5, ZK_F5},
    {XK_F6, ZK_F6},
    {XK_F7, ZK_F7},
    {XK_F8, ZK_F8},
    {XK_F9, ZK_F9},
    {XK_F10, ZK_F10},
    {XK_F11, ZK_F11},
    {XK_F12, ZK_F12},

    {XK_grave, ZK_GRAVE},
    {XK_minus, ZK_MINUS},
    {XK_equal, ZK_EQUALS},

    {XK_bracketleft, ZK_LEFTBRACKET},
    {XK_bracketright, ZK_RIGHTBRACKET},
    {XK_backslash, ZK_BACKSLASH},
    {XK_semicolon, ZK_SEMICOLON},
    {XK_apostrophe, ZK_APOSTROPHE},
    {XK_comma, ZK_COMMA},
    {XK_period, ZK_PERIOD},
    {XK_slash, ZK_SLASH},

    {XK_space, ZK_SPC},

    {XK_BackSpace, ZK_BACK},
    {XK_Tab, ZK_TAB},
    {XK_Return, ZK_RET},
    {XK_Escape, ZK_ESC},
    {XK_Delete, ZK_DEL},

    {XK_Caps_Lock, ZK_CAPS},
    {XK_Alt_L, ZK_LALT},
    {XK_Control_L, ZK_LCTRL},
    {XK_Shift_L, ZK_LSHIFT},
    {XK_Alt_R, ZK_RALT},
    {XK_Control_R, ZK_RCTRL},
    {XK_Shift_R, ZK_RSHIFT},

    {XK_Up, ZK_UP},
    {XK_Down, ZK_DOWN},
    {XK_Left, ZK_LEFT},
    {XK_Right, ZK_RIGHT},
};

void init_LK_from_XK(void) {
    // best seed known 29882297
    // max lookup len of 2, and only 7 keys actually take 2.
    
    ZK_list = create_UIntMap32(NUM_SUPPORTED_XK, inputs, "X11 to Zigil keycodes", 29882297);
    //print_UIntMap32(ZK_list);
    //constrain_max_lookup_UIntMap32(ZK_list, 1, UINT32_MAX);
}

void shut_LK_from_XK(void) {
    destroy_UIntMap32(ZK_list);
}

zgl_KeyCode ZK_from_XK(KeySym XK) {
    uint32_t val;
    
    if ( ! UIntMap32_lookup(ZK_list, (uint32_t)XK, &val)) {
        return ZK_NONE;
    }

    return val;
}

zgl_LongKeyCode LK_from_XK(KeySym XK) {
    return ZK_to_LK(ZK_from_XK(XK));
}
