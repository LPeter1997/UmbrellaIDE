#ifndef __GAP_BUFFER_H__
#define __GAP_BUFFER_H__

#include <stddef.h>

typedef struct
{
	char* data;
	size_t size;
	size_t beg_cur;
	size_t end_cur;
} gapbuf;

void gapbuf_new(gapbuf*, size_t);
void gapbuf_del(gapbuf*);
void gapbuf_add(gapbuf*, char);
void gapbuf_rem(gapbuf*);
void gapbuf_left(gapbuf*);
void gapbuf_right(gapbuf*);

#endif /* __GAP_BUFFER_H__ */