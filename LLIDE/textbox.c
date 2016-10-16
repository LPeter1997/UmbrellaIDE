#include <stdint.h>
#include <windows.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include "textbox.h"
#include "console.h"
#include "text_line.h"

#define ARROW_KEY 224
#define BACKSPACE_KEY 8
#define ENTER_KEY 13

#define VK_UP	 72
#define VK_DOWN  80
#define VK_LEFT  75
#define VK_RIGHT 77

static size_t box_x = 0;
static size_t box_y = 1;

static size_t pos_x = 0;
static size_t pos_y = 0;

static size_t wpos_x = 0;

static size_t width;
static size_t height;

static line* first_line;
static line* current_line;
static gapbuf* curr_buff;

static void textbox_arrow_key(char);
static void textbox_new_line(void);
static void textbox_backspace(void);
static void textbox_redraw_down(void);
static void textbox_redraw_up(void);

void textbox_init(void)
{
	width = console_width() - box_x;
	height = console_height() - box_y;

	first_line = (line*)malloc(sizeof(line));
	line_new(first_line, NULL, NULL);
	current_line = first_line;
	curr_buff = &current_line->buffer;
}

void textbox_activate(void)
{
	console_col(console_mix(Col_White, Col_Black));

	console_pos(0, 0);
	console_pos(box_x, box_y);

	int key;
	for (;;)
	{
		while (!kbhit());
		key = getch();

		if (key == ARROW_KEY)
		{
			textbox_arrow_key(getch());
		}
		else if (key == ENTER_KEY)
		{
			textbox_new_line();
		}
		else if (key == BACKSPACE_KEY)
		{
			textbox_backspace();
		}
		else if (isprint(key))
		{
			console_writec((char)key);
			gapbuf_add(curr_buff, (char)key);
			wpos_x = ++pos_x;
			console_pos(box_x + pos_x, box_y + pos_y);
			console_writen(curr_buff->data + curr_buff->end_cur,
				curr_buff->size - curr_buff->end_cur);
			console_pos(box_x + pos_x, box_y + pos_y);
			current_line->len++;
		}
		else
		{
			printf("%d", key);
			for (; ; );
		}
	}
}

static void textbox_arrow_key(char wh)
{
	switch (wh)
	{
	case VK_UP:
		if (current_line->prev != NULL)
		{
			pos_y--;
			current_line = current_line->prev;
			curr_buff = &current_line->buffer;
			if (current_line->len <= wpos_x)
			{
				pos_x = current_line->len;
				curr_buff->end_cur = curr_buff->size;
				curr_buff->beg_cur = current_line->len;
			}
			else
			{
				// Just copy everything to front
				memcpy(curr_buff->data + curr_buff->beg_cur,
					curr_buff->data + curr_buff->end_cur,
					curr_buff->size - curr_buff->end_cur);

				size_t len = current_line->len - wpos_x;
				curr_buff->beg_cur = wpos_x;
				curr_buff->end_cur = curr_buff->size - len;
				// Move rest
				memcpy(curr_buff->data + curr_buff->size - len,
					curr_buff->data + wpos_x, len);

				pos_x = wpos_x;
			}
		}
		break;

	case VK_DOWN:
		if (current_line->next != NULL)
		{
			pos_y++;
			current_line = current_line->next;
			curr_buff = &current_line->buffer;
			if (current_line->len <= wpos_x)
			{
				pos_x = current_line->len;
				curr_buff->end_cur = curr_buff->size;
				curr_buff->beg_cur = current_line->len;
			}
			else
			{
				// Just copy everything to front
				memcpy(curr_buff->data + curr_buff->beg_cur,
					curr_buff->data + curr_buff->end_cur,
					curr_buff->size - curr_buff->end_cur);

				size_t len = current_line->len - wpos_x;
				curr_buff->beg_cur = wpos_x;
				curr_buff->end_cur = curr_buff->size - len;
				// Move rest
				memcpy(curr_buff->data + curr_buff->size - len,
					curr_buff->data + wpos_x, len);

				pos_x = wpos_x;
			}
		}
		break;

	case VK_LEFT:
		if (pos_x > 0)
		{
			wpos_x = --pos_x;
			gapbuf_left(curr_buff);
		}
		break;

	case VK_RIGHT:
		if (pos_x < current_line->len)
		{
			wpos_x = ++pos_x;
			gapbuf_right(curr_buff);
		}
		break;
	}

	console_pos(box_x + pos_x, box_y + pos_y);
}

