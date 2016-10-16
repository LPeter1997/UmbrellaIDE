#include <string.h>
#include "gap_buffer.h"

void gapbuf_new(gapbuf* buf, size_t init)
{
	buf->data = (char*)malloc(init * sizeof(char));
	memset(buf->data, ' ', init * sizeof(char));
	buf->size = init;
	buf->beg_cur = 0;
	buf->end_cur = init;
}

void gapbuf_del(gapbuf* buf)
{
	free(buf->data);
	buf->data = NULL;
	buf->beg_cur = buf->end_cur = buf->size = 0;
}

void gapbuf_add(gapbuf* buf, char c)
{
	buf->data[buf->beg_cur++] = c;
}

void gapbuf_rem(gapbuf* buf)
{
	buf->beg_cur--;
}

void gapbuf_left(gapbuf* buf)
{
	buf->data[--buf->end_cur] = buf->data[--buf->beg_cur];
}

void gapbuf_right(gapbuf* buf)
{
	buf->data[buf->beg_cur++] = buf->data[buf->end_cur++];
}