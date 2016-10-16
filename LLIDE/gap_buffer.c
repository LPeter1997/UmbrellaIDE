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

void gapbuf_addn(gapbuf* buf, char c, size_t len)
{
	while (len--)
	{
		buf->data[buf->beg_cur++] = c;
	}
}

void gapbuf_rem(gapbuf* buf)
{
	buf->beg_cur--;
}

void gapbuf_remn(gapbuf* buf, size_t len)
{
	buf->beg_cur -= len;
}

void gapbuf_left(gapbuf* buf)
{
	buf->data[--buf->end_cur] = buf->data[--buf->beg_cur];
}

void gapbuf_right(gapbuf* buf)
{
	buf->data[buf->beg_cur++] = buf->data[buf->end_cur++];
}

void gapbuf_end(gapbuf* buf)
{
	memcpy(buf->data + buf->beg_cur,
		buf->data + buf->end_cur,
		buf->size - buf->end_cur);

	buf->beg_cur += buf->size - buf->end_cur;
	buf->end_cur = buf->size;
}

void gapbuf_transfer(gapbuf* from, gapbuf* to)
{
	char* nexts = gapbuf_dataafter(from);
	size_t rem = gapbuf_after(from);

	memcpy(gapbuf_dataend(to) - rem, nexts, rem);
	memset(nexts, ' ', rem);
	to->end_cur -= rem;
	from->end_cur = from->size;
}

void gapbuf_moveend(gapbuf* buf, size_t off, size_t len)
{
	memcpy(gapbuf_dataend(buf) - len, buf->data + off, len);
}