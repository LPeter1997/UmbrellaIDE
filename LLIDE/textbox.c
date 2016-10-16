#include <stdint.h>
#include <windows.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include "textbox.h"
#include "console.h"
#include "text_line.h"
#include "position.h"
#include "status_bar.h"

#define ARROW_KEY 224
#define BACKSPACE_KEY 8
#define ENTER_KEY 13
#define TAB_KEY 9

#define VK_UP	 72
#define VK_DOWN  80
#define VK_LEFT  75
#define VK_RIGHT 77

#define cur_repos() (console_pos(box_pos.x + cur_pos.x, box_pos.y + cur_pos.y))
#define clear(x) (console_repeat(' ', x))

static pos box_pos = { 0, 1 };
static pos cur_pos = { 0, 0 };
static size_t wpos_x = 0;

static pos size = { 0, 0 };

static line* first_line;
static line* current_line;
static gapbuf* curr_buff;

static void textbox_arrow_key(char);
static void textbox_new_line(void);
static void textbox_backspace(void);
static void textbox_print(char);
static void textbox_tab(void);
static void textbox_redraw_down(void);
static void textbox_redraw_up(void);
static void textbox_line(line*);

void textbox_init(void)
{
	size.x = console_width() - box_pos.x;
	size.y = console_height() - box_pos.y;

	first_line = (line*)malloc(sizeof(line));
	line_new(first_line, NULL, NULL);
	textbox_line(first_line);
}

void textbox_activate(void)
{
	console_col(console_mix(Col_White, Col_Black));

	console_pos(0, 0);
	console_pos(box_pos.x, box_pos.y);
	statusbar_update(cur_pos.x, cur_pos.y);
	cur_repos();

	int key;
	for (;;)
	{
		while (!kbhit());
		key = getch();

		if (key == ARROW_KEY)
		{
			textbox_arrow_key(getch());
			statusbar_update(cur_pos.x, cur_pos.y);
			cur_repos();
		}
		else if (key == TAB_KEY)
		{
			textbox_tab();
			statusbar_update(cur_pos.x, cur_pos.y);
			cur_repos();
		}
		else if (key == ENTER_KEY)
		{
			textbox_new_line();
			statusbar_update(cur_pos.x, cur_pos.y);
			cur_repos();
		}
		else if (key == BACKSPACE_KEY)
		{
			textbox_backspace();
			statusbar_update(cur_pos.x, cur_pos.y);
			cur_repos();
		}
		else if (isprint(key))
		{
			textbox_print((char)key);
			statusbar_update(cur_pos.x, cur_pos.y);
			cur_repos();
		}
		else
		{
			printf("%d", key);
			for (; ; );
		}
	}
}

size_t textbow_width(void)
{
	return size.x;
}

static void textbox_arrow_key(char wh)
{
	switch (wh)
	{
	case VK_UP:
	case VK_DOWN:;
		line* nline = NULL;
		int add = 0;
		if (wh == VK_DOWN)
		{
			nline = current_line->next;
			add = 1;
		}
		else
		{
			nline = current_line->prev;
			add = -1;
		}
		if (nline != NULL)
		{
			cur_pos.y += add;
			textbox_line(nline);
			if (current_line->len <= wpos_x)
			{
				cur_pos.x = current_line->len;
				curr_buff->end_cur = curr_buff->size;
				curr_buff->beg_cur = current_line->len;
			}
			else
			{
				// Just copy everything to front
				gapbuf_end(curr_buff);

				size_t len = current_line->len - wpos_x;
				curr_buff->beg_cur = wpos_x;
				curr_buff->end_cur = curr_buff->size - len;
				// Move rest
				gapbuf_moveend(curr_buff, wpos_x, len);
				cur_pos.x = wpos_x;
			}
		}
		break;

	case VK_LEFT:
		if (cur_pos.x > 0)
		{
			wpos_x = --cur_pos.x;
			gapbuf_left(curr_buff);
		}
		else if (current_line->prev != NULL)
		{
			// Just copy everything to front
			textbox_line(current_line->prev);
			gapbuf_end(curr_buff);

			size_t len = current_line->len;
			cur_pos.x = len;
			cur_pos.y--;
			wpos_x = len;
		}
		break;

	case VK_RIGHT:
		if (cur_pos.x < current_line->len)
		{
			wpos_x = ++cur_pos.x;
			gapbuf_right(curr_buff);
		}
		else if (current_line->next != NULL)
		{
			// Just copy everything to front
			textbox_line(current_line->next);
			size_t len = current_line->len;
			curr_buff->beg_cur = 0;
			curr_buff->end_cur = curr_buff->size - len;
			gapbuf_moveend(curr_buff, 0, len);
			cur_pos.x = 0;
			cur_pos.y++;
			wpos_x = 0;
		}
		break;
	}

	cur_repos();
}

