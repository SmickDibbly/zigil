#include "src/Windows/zglwin_LK_from_VK.h"
#include <assert.h>
#include <stdio.h>

extern void init_LK_from_VK(void);
extern zgl_LongKeyCode LK_from_VK(WORD vk);
extern void shut_LK_from_VK(void);


/* HASH GENERALITIES */
/* overhead parameter that determines both space and search costs */
/* must be strictly greater than 1 */
#define OVERHEAD (1.1) // TODO: Use fixed point
#define NULL_ID (-1)

typedef struct ID_List {
    int size;
    int ids[1]; /* we'll actually malloc more space than this */
} *ID_List;

/* build an IDList out of an unsorted array of n good ids */
/* returns 0 on allocation failure */
static ID_List create_ID_List(int n, int unsorted_list[]);

/* destroy an IDList */
static void destroy_ID_List(ID_List list);

/* returns nonzero if VK is in the list */
static int contained_in_ID_List(ID_List list, WORD VK);

// This is ultimately the function which converts a VK to a value in a
// smaller range.
static int hash(ID_List list, WORD VK);

//static void print_ID_List(ID_List list);






/* APPLYING THE HASH */
static void update_LK_from_VKhash(void);

#define raw_hash(list, value) ((value) & ((list)->size - 1))

static int hash(ID_List list, WORD VK) {
    int probe;
    for (probe = (raw_hash(list, VK));
         list->ids[probe] != NULL_ID;
         probe = (probe + 1) & (list->size - 1)) {
        if (list->ids[probe] == VK) {
            return probe;
        }
    }
    return NULL_ID;
}

static ID_List create_ID_List(int list_len, int unsorted_id_list[]) {
    int size = (int)(list_len * OVERHEAD + 1);

    // "round up"; change size to the smallest power of 2 greater than or equal to original size
    for (int i = 1; ; i *= 2) {
        if (size <= i) {
            size = i;
            break;
        }
    }  

    ID_List list = malloc(sizeof(*list) + sizeof(int)*(size-1));
    if (list == NULL) return 0;

    list->size = size;
 
    /* clear the hash table */
    for(int i = 0; i < size; i++) {
        list->ids[i] = NULL_ID;
    }
 
    /* load it up */
    for(int i = 0; i < list_len; i++) {
        int probe;
        /* hashing with open addressing by division */
        /* this MUST be the same pattern as in IDListContains */
        for(probe = unsorted_id_list[i] & (list->size - 1);
            list->ids[probe] != NULL_ID;
            probe = (probe + 1) & (list->size - 1));
        
        assert(list->ids[probe] == NULL_ID);
 
        list->ids[probe] = unsorted_id_list[i];
    }
 
    return list;
}
 
static void destroy_ID_List(ID_List list) {
    free(list);
}

static int contained_in_ID_List(ID_List list, WORD VK) {
    int probe;
         
    /* this MUST be the same pattern as in IDListCreate */
    for(probe = VK & (list->size - 1);
        list->ids[probe] != NULL_ID;
        probe = (probe + 1) & (list->size - 1)) {
        if(list->ids[probe] == VK) {
            return 1;
        }
    }
 
    return 0;
}

/*
static void print_ID_List(ID_List list) {
    for (int i = 0; i < list->size; i++) {
        printf("[%i] = %s\n", i, VK_strs[list->ids[i]]);
    }
}
*/

#define VK_0 0x30
#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35
#define VK_6 0x36
#define VK_7 0x37
#define VK_8 0x38
#define VK_9 0x39
#define VK_a 0x41
#define VK_b 0x42
#define VK_c 0x43
#define VK_d 0x44
#define VK_e 0x45
#define VK_f 0x46
#define VK_g 0x47
#define VK_h 0x48
#define VK_i 0x49
#define VK_j 0x4A
#define VK_k 0x4B
#define VK_l 0x4C
#define VK_m 0x4D
#define VK_n 0x4E
#define VK_o 0x4F
#define VK_p 0x50
#define VK_q 0x51
#define VK_r 0x52
#define VK_s 0x53
#define VK_t 0x54
#define VK_u 0x55
#define VK_v 0x56
#define VK_w 0x57
#define VK_x 0x58
#define VK_y 0x59
#define VK_z 0x5A