static void textbox_new_line(void)
{
	// Create new line
	line* ln = (line*)malloc(sizeof(line));
	line_new(ln, current_line, current_line->next);

	// Cut the end of current buffer
	char* nexts = curr_buff->data + curr_buff->end_cur;
	size_t nextl = curr_buff->size - curr_buff->end_cur;

	// Copy to next line
	memcpy(ln->buffer.data + ln->buffer.size - nextl,
		nexts, nextl);
	memset(nexts, ' ', nextl);
	ln->buffer.end_cur -= nextl;
	current_line->len -= nextl;
	curr_buff->end_cur = curr_buff->size;
	current_line = ln;
	curr_buff = &ln->buffer;
	console_repeat(' ', nextl);
	pos_y++;
	pos_x = 0;
	current_line->len = nextl;
	console_pos(box_x + pos_x, box_y + pos_y);
	console_writen(curr_buff->data + curr_buff->end_cur, nextl);
	console_pos(box_x + pos_x, box_y + pos_y);
	textbox_redraw_down();
}

static void textbox_backspace(void)
{
	if (pos_x > 0)
	{
		wpos_x = --pos_x;
		gapbuf_rem(curr_buff);
		console_pos(box_x + pos_x, box_y + pos_y);
		console_writen(curr_buff->data + curr_buff->end_cur,
			curr_buff->size - curr_buff->end_cur);

		console_repeat(' ', width - pos_x);
		console_pos(box_x + pos_x, box_y + pos_y);
		current_line->len--;
	}
	else
	{
		line* prev = current_line->prev;
		if (prev != NULL)
		{
			memcpy(prev->buffer.data + prev->buffer.size - current_line->len,
				current_line->buffer.data + current_line->buffer.end_cur,
				current_line->len);
			console_pos(box_x, box_y + pos_y);
			console_repeat(' ', current_line->len);
			wpos_x = pos_x = prev->len;
			pos_y--;
			prev->buffer.beg_cur = prev->len;
			prev->buffer.end_cur = prev->buffer.size - current_line->len;
			prev->len += current_line->len;
			current_line = current_line->prev;
			curr_buff = &current_line->buffer;
			line_del(current_line->next);
			console_pos(box_x + pos_x, box_y + pos_y);
			console_writen(curr_buff->data + curr_buff->end_cur,
				curr_buff->size - curr_buff->end_cur);
			console_pos(box_x + pos_x, box_y + pos_y);
			textbox_redraw_up();
		}
	}
}

size_t textbow_width(void)
{
	return width;
}

static void textbox_redraw_up(void)
{
	size_t nextsz = 0;
	size_t orig_y = pos_y;
	line* ln = current_line->next;
	while (ln != NULL)
	{
		pos_y++;
		console_pos(box_x, box_y + pos_y);
		console_writen(ln->buffer.data, ln->buffer.beg_cur);
		console_writen(ln->buffer.data + ln->buffer.end_cur,
			ln->buffer.size - ln->buffer.end_cur);
		nextsz = ln->len;
		if (ln->next != NULL)
		{
			if (ln->next->len < ln->len)
			{
				console_repeat(' ', ln->len - ln->next->len);
			}
		}
		ln = ln->next;
	}
	pos_y++;
	console_pos(box_x, box_y + pos_y);
	console_repeat(' ', nextsz);
	pos_y = orig_y;
	console_pos(box_x + pos_x, box_y + pos_y);
}

static void textbox_redraw_down(void)
{
	size_t lastsz = 0;
	size_t orig_y = pos_y;
	line* ln = current_line->next;
	while (ln != NULL)
	{
		if (ln->prev != NULL)
		{
			if (ln->prev->len < ln->len)
			{
				console_pos(box_x + ln->prev->len, box_y + pos_y);
				console_repeat(' ', ln->len - ln->prev->len);
			}
		}
		pos_y++;
		console_pos(box_x, box_y + pos_y);
		console_writen(ln->buffer.data, ln->buffer.beg_cur);
		console_writen(ln->buffer.data + ln->buffer.end_cur,
			ln->buffer.size - ln->buffer.end_cur);
		ln = ln->next;
	}
	pos_y = orig_y;
	console_pos(box_x + pos_x, box_y + pos_y);
}