static void textbox_new_line(void)
{
	// Create new line
	line* ln = (line*)malloc(sizeof(line));
	line_new(ln, current_line, current_line->next);

	// Copy to next line
	size_t nextl = gapbuf_after(curr_buff);
	gapbuf_transfer(current_line, ln);
	current_line->len -= nextl;
	textbox_line(ln);
	// Crear after
	clear(nextl);
	cur_pos.y++;
	cur_pos.x = 0;
	wpos_x = 0;
	current_line->len = nextl;
	cur_repos();
	console_writen(gapbuf_dataafter(curr_buff), nextl);
	cur_repos();
	textbox_redraw_down();
}

static void textbox_backspace(void)
{
	if (cur_pos.x > 0)
	{
		if (gapbuf_char(curr_buff) == '\t')
		{
			size_t cnt = 0;
			while (gapbuf_charback(curr_buff, cnt) == '\t' && ++cnt < 4);
			gapbuf_remn(curr_buff, cnt);
			cur_pos.x -= cnt;
			wpos_x = cur_pos.x;
			cur_repos();
			console_writen(gapbuf_dataafter(curr_buff), gapbuf_after(curr_buff));
			clear(current_line->len - cur_pos.x);
			cur_repos();
			current_line->len -= cnt;
		}
		else
		{
			wpos_x = --cur_pos.x;
			gapbuf_rem(curr_buff);
			cur_repos();
			console_writen(gapbuf_dataafter(curr_buff), gapbuf_after(curr_buff));

			clear(current_line->len - cur_pos.x);
			cur_repos();
			current_line->len--;
		}
	}
	else
	{
		line* prev = current_line->prev;
		if (prev != NULL)
		{
			gapbuf_transfer(curr_buff, &prev->buffer);
			clear(current_line->len);
			wpos_x = cur_pos.x = prev->len;
			cur_pos.y--;
			prev->buffer.beg_cur = prev->len;
			prev->buffer.end_cur = prev->buffer.size - current_line->len;
			prev->len += current_line->len;
			textbox_line(current_line->prev);
			line_del(current_line->next);
			cur_repos();
			console_writen(gapbuf_dataafter(curr_buff), gapbuf_after(curr_buff));
			cur_repos();
			textbox_redraw_up();
			wpos_x = cur_pos.x;
		}
	}
}

static void textbox_print(char ch)
{
	console_writec((char)ch);
	gapbuf_add(curr_buff, (char)ch);
	wpos_x = ++cur_pos.x;
	cur_repos();
	console_writen(gapbuf_dataafter(curr_buff), gapbuf_after(curr_buff));
	cur_repos();
	current_line->len++;
}

static void textbox_tab(void)
{
	size_t off = 4 - (cur_pos.x % 4);
	console_repeat('\t', off);
	gapbuf_addn(curr_buff, '\t', off);
	cur_pos.x += off;
	wpos_x = cur_pos.x;
	cur_repos();
	console_writen(gapbuf_dataafter(curr_buff), gapbuf_after(curr_buff));
	cur_repos();
	current_line->len += off;
}

static void textbox_redraw_up(void)
{
	size_t nextsz = 0;
	size_t orig_y = cur_pos.y;
	line* ln = current_line->next;
	while (ln != NULL)
	{
		cur_pos.y++;
		console_pos(box_pos.x, box_pos.y + cur_pos.y);
		console_writen(ln->buffer.data, ln->buffer.beg_cur);
		console_writen(gapbuf_dataafter(&ln->buffer), gapbuf_after(&ln->buffer));
		nextsz = ln->len;
		if (ln->next != NULL)
		{
			if (ln->next->len < ln->len)
			{
				clear(ln->len - ln->next->len);
			}
		}
		ln = ln->next;
	}
	cur_pos.y++;
	console_pos(box_pos.x, box_pos.y + cur_pos.y);
	clear(nextsz);
	cur_pos.y = orig_y;
	cur_repos();
}

static void textbox_redraw_down(void)
{
	size_t lastsz = 0;
	size_t orig_y = cur_pos.y;
	line* ln = current_line->next;
	while (ln != NULL)
	{
		if (ln->prev != NULL)
		{
			if (ln->prev->len < ln->len)
			{
				console_pos(box_pos.x + ln->prev->len, box_pos.y + cur_pos.y);
				clear(ln->len - ln->prev->len);
			}
		}
		cur_pos.y++;
		console_pos(box_pos.x, box_pos.y + cur_pos.y);
		console_writen(ln->buffer.data, ln->buffer.beg_cur);
		console_writen(gapbuf_dataafter(&ln->buffer), gapbuf_after(&ln->buffer));
		ln = ln->next;
	}
	cur_pos.y = orig_y;
	cur_repos();
}

static void textbox_line(line* ln)
{
	current_line = ln;
	curr_buff = &ln->buffer;
}