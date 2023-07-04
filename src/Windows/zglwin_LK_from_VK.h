#ifndef ZGLWIN_LK_FROM_MSG_H
#define ZGLWIN_LK_FROM_MSG_H

#include "src/Windows/zglwin.h"
#include "src/zigil_input.h"

extern void init_LK_from_VK(void);
extern zgl_LongKeyCode LK_from_VK(WORD vk);
extern void shut_LK_from_VK(void);

#endif