#define SUPPORTED_VK                                    \
    Z(VK_LBUTTON)                                       \
         Z(VK_RBUTTON)                                  \
         Z(VK_MBUTTON)                                  \
         Z(VK_XBUTTON1)                                 \
         Z(VK_XBUTTON2)                                 \
                                                        \
         Z(VK_0)                                        \
         Z(VK_1)                                        \
         Z(VK_2)                                        \
    Z(VK_3)                                             \
    Z(VK_4)                                             \
    Z(VK_5)                                             \
    Z(VK_6)                                             \
    Z(VK_7)                                             \
    Z(VK_8)                                             \
    Z(VK_9)                                             \
                                                        \
    Z(VK_a)                                             \
    Z(VK_b)                                             \
    Z(VK_c)                                             \
    Z(VK_d)                                             \
         Z(VK_e)                                        \
         Z(VK_f)                                        \
         Z(VK_g)                                        \
         Z(VK_h)                                        \
         Z(VK_i)                                        \
         Z(VK_j)                                        \
         Z(VK_k)                                        \
         Z(VK_l)                                        \
    Z(VK_m)                                             \
    Z(VK_n)                                             \
    Z(VK_o)                                             \
    Z(VK_p)                                             \
    Z(VK_q)                                             \
    Z(VK_r)                                             \
    Z(VK_s)                                             \
    Z(VK_t)                                             \
    Z(VK_u)                                             \
    Z(VK_v)                                             \
    Z(VK_w)                                             \
    Z(VK_x)                                             \
         Z(VK_y)                                        \
         Z(VK_z)                                        \
                                                        \
         Z(VK_F1)                                       \
         Z(VK_F2)                                       \
         Z(VK_F3)                                       \
         Z(VK_F4)                                       \
         Z(VK_F5)                                       \
    Z(VK_F6)                                            \
    Z(VK_F7)                                            \
    Z(VK_F8)                                            \
    Z(VK_F9)                                            \
    Z(VK_F10)                                           \
    Z(VK_F11)                                           \
    Z(VK_F12)                                           \
                                                        \
    Z(VK_OEM_3)                                         \
    Z(VK_OEM_MINUS)                                     \
    Z(VK_OEM_PLUS)                                      \
                                                        \
         Z(VK_OEM_4)                                    \
         Z(VK_OEM_6)                                    \
         Z(VK_OEM_5)                                    \
         Z(VK_OEM_1)                                    \
         Z(VK_OEM_7)                                    \
         Z(VK_OEM_COMMA)                                \
         Z(VK_OEM_PERIOD)                               \
    Z(VK_OEM_2)                                         \
                                                        \
    Z(VK_SPACE)                                         \
                                                        \
    Z(VK_BACK)                                          \
    Z(VK_TAB)                                           \
    Z(VK_RETURN)                                        \
    Z(VK_ESCAPE)                                        \
    Z(VK_DELETE)                                        \
                                                        \
    Z(VK_CAPITAL)                                       \
         Z(VK_CONTROL)                                  \
         Z(VK_MENU)                                     \
    Z(VK_SHIFT)                                         \
                                                        \
         Z(VK_UP)                                       \
         Z(VK_DOWN)                                     \
         Z(VK_LEFT)                                     \
         Z(VK_RIGHT)                                    \

#define NUM_SUPPORTED_VK (82)

char *VK_strs[512] = {
#define Z(VK) [VK] = #VK,
    SUPPORTED_VK
#undef Z
};

ID_List list;

static int unsorted_list[NUM_SUPPORTED_VK] = {
#define Z(VK) VK,
    SUPPORTED_VK
#undef Z
};

void init_LK_from_VK(void) {
    list = create_ID_List(NUM_SUPPORTED_VK, unsorted_list);
    update_LK_from_VKhash();
}

void shut_LK_from_VK(void) {
    destroy_ID_List(list);
}

#define MOD_NONE 0
#define MOD_C 1
#define MOD_M 2
#define MOD_S 4

#define NUM_MODIFIER_COMBINATIONS 8
static zgl_LongKeyCode LK_from_VKhash[NUM_LK][NUM_MODIFIER_COMBINATIONS];

