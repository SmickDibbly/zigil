#include "src/zigil_keycodes.h"
#include <stddef.h>
#include <string.h>


uint8_t ZK_states[NUM_ZK] = {0};
uint8_t LK_states[NUM_LK] = {0};
uint8_t mouse_states = 0;
uint8_t modifier_states = 0;

char const *const ZK_strs[NUM_ZK] = {
    [ZK_NONE] = "<NONE>",
     
    [ZK_POINTER_LEFT] = "<MOUSE LEFT>",
    [ZK_POINTER_MIDDLE] = "<MOUSE MID>",
    [ZK_POINTER_RIGHT] = "<MOUSE RIGHT>",
    [ZK_POINTER_X1] = "<MOUSE X1>",
    [ZK_POINTER_X2] = "<MOUSE X2>",
    [ZK_POINTER_WHEELUP] = "<MSWHEEL UP>",
    [ZK_POINTER_WHEELDOWN] = "<MSWHEEL DOWN>",
    
    [ZK_0] = "0",
    [ZK_1] = "1",
    [ZK_2] = "2",
    [ZK_3] = "3",
    [ZK_4] = "4",
    [ZK_5] = "5",
    [ZK_6] = "6",
    [ZK_7] = "7",
    [ZK_8] = "8",
    [ZK_9] = "9",
    [ZK_a] = "a",
    [ZK_b] = "b",
    [ZK_c] = "c",
    [ZK_d] = "d",
    [ZK_e] = "e",
    [ZK_f] = "f",
    [ZK_g] = "g",
    [ZK_h] = "h",
    [ZK_i] = "i",
    [ZK_j] = "j",
    [ZK_k] = "k",
    [ZK_l] = "l",
    [ZK_m] = "m",
    [ZK_n] = "n",
    [ZK_o] = "o",
    [ZK_p] = "p",
    [ZK_q] = "q",
    [ZK_r] = "r",
    [ZK_s] = "s",
    [ZK_t] = "t",
    [ZK_u] = "u",
    [ZK_v] = "v",
    [ZK_w] = "w",
    [ZK_x] = "x",
    [ZK_y] = "y",
    [ZK_z] = "z",
     
    [ZK_F1]  = "<F1>",
    [ZK_F2]  = "<F2>",
    [ZK_F3]  = "<F3>",
    [ZK_F4]  = "<F4>",
    [ZK_F5]  = "<F5>",
    [ZK_F6]  = "<F6>",
    [ZK_F7]  = "<F7>",
    [ZK_F8]  = "<F8>",
    [ZK_F9]  = "<F9>",
    [ZK_F10] = "<F10>",
    [ZK_F11] = "<F11>",
    [ZK_F12] = "<F12>",

    [ZK_LALT] = "<LALT>",
    [ZK_LCTRL] = "<LCTRL>",
    [ZK_LSHIFT] = "<LSHIFT>",
    [ZK_RALT] = "<RALT>",
    [ZK_RCTRL] = "<RCTRL>",
    [ZK_RSHIFT] = "<RSHIFT>",
     
    [ZK_BACK] = "<BACKSPC>",
    [ZK_ESC] = "<ESC>",
    [ZK_CAPS] = "<CAPSLOCK>",
    [ZK_DEL] = "<DEL>",

    [ZK_RET] = "<RET>",
    [ZK_SPC] = "<SPC>", 
    [ZK_TAB] = "<TAB>",
    [ZK_GRAVE] = "`",
    [ZK_MINUS] = "-",
    [ZK_EQUALS] = "=",
    [ZK_LEFTBRACKET] = "[",
    [ZK_RIGHTBRACKET] = "]",
    [ZK_BACKSLASH] = "\\",
    [ZK_SEMICOLON] = ";",
    [ZK_APOSTROPHE] = "'",
    [ZK_COMMA] = ",",
    [ZK_PERIOD] = ".",
    [ZK_SLASH] = "/",
    [ZK_UP] = "<UP>",
    [ZK_DOWN] = "<DOWN>",
    [ZK_LEFT] = "<LEFT>",
    [ZK_RIGHT] = "<RIGHT>",    
};

