#ifndef ZIGIL_EVENT_H
#define ZIGIL_EVENT_H

//#include "zigil.h"
#include "zigil_input.h"

// IDEA: Instead of mere keycodes and KeyPress and KeyRelease, how about:

// 1) Button: A one-time input. This input is received, an action is
// performed. Simple.

// 2) Switch: The input is received with a binary state datum that alternates
// between ON and OFF each time the input is received.

// 3) Axis: The input is received with a value within a certain linearly-ordered
// range, for example -1 to 1.

// How do we map from "raw" inputs to the the more abstract types above? For
// example, a key might sometimes cause only a ButtonInput on being pressed,
// whereas other times the same key might map to a SwitchInput, with the state
// datum set according to whether the key is pressed or released. Do we always
// generate all kinds of input type from each "raw" input, which can then be
// ignored as needed by the subsequent handlers? Or do we have a way to
// communicate from application to input engine that "now this key is a
// button... okay now it is a switch"

// How do we robustly handle key combinations? If SHIFT and A pressed, it is
// easy enough to translate that to one Shift+A input, but should the separate
// Shift and A inputs still be generated? If Shift+A is pressed and then Shift
// is released, does that mean Shift+A combination was "released"? Does it also
// count as "pressing" A even though the physical A key was already pressed?

// Observation: Modifier keys are easy for ButtonInput.

// Idea: Since every Button/Switch/Axis input is ultimately tied to some
// (combination of) raw inputs, maybe we delay interpretation of raw input until
// specifically asked. This is how my VirtualDPad works: Zigil doesn't
// automatically compute DPad inputs, instead the Application registers a DPad
// with Zigil as a combination of four keys, and then when the Application
// receives a KeyPress or KeyRelease event and finds that the key is one of the
// four DPad keys, it then calls the "get DPad input" function of Zigil which
// translates the combined states of the DPad buttons into one of 8 directions.

typedef enum zgl_EventInputType {
    EIT_Pointer,
    EIT_Key,
    NUM_EIT
} zgl_EventInputType;

typedef enum zgl_EventCode {
    EC_PointerPress,
    EC_PointerRelease,
    EC_PointerMotion,
    EC_PointerEnter,
    EC_PointerLeave,
    EC_KeyPress,
    EC_KeyRelease,
    EC_CloseRequest,
    NUM_EC,
} zgl_EventCode;

typedef struct zgl_KeyPressEvent {
    zgl_KeyCode zk;
    zgl_LongKeyCode lk;
    uint8_t mod_flags;
} zgl_KeyPressEvent;

typedef struct zgl_KeyReleaseEvent {
    zgl_KeyCode zk;
    zgl_LongKeyCode lk;
    uint8_t mod_flags;
} zgl_KeyReleaseEvent;

typedef struct zgl_PointerPressEvent {
    int x, y;
    zgl_KeyCode zk;
    zgl_LongKeyCode lk;
    uint8_t mod_flags;
    uint8_t but_flags;
} zgl_PointerPressEvent;

typedef struct zgl_PointerReleaseEvent {
    int x, y;
    zgl_KeyCode zk;
    zgl_LongKeyCode lk;
    uint8_t mod_flags;
    uint8_t but_flags;
} zgl_PointerReleaseEvent;

typedef struct zgl_PointerMotionEvent {
    int x, y;
    int dx, dy;
    uint8_t mod_flags;
    uint8_t but_flags;
} zgl_PointerMotionEvent;

typedef struct zgl_PointerEnterEvent {
    int x, y;
} zgl_PointerEnterEvent;

typedef struct zgl_PointerLeaveEvent {
    int x, y;
} zgl_PointerLeaveEvent;

typedef struct zgl_Event {
    zgl_EventInputType input_type;
    zgl_EventCode type;

    union {
        zgl_KeyPressEvent key_press;
        zgl_KeyReleaseEvent key_release;
        zgl_PointerPressEvent pointer_press;
        zgl_PointerReleaseEvent pointer_release;
        zgl_PointerMotionEvent pointer_motion;
        zgl_PointerEnterEvent pointer_enter;
        zgl_PointerLeaveEvent pointer_leave;
    } u;
} zgl_Event;

extern zgl_Result zgl_UpdateEventQueue(void);
extern zgl_Result zgl_WaitPopEvent(zgl_Event *evt);
extern zgl_Result zgl_PopEvent(zgl_Event *evt);
extern zgl_Result zgl_PushEvent(zgl_Event *evt);

// Some special events have their own push function.
extern zgl_Result zgl_PushCloseEvent(void);

#endif
