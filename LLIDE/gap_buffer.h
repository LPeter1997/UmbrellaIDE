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
void gapbuf_addn(gapbuf*, char, size_t);
void gapbuf_rem(gapbuf*);
void gapbuf_remn(gapbuf*, size_t);
void gapbuf_left(gapbuf*);
void gapbuf_right(gapbuf*);
void gapbuf_end(gapbuf*);
void gapbuf_transfer(gapbuf*, gapbuf*);
void gapbuf_moveend(gapbuf*, size_t, size_t);

#define gapbuf_after(x) ((x)->size - (x)->end_cur)
#define gapbuf_dataend(x) ((x)->data + (x)->size)
#define gapbuf_dataafter(x) ((x)->data + (x)->end_cur)
#define gapbuf_dataat(x) ((x)->data + (x)->beg_cur)
#define gapbuf_char(x) ((x)->data[(x)->beg_cur - 1])
#define gapbuf_charback(x, y) ((x)->data[(x)->beg_cur - 1 - (y)])

#endif /* __GAP_BUFFER_H__ */