char const *const LK_strs[NUM_LK] = {
    [LK_NONE] = "<NONE>",
     
    [LK_POINTER_LEFT] = "<MOUSE LEFT>",
    [LK_POINTER_MIDDLE] = "<MOUSE MID>",
    [LK_POINTER_RIGHT] = "<MOUSE RIGHT>",
    [LK_POINTER_X1] = "<MOUSE X1>",
    [LK_POINTER_X2] = "<MOUSE X2>",
    [LK_POINTER_WHEELUP] = "<MSWHEEL UP>",
    [LK_POINTER_WHEELDOWN] = "<MSWHEEL DOWN>",
    
    [LK_0] = "0",
    [LK_1] = "1",
    [LK_2] = "2",
    [LK_3] = "3",
    [LK_4] = "4",
    [LK_5] = "5",
    [LK_6] = "6",
    [LK_7] = "7",
    [LK_8] = "8",
    [LK_9] = "9",
    [LK_a] = "a",
    [LK_b] = "b",
    [LK_c] = "c",
    [LK_d] = "d",
    [LK_e] = "e",
    [LK_f] = "f",
    [LK_g] = "g",
    [LK_h] = "h",
    [LK_i] = "i",
    [LK_j] = "j",
    [LK_k] = "k",
    [LK_l] = "l",
    [LK_m] = "m",
    [LK_n] = "n",
    [LK_o] = "o",
    [LK_p] = "p",
    [LK_q] = "q",
    [LK_r] = "r",
    [LK_s] = "s",
    [LK_t] = "t",
    [LK_u] = "u",
    [LK_v] = "v",
    [LK_w] = "w",
    [LK_x] = "x",
    [LK_y] = "y",
    [LK_z] = "z",
     
    [LK_F1]  = "<F1>",
    [LK_F2]  = "<F2>",
    [LK_F3]  = "<F3>",
    [LK_F4]  = "<F4>",
    [LK_F5]  = "<F5>",
    [LK_F6]  = "<F6>",
    [LK_F7]  = "<F7>",
    [LK_F8]  = "<F8>",
    [LK_F9]  = "<F9>",
    [LK_F10] = "<F10>",
    [LK_F11] = "<F11>",
    [LK_F12] = "<F12>",

    [LK_LALT] = "<LALT>",
    [LK_LCTRL] = "<LCTRL>",
    [LK_LSHIFT] = "<LSHIFT>",
    [LK_RALT] = "<RALT>",
    [LK_RCTRL] = "<RCTRL>",
    [LK_RSHIFT] = "<RSHIFT>",
     
    [LK_BACK] = "<BACKSPC>",
    [LK_ESC] = "<ESC>",
    [LK_CAPS] = "<CAPSLOCK>",
    [LK_DEL] = "<DEL>",

    [LK_RET] = "<RET>",
    [LK_SPC] = "<SPC>", 
    [LK_TAB] = "<TAB>",
    [LK_GRAVE] = "`",
    [LK_MINUS] = "-",
    [LK_EQUALS] = "=",
    [LK_LEFTBRACKET] = "[",
    [LK_RIGHTBRACKET] = "]",
    [LK_BACKSLASH] = "\\",
    [LK_SEMICOLON] = ";",
    [LK_APOSTROPHE] = "'",
    [LK_COMMA] = ",",
    [LK_PERIOD] = ".",
    [LK_SLASH] = "/",
    [LK_UP] = "<UP>",
    [LK_DOWN] = "<DOWN>",
    [LK_LEFT] = "<LEFT>",
    [LK_RIGHT] = "<RIGHT>",

    [LK_C_RET] = "C-<RET>",
    [LK_C_SPC] = "C-<SPC>", 
    [LK_C_TAB] = "C-<TAB>",
    [LK_C_GRAVE] = "C-`",
    [LK_C_MINUS] = "C--",
    [LK_C_EQUALS] = "C-=",
    [LK_C_LEFTBRACKET] = "C-[",
    [LK_C_RIGHTBRACKET] = "C-]",
    [LK_C_BACKSLASH] = "C-\\",
    [LK_C_SEMICOLON] = "C-;",
    [LK_C_APOSTROPHE] = "C-'",
    [LK_C_COMMA] = "C-,",
    [LK_C_PERIOD] = "C-.",
    [LK_C_SLASH] = "C-/",
    [LK_C_UP] = "C-<UP>",
    [LK_C_DOWN] = "C-<DOWN>",
    [LK_C_LEFT] = "C-<LEFT>",
    [LK_C_RIGHT] = "C-<RIGHT>",
    [LK_C_0] = "C-0",
    [LK_C_1] = "C-1",
    [LK_C_2] = "C-2",
    [LK_C_3] = "C-3",
    [LK_C_4] = "C-4",
    [LK_C_5] = "C-5",
    [LK_C_6] = "C-6",
    [LK_C_7] = "C-7",
    [LK_C_8] = "C-8",
    [LK_C_9] = "C-9",
    [LK_C_a] = "C-a",
    [LK_C_b] = "C-b",
    [LK_C_c] = "C-c",
    [LK_C_d] = "C-d",
    [LK_C_e] = "C-e",
    [LK_C_f] = "C-f",
    [LK_C_g] = "C-g",
    [LK_C_h] = "C-h",
    [LK_C_i] = "C-i",
    [LK_C_j] = "C-j",
    [LK_C_k] = "C-k",
    [LK_C_l] = "C-l",
    [LK_C_m] = "C-m",
    [LK_C_n] = "C-n",
    [LK_C_o] = "C-o",
    [LK_C_p] = "C-p",
    [LK_C_q] = "C-q",
    [LK_C_r] = "C-r",
    [LK_C_s] = "C-s",
    [LK_C_t] = "C-t",
    [LK_C_u] = "C-u",
    [LK_C_v] = "C-v",
    [LK_C_w] = "C-w",
    [LK_C_x] = "C-x",
    [LK_C_y] = "C-y",
    [LK_C_z] = "C-z",

    [LK_M_RET] = "M-<RET>",
    [LK_M_SPC] = "M-<SPC>", 
    [LK_M_TAB] = "M-<TAB>",
    [LK_M_GRAVE] = "M-`",
    [LK_M_MINUS] = "M--",
    [LK_M_EQUALS] = "M-=",
    [LK_M_LEFTBRACKET] = "M-[",
    [LK_M_RIGHTBRACKET] = "M-]",
    [LK_M_BACKSLASH] = "M-\\",
    [LK_M_SEMICOLON] = "M-;",
    [LK_M_APOSTROPHE] = "M-'",
    [LK_M_COMMA] = "M-,",
    [LK_M_PERIOD] = "M-.",
    [LK_M_SLASH] = "M-/",
    [LK_M_UP] = "M-<UP>",
    [LK_M_DOWN] = "M-<DOWN>",
    [LK_M_LEFT] = "M-<LEFT>",
    [LK_M_RIGHT] = "M-<RIGHT>",
    [LK_M_0] = "M-0",
    [LK_M_1] = "M-1",
    [LK_M_2] = "M-2",
    [LK_M_3] = "M-3",
    [LK_M_4] = "M-4",
    [LK_M_5] = "M-5",
    [LK_M_6] = "M-6",
    [LK_M_7] = "M-7",
    [LK_M_8] = "M-8",
    [LK_M_9] = "M-9",     
    [LK_M_a] = "M-a",
    [LK_M_b] = "M-b",
    [LK_M_c] = "M-c",
    [LK_M_d] = "M-d",
    [LK_M_e] = "M-e",
    [LK_M_f] = "M-f",
    [LK_M_g] = "M-g",
    [LK_M_h] = "M-h",
    [LK_M_i] = "M-i",
    [LK_M_j] = "M-j",
    [LK_M_k] = "M-k",
    [LK_M_l] = "M-l",
    [LK_M_m] = "M-m",
    [LK_M_n] = "M-n",
    [LK_M_o] = "M-o",
    [LK_M_p] = "M-p",
    [LK_M_q] = "M-q",
    [LK_M_r] = "M-r",
    [LK_M_s] = "M-s",
    [LK_M_t] = "M-t",
    [LK_M_u] = "M-u",
    [LK_M_v] = "M-v",
    [LK_M_w] = "M-w",
    [LK_M_x] = "M-x",
    [LK_M_y] = "M-y",
    [LK_M_z] = "M-z",

    [LK_S_RET] = "S-<RET>",
    [LK_S_SPC] = "S-<SPC>", 
    [LK_S_TAB] = "S-<TAB>",
    [LK_S_GRAVE] = "~",
    [LK_S_MINUS] = "_",
    [LK_S_EQUALS] = "+",
    [LK_S_LEFTBRACKET] = "{",
    [LK_S_RIGHTBRACKET] = "}",
    [LK_S_BACKSLASH] = "|",
    [LK_S_SEMICOLON] = ":",
    [LK_S_APOSTROPHE] = "\"",
    [LK_S_COMMA] = "<",
    [LK_S_PERIOD] = ">",
    [LK_S_SLASH] = "?",
    [LK_S_UP] = "S-<UP>",
    [LK_S_DOWN] = "S-<DOWN>",
    [LK_S_LEFT] = "S-<LEFT>",
    [LK_S_RIGHT] = "S-<RIGHT>",
    [LK_S_0] = ")",
    [LK_S_1] = "!",
    [LK_S_2] = "@",
    [LK_S_3] = "#",
    [LK_S_4] = "$",
    [LK_S_5] = "%",
    [LK_S_6] = "^",
    [LK_S_7] = "&",
    [LK_S_8] = "*",
    [LK_S_9] = "(",     
    [LK_S_a] = "A",
    [LK_S_b] = "B",
    [LK_S_c] = "C",
    [LK_S_d] = "D",
    [LK_S_e] = "E",
    [LK_S_f] = "F",
    [LK_S_g] = "G",
    [LK_S_h] = "H",
    [LK_S_i] = "I",
    [LK_S_j] = "J",
    [LK_S_k] = "K",
    [LK_S_l] = "L",
    [LK_S_m] = "M",
    [LK_S_n] = "N",
    [LK_S_o] = "O",
    [LK_S_p] = "P",
    [LK_S_q] = "Q",
    [LK_S_r] = "R",
    [LK_S_s] = "S",
    [LK_S_t] = "T",
    [LK_S_u] = "U",
    [LK_S_v] = "V",
    [LK_S_w] = "W",
    [LK_S_x] = "X",
    [LK_S_y] = "Y",
    [LK_S_z] = "Z",


    [LK_C_M_RET] = "C-M-<RET>",
    [LK_C_M_SPC] = "C-M-<SPC>", 
    [LK_C_M_TAB] = "C-M-<TAB>",
    [LK_C_M_GRAVE] = "C-M-`",
    [LK_C_M_MINUS] = "C-M--",
    [LK_C_M_EQUALS] = "C-M-=",
    [LK_C_M_LEFTBRACKET] = "C-M-[",
    [LK_C_M_RIGHTBRACKET] = "C-M-]",
    [LK_C_M_BACKSLASH] = "C-M-\\",
    [LK_C_M_SEMICOLON] = "C-M-;",
    [LK_C_M_APOSTROPHE] = "C-M-'",
    [LK_C_M_COMMA] = "C-M-,",
    [LK_C_M_PERIOD] = "C-M-.",
    [LK_C_M_SLASH] = "C-M-/",
    [LK_C_M_UP] = "C-M-<UP>",
    [LK_C_M_DOWN] = "C-M-<DOWN>",
    [LK_C_M_LEFT] = "C-M-<LEFT>",
    [LK_C_M_RIGHT] = "C-M-<RIGHT>",
    [LK_C_M_0] = "C-M-0",
    [LK_C_M_1] = "C-M-1",
    [LK_C_M_2] = "C-M-2",
    [LK_C_M_3] = "C-M-3",
    [LK_C_M_4] = "C-M-4",
    [LK_C_M_5] = "C-M-5",
    [LK_C_M_6] = "C-M-6",
    [LK_C_M_7] = "C-M-7",
    [LK_C_M_8] = "C-M-8",
    [LK_C_M_9] = "C-M-9",     
    [LK_C_M_a] = "C-M-a",
    [LK_C_M_b] = "C-M-b",
    [LK_C_M_c] = "C-M-c",
    [LK_C_M_d] = "C-M-d",
    [LK_C_M_e] = "C-M-e",
    [LK_C_M_f] = "C-M-f",
    [LK_C_M_g] = "C-M-g",
    [LK_C_M_h] = "C-M-h",
    [LK_C_M_i] = "C-M-i",
    [LK_C_M_j] = "C-M-j",
    [LK_C_M_k] = "C-M-k",
    [LK_C_M_l] = "C-M-l",
    [LK_C_M_m] = "C-M-m",
    [LK_C_M_n] = "C-M-n",
    [LK_C_M_o] = "C-M-o",
    [LK_C_M_p] = "C-M-p",
    [LK_C_M_q] = "C-M-q",
    [LK_C_M_r] = "C-M-r",
    [LK_C_M_s] = "C-M-s",
    [LK_C_M_t] = "C-M-t",
    [LK_C_M_u] = "C-M-u",
    [LK_C_M_v] = "C-M-v",
    [LK_C_M_w] = "C-M-w",
    [LK_C_M_x] = "C-M-x",
    [LK_C_M_y] = "C-M-y",
    [LK_C_M_z] = "C-M-z",

    [LK_C_S_RET] = "C-S-<RET>",
    [LK_C_S_SPC] = "C-S-<SPC>", 
    [LK_C_S_TAB] = "C-S-<TAB>",
    [LK_C_S_GRAVE] = "C-~",
    [LK_C_S_MINUS] = "C-_",
    [LK_C_S_EQUALS] = "C-+",
    [LK_C_S_LEFTBRACKET] = "C-{",
    [LK_C_S_RIGHTBRACKET] = "C-}",
    [LK_C_S_BACKSLASH] = "C-|",
    [LK_C_S_SEMICOLON] = "C-:",
    [LK_C_S_APOSTROPHE] = "C-\"",
    [LK_C_S_COMMA] = "C-<",
    [LK_C_S_PERIOD] = "C->",
    [LK_C_S_SLASH] = "C-?",
    [LK_C_S_UP] = "C-S-<UP>",
    [LK_C_S_DOWN] = "C-S-<DOWN>",
    [LK_C_S_LEFT] = "C-S-<LEFT>",
    [LK_C_S_RIGHT] = "C-S-<RIGHT>",
    [LK_C_S_0] = "C-)",
    [LK_C_S_1] = "C-!",
    [LK_C_S_2] = "C-@",
    [LK_C_S_3] = "C-#",
    [LK_C_S_4] = "C-$",
    [LK_C_S_5] = "C-%",
    [LK_C_S_6] = "C-^",
    [LK_C_S_7] = "C-&",
    [LK_C_S_8] = "C-*",
    [LK_C_S_9] = "C-(",
    [LK_C_S_a] = "C-A",
    [LK_C_S_b] = "C-B",
    [LK_C_S_c] = "C-C",
    [LK_C_S_d] = "C-D",
    [LK_C_S_e] = "C-E",
    [LK_C_S_f] = "C-F",
    [LK_C_S_g] = "C-G",
    [LK_C_S_h] = "C-H",
    [LK_C_S_i] = "C-I",
    [LK_C_S_j] = "C-J",
    [LK_C_S_k] = "C-K",
    [LK_C_S_l] = "C-L",
    [LK_C_S_m] = "C-M",
    [LK_C_S_n] = "C-N",
    [LK_C_S_o] = "C-O",
    [LK_C_S_p] = "C-P",
    [LK_C_S_q] = "C-Q",
    [LK_C_S_r] = "C-R",
    [LK_C_S_s] = "C-S",
    [LK_C_S_t] = "C-T",
    [LK_C_S_u] = "C-U",
    [LK_C_S_v] = "C-V",
    [LK_C_S_w] = "C-W",
    [LK_C_S_x] = "C-X",
    [LK_C_S_y] = "C-Y",
    [LK_C_S_z] = "C-Z",

    [LK_M_S_RET] = "M-S-<RET>",
    [LK_M_S_SPC] = "M-S-<SPC>", 
    [LK_M_S_TAB] = "M-S-<TAB>",
    [LK_M_S_GRAVE] = "M-~",
    [LK_M_S_MINUS] = "M-_",
    [LK_M_S_EQUALS] = "M-+",
    [LK_M_S_LEFTBRACKET] = "M-{",
    [LK_M_S_RIGHTBRACKET] = "M-}",
    [LK_M_S_BACKSLASH] = "M-|",
    [LK_M_S_SEMICOLON] = "M-:",
    [LK_M_S_APOSTROPHE] = "M-\"",
    [LK_M_S_COMMA] = "M-<",
    [LK_M_S_PERIOD] = "M->",
    [LK_M_S_SLASH] = "M-?",
    [LK_M_S_UP] = "M-S-<UP>",
    [LK_M_S_DOWN] = "M-S-<DOWN>",
    [LK_M_S_LEFT] = "M-S-<LEFT>",
    [LK_M_S_RIGHT] = "M-S-<RIGHT>",
    [LK_M_S_0] = "M-)",
    [LK_M_S_1] = "M-!",
    [LK_M_S_2] = "M-@",
    [LK_M_S_3] = "M-#",
    [LK_M_S_4] = "M-$",
    [LK_M_S_5] = "M-%",
    [LK_M_S_6] = "M-^",
    [LK_M_S_7] = "M-&",
    [LK_M_S_8] = "M-*",
    [LK_M_S_9] = "M-(",     
    [LK_M_S_a] = "M-A",
    [LK_M_S_b] = "M-B",
    [LK_M_S_c] = "M-C",
    [LK_M_S_d] = "M-D",
    [LK_M_S_e] = "M-E",
    [LK_M_S_f] = "M-F",
    [LK_M_S_g] = "M-G",
    [LK_M_S_h] = "M-H",
    [LK_M_S_i] = "M-I",
    [LK_M_S_j] = "M-J",
    [LK_M_S_k] = "M-K",
    [LK_M_S_l] = "M-L",
    [LK_M_S_m] = "M-M",
    [LK_M_S_n] = "M-N",
    [LK_M_S_o] = "M-O",
    [LK_M_S_p] = "M-P",
    [LK_M_S_q] = "M-Q",
    [LK_M_S_r] = "M-R",
    [LK_M_S_s] = "M-S",
    [LK_M_S_t] = "M-T",
    [LK_M_S_u] = "M-U",
    [LK_M_S_v] = "M-V",
    [LK_M_S_w] = "M-W",
    [LK_M_S_x] = "M-X",
    [LK_M_S_y] = "M-Y",
    [LK_M_S_z] = "M-Z",

    [LK_C_M_S_RET] = "C-M-S-<RET>",
    [LK_C_M_S_SPC] = "C-M-S-<SPC>", 
    [LK_C_M_S_TAB] = "C-M-S-<TAB>",
    [LK_C_M_S_GRAVE] = "C-M-~",
    [LK_C_M_S_MINUS] = "C-M-_",
    [LK_C_M_S_EQUALS] = "C-M-+",
    [LK_C_M_S_LEFTBRACKET] = "C-M-{",
    [LK_C_M_S_RIGHTBRACKET] = "C-M-}",
    [LK_C_M_S_BACKSLASH] = "C-M-|",
    [LK_C_M_S_SEMICOLON] = "C-M-:",
    [LK_C_M_S_APOSTROPHE] = "C-M-\"",
    [LK_C_M_S_COMMA] = "C-M-<",
    [LK_C_M_S_PERIOD] = "C-M->",
    [LK_C_M_S_SLASH] = "C-M-?",
    [LK_C_M_S_UP] = "C-M-S-<UP>",
    [LK_C_M_S_DOWN] = "C-M-S-<DOWN>",
    [LK_C_M_S_LEFT] = "C-M-S-<LEFT>",
    [LK_C_M_S_RIGHT] = "C-M-S-<RIGHT>",
    [LK_C_M_S_0] = "C-M-)",
    [LK_C_M_S_1] = "C-M-!",
    [LK_C_M_S_2] = "C-M-@",
    [LK_C_M_S_3] = "C-M-#",
    [LK_C_M_S_4] = "C-M-$",
    [LK_C_M_S_5] = "C-M-%",
    [LK_C_M_S_6] = "C-M-^",
    [LK_C_M_S_7] = "C-M-&",
    [LK_C_M_S_8] = "C-M-*",
    [LK_C_M_S_9] = "C-M-(",     
    [LK_C_M_S_a] = "C-M-A",
    [LK_C_M_S_b] = "C-M-B",
    [LK_C_M_S_c] = "C-M-C",
    [LK_C_M_S_d] = "C-M-D",
    [LK_C_M_S_e] = "C-M-E",
    [LK_C_M_S_f] = "C-M-F",
    [LK_C_M_S_g] = "C-M-G",
    [LK_C_M_S_h] = "C-M-H",
    [LK_C_M_S_i] = "C-M-I",
    [LK_C_M_S_j] = "C-M-J",
    [LK_C_M_S_k] = "C-M-K",
    [LK_C_M_S_l] = "C-M-L",
    [LK_C_M_S_m] = "C-M-M",
    [LK_C_M_S_n] = "C-M-N",
    [LK_C_M_S_o] = "C-M-O",
    [LK_C_M_S_p] = "C-M-P",
    [LK_C_M_S_q] = "C-M-Q",
    [LK_C_M_S_r] = "C-M-R",
    [LK_C_M_S_s] = "C-M-S",
    [LK_C_M_S_t] = "C-M-T",
    [LK_C_M_S_u] = "C-M-U",
    [LK_C_M_S_v] = "C-M-V",
    [LK_C_M_S_w] = "C-M-W",
    [LK_C_M_S_x] = "C-M-X",
    [LK_C_M_S_y] = "C-M-Y",
    [LK_C_M_S_z] = "C-M-Z",
};

