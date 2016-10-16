#include "text_line.h"
#include "textbox.h"

void line_new(line* ln, line* prev, line* next)
{
	gapbuf_new(&ln->buffer, textbow_width());
	ln->len = 0;
	ln->next = next;
	ln->prev = prev;
	if (next != NULL)
	{
		next->prev = ln;
	}
	if (prev != NULL)
	{
		prev->next = ln;
	}
}

void line_del(line* ln)
{
	gapbuf_del(&ln->buffer);
	if (ln->prev != NULL)
	{
		ln->prev->next = ln->next;
	}
	if (ln->next != NULL)
	{
		ln->next->prev = ln->prev;
	}
	ln->len = 0;
}