static void update_LK_from_VKhash(void) {
    // modifiable 
#define temp_map(VK_SUFFIX, LK_SUFFIX)                                  \
    LK_from_VKhash[hash(list, VK_##VK_SUFFIX)][MOD_NONE] = LK_##LK_SUFFIX; \
    LK_from_VKhash[hash(list, VK_##VK_SUFFIX)][MOD_C] = LK_C_##LK_SUFFIX; \
    LK_from_VKhash[hash(list, VK_##VK_SUFFIX)][MOD_M] = LK_M_##LK_SUFFIX; \
    LK_from_VKhash[hash(list, VK_##VK_SUFFIX)][MOD_S] = LK_S_##LK_SUFFIX; \
    LK_from_VKhash[hash(list, VK_##VK_SUFFIX)][MOD_C | MOD_M] = LK_C_M_##LK_SUFFIX; \
    LK_from_VKhash[hash(list, VK_##VK_SUFFIX)][MOD_C | MOD_S] = LK_C_S_##LK_SUFFIX; \
    LK_from_VKhash[hash(list, VK_##VK_SUFFIX)][MOD_M | MOD_S] = LK_M_S_##LK_SUFFIX; \
    LK_from_VKhash[hash(list, VK_##VK_SUFFIX)][MOD_C | MOD_M | MOD_S] = LK_C_M_S_##LK_SUFFIX

    temp_map(RETURN, RET);
    temp_map(SPACE, SPC);
    temp_map(TAB, TAB);
    temp_map(OEM_3, GRAVE);
    temp_map(OEM_MINUS, MINUS);
    temp_map(OEM_PLUS, EQUALS);
    temp_map(OEM_4, LEFTBRACKET);
    temp_map(OEM_6, RIGHTBRACKET);
    temp_map(OEM_5, BACKSLASH);
    temp_map(OEM_1, SEMICOLON);
    temp_map(OEM_7, APOSTROPHE);
    temp_map(OEM_COMMA, COMMA);
    temp_map(OEM_PERIOD, PERIOD);
    temp_map(OEM_2, SLASH);
    temp_map(UP, UP);
    temp_map(DOWN, DOWN);
    temp_map(LEFT, LEFT);
    temp_map(RIGHT, RIGHT);
    temp_map(0, 0);
    temp_map(1, 1);
    temp_map(2, 2);
    temp_map(3, 3);
    temp_map(4, 4);
    temp_map(5, 5);
    temp_map(6, 6);
    temp_map(7, 7);
    temp_map(8, 8);
    temp_map(9, 9);
    temp_map(a, a);
    temp_map(b, b);
    temp_map(c, c);
    temp_map(d, d);
    temp_map(e, e);
    temp_map(f, f);
    temp_map(g, g);
    temp_map(h, h);
    temp_map(i, i);
    temp_map(j, j);
    temp_map(k, k);
    temp_map(l, l);
    temp_map(m, m);
    temp_map(n, n);
    temp_map(o, o);
    temp_map(p, p);
    temp_map(q, q);
    temp_map(r, r);
    temp_map(s, s);
    temp_map(t, t);
    temp_map(u, u);
    temp_map(v, v);
    temp_map(w, w);
    temp_map(x, x);
    temp_map(y, y);
    temp_map(z, z);
#undef temp_map

    //unmodifiable
#define temp_map(VK_SUFFIX, LK_SUFFIX)                                  \
    LK_from_VKhash[hash(list, VK_##VK_SUFFIX)][MOD_NONE] = LK_##LK_SUFFIX;  \
    LK_from_VKhash[hash(list, VK_##VK_SUFFIX)][MOD_C] = LK_##LK_SUFFIX;     \
    LK_from_VKhash[hash(list, VK_##VK_SUFFIX)][MOD_M] = LK_##LK_SUFFIX;     \
    LK_from_VKhash[hash(list, VK_##VK_SUFFIX)][MOD_S] = LK_##LK_SUFFIX;     \
    LK_from_VKhash[hash(list, VK_##VK_SUFFIX)][MOD_C | MOD_M] = LK_##LK_SUFFIX; \
    LK_from_VKhash[hash(list, VK_##VK_SUFFIX)][MOD_C | MOD_S] = LK_##LK_SUFFIX; \
    LK_from_VKhash[hash(list, VK_##VK_SUFFIX)][MOD_M | MOD_S] = LK_##LK_SUFFIX; \
    LK_from_VKhash[hash(list, VK_##VK_SUFFIX)][MOD_C | MOD_M | MOD_S] = LK_##LK_SUFFIX
        
    temp_map(LBUTTON, POINTER_LEFT);
    temp_map(MBUTTON, POINTER_MIDDLE);
    temp_map(RBUTTON, POINTER_RIGHT);
    temp_map(XBUTTON1, POINTER_X1);
    temp_map(XBUTTON2, POINTER_X2);
    temp_map(F1, F1);
    temp_map(F2, F2);
    temp_map(F3, F3);
    temp_map(F4, F4);
    temp_map(F5, F5);
    temp_map(F6, F6);
    temp_map(F7, F7);
    temp_map(F8, F8);
    temp_map(F9, F9);
    temp_map(F10, F10);
    temp_map(F11, F11);
    temp_map(F12, F12);

    temp_map(BACK, BACK);
    temp_map(ESCAPE, ESC);
    temp_map(CAPITAL, CAPS);
    temp_map(DELETE, DEL);
#undef temp_map

    /*
#define Z(VK) printf(#VK " = %i -> %i -> %i\n", VK, raw_hash(list, VK), hash(list, VK));
    SUPPORTED_VK
#undef Z
    
        putchar('\n');
    print_ID_List(list);
    */
}

static int is_supported_VK(WORD vk) {
    if (contained_in_ID_List(list, vk)) {
        return 1;
    }
    return 0;
}

#define PRESSED 0x8000

zgl_LongKeyCode LK_from_VK(WORD vk) {
    if ( ! is_supported_VK(vk)) {
        return LK_NONE;
    }

    uint32_t mod = 0;
    if (GetKeyState(VK_LCONTROL) & PRESSED) mod |= MOD_C;
    if (GetKeyState(VK_LMENU)    & PRESSED) mod |= MOD_M;
    if (GetKeyState(VK_LSHIFT)   & PRESSED) mod |= MOD_S;

    return LK_from_VKhash[hash(list, vk)][mod];
}