#include "src/diblib_local/dibhash.h"
static Index *ZK_str_index;
static Index *LK_str_index;
// TODO: Is it worth having an index type that has external storage for the
// keys? Since we are guaranteed to be keeping all the keys in ZK_strs[] and
// LK_strs[], the index could be set to hold pointers to that constant and
// pre-existing memory instead of duplicating everything internally.

void zgl_InitKeyCodes(void) {
    ZK_str_index = create_Index(0, NULL, "Zigil ZK-string-to-code", 0);
    LK_str_index = create_Index(0, NULL, "Zigil LK-string-to-code", 0);

    for (zgl_KeyCode ZK = ZK_NONE; ZK < NUM_ZK; ZK++) {
        Index_insert(ZK_str_index, (StrInt){ZK_strs[ZK], ZK});
    }
    for (zgl_LongKeyCode LK = LK_NONE; LK < NUM_LK; LK++) {
        Index_insert(LK_str_index, (StrInt){LK_strs[LK], LK});
    }
}

zgl_KeyCode ZK_str_to_ZK(char const *ZK_str) {
    uint64_t ZK;
    if (Index_lookup(ZK_str_index, ZK_str, &ZK)) {
        return (zgl_KeyCode)ZK;
    }
 
    return ZK_NONE;
}

zgl_LongKeyCode LK_str_to_LK(char const *LK_str) {
    uint64_t LK;
    if (Index_lookup(LK_str_index, LK_str, &LK)) {
        return (zgl_LongKeyCode)LK;
    }
    
    return LK_NONE;
}

