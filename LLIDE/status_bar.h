#ifndef __STATUS_BAR_H__
#define __STATUS_BAR_H__

#include <stddef.h>

void statusbar_init(void);
void statusbar_draw(void);
void statusbar_update(size_t, size_t);

#endif