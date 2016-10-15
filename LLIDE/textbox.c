#include <stdint.h>
#include <windows.h>
#include <conio.h>
#include "textbox.h"
#include "console.h"

#define ARROW_KEY 224

#define VK_UP	 72
#define VK_DOWN  80
#define VK_LEFT  75
#define VK_RIGHT 77

static size_t box_x = 2;
static size_t box_y = 1;

static size_t pos_x = 0;
static size_t pos_y = 0;

static size_t width;
static size_t height;

static void textbox_arrow_key(char);

void textbox_init(void)
{
	width = console_width() - 2;
	height = console_height() - 1;
}

void textbox_draw(void)
{
	size_t i;
	char buff[8];
	for (i = 1; i < console_height(); i++)
	{
		itoa(i, buff, 10);
		console_col(console_mix(Col_Black, Col_LightGrey));
		console_writen(buff, 2);
		console_col(console_mix(Col_White, Col_Black));
		console_repeat(' ', console_width() - 2);
	}
}

void textbox_activate(void)
{
	int key;
	for (;;)
	{
		while (!kbhit());
		key = getch();

		if (key == ARROW_KEY)
		{
			textbox_arrow_key(getch());
		}
	}
}

static void textbox_arrow_key(char wh)
{
	switch (wh)
	{
	case VK_UP:
		if (pos_y > 0)
			pos_y--;
		break;

	case VK_DOWN:
		if (pos_y < height)
			pos_y++;
		break;

	case VK_LEFT:
		if (pos_x > 0)
			pos_x--;
		break;

	case VK_RIGHT:
		if (pos_x < width)
			pos_x++;
		break;
	}

	console_pos(box_x + pos_x, box_y + pos_y);
}