static inline bool prefix(const char *pre, const char *str) {
    return strncmp(pre, str, strlen(pre)) == 0;
}

zgl_Result LK_str_to_KeyComplex(char const *str, zgl_KeyComplex *out_KC) {
    zgl_KeyComplex KC = {0};

    KC.lk = LK_str_to_LK(str);
    
    if (prefix("C-M-S-", str)) {
        KC.mod_states = ZGL_CTRL_MASK | ZGL_ALT_MASK | ZGL_SHIFT_MASK;
        str += 6;
    }
    else if (prefix("M-S-", str)) {
        KC.mod_states = ZGL_ALT_MASK | ZGL_SHIFT_MASK;
        str += 4;
    }
    else if (prefix("C-S-", str)) {
        KC.mod_states = ZGL_CTRL_MASK | ZGL_SHIFT_MASK;
        str += 4;
    }
    else if (prefix("C-M-", str)) {
        KC.mod_states = ZGL_CTRL_MASK | ZGL_ALT_MASK;
        str += 4;
    }
    else if (prefix("S-", str)) {
        KC.mod_states = ZGL_SHIFT_MASK;
        str += 2;
    }
    else if (prefix("M-", str)) {
        KC.mod_states = ZGL_ALT_MASK;
        str += 2;
    }
    else if (prefix("C-", str)) {
        KC.mod_states = ZGL_CTRL_MASK;
        str += 2;
    }

    KC.zk = ZK_str_to_ZK(str);

    *out_KC = KC;

    return ZR_SUCCESS;
}




#define NUM_MOD_COMBO 8
static zgl_LongKeyCode ZK_to_LK_arr[NUM_ZK][NUM_MOD_COMBO] = {
    // modifiable 
#define temp_map(LK_SUFFIX)                                             \
    [LK_##LK_SUFFIX][0] = LK_##LK_SUFFIX,                               \
    [LK_##LK_SUFFIX][ZGL_CTRL_MASK] = LK_C_##LK_SUFFIX,                  \
    [LK_##LK_SUFFIX][ZGL_ALT_MASK] = LK_M_##LK_SUFFIX,                   \
    [LK_##LK_SUFFIX][ZGL_SHIFT_MASK] = LK_S_##LK_SUFFIX,                 \
    [LK_##LK_SUFFIX][ZGL_CTRL_MASK | ZGL_ALT_MASK] = LK_C_M_##LK_SUFFIX,  \
    [LK_##LK_SUFFIX][ZGL_CTRL_MASK | ZGL_SHIFT_MASK] = LK_C_S_##LK_SUFFIX, \
    [LK_##LK_SUFFIX][ZGL_ALT_MASK | ZGL_SHIFT_MASK] = LK_M_S_##LK_SUFFIX, \
    [LK_##LK_SUFFIX][ZGL_CTRL_MASK | ZGL_ALT_MASK | ZGL_SHIFT_MASK] = LK_C_M_S_##LK_SUFFIX,

    temp_map(RET)
    temp_map(SPC)
    temp_map(TAB)
    temp_map(GRAVE)
    temp_map(MINUS)
    temp_map(EQUALS)
    temp_map(LEFTBRACKET)
    temp_map(RIGHTBRACKET)
    temp_map(BACKSLASH)
    temp_map(SEMICOLON)
    temp_map(APOSTROPHE)
    temp_map(COMMA)
    temp_map(PERIOD)
    temp_map(SLASH)
    temp_map(UP)
    temp_map(DOWN)
    temp_map(LEFT)
    temp_map(RIGHT)
    temp_map(0)
    temp_map(1)
    temp_map(2)
    temp_map(3)
    temp_map(4)
    temp_map(5)
    temp_map(6)
    temp_map(7)
    temp_map(8)
    temp_map(9)
    temp_map(a)
    temp_map(b)
    temp_map(c)
    temp_map(d)
    temp_map(e)
    temp_map(f)
    temp_map(g)
    temp_map(h)
    temp_map(i)
    temp_map(j)
    temp_map(k)
    temp_map(l)
    temp_map(m)
    temp_map(n)
    temp_map(o)
    temp_map(p)
    temp_map(q)
    temp_map(r)
    temp_map(s)
    temp_map(t)
    temp_map(u)
    temp_map(v)
    temp_map(w)
    temp_map(x)
    temp_map(y)
    temp_map(z)
#undef temp_map

    //unmodifiable
#define temp_map(LK_SUFFIX)                                             \
    [LK_##LK_SUFFIX][0] = LK_##LK_SUFFIX,                               \
    [LK_##LK_SUFFIX][ZGL_CTRL_MASK] = LK_##LK_SUFFIX,                    \
    [LK_##LK_SUFFIX][ZGL_ALT_MASK] = LK_##LK_SUFFIX,                     \
    [LK_##LK_SUFFIX][ZGL_SHIFT_MASK] = LK_##LK_SUFFIX,                   \
    [LK_##LK_SUFFIX][ZGL_CTRL_MASK | ZGL_ALT_MASK] = LK_##LK_SUFFIX,      \
    [LK_##LK_SUFFIX][ZGL_CTRL_MASK | ZGL_SHIFT_MASK] = LK_##LK_SUFFIX,    \
    [LK_##LK_SUFFIX][ZGL_ALT_MASK | ZGL_SHIFT_MASK] = LK_##LK_SUFFIX,     \
    [LK_##LK_SUFFIX][ZGL_CTRL_MASK | ZGL_ALT_MASK | ZGL_SHIFT_MASK] = LK_##LK_SUFFIX,
        
    temp_map(POINTER_LEFT)
    temp_map(POINTER_MIDDLE)
    temp_map(POINTER_RIGHT)
    temp_map(POINTER_WHEELUP)
    temp_map(POINTER_WHEELDOWN)
    temp_map(F1)
    temp_map(F2)
    temp_map(F3)
    temp_map(F4)
    temp_map(F5)
    temp_map(F6)
    temp_map(F7)
    temp_map(F8)
    temp_map(F9)
    temp_map(F10)
    temp_map(F11)
    temp_map(F12)

    temp_map(BACK)
    temp_map(ESC)
    temp_map(CAPS)
    temp_map(DEL)

    temp_map(LALT)
    temp_map(LCTRL)
    temp_map(LSHIFT)
    temp_map(RALT)
    temp_map(RCTRL)
    temp_map(RSHIFT)
#undef temp_map
};

zgl_LongKeyCode KeyComplex_to_LK(zgl_KeyComplex *KC) {
    return ZK_to_LK_arr[KC->zk][KC->mod_states];
}

zgl_LongKeyCode ZK_to_LK(zgl_KeyCode ZK) {
    return ZK_to_LK_arr[ZK][modifier_states];
}
