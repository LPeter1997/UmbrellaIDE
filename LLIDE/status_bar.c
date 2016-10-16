#include <stdlib.h>
#include "status_bar.h"
#include "console.h"

static size_t width;
static size_t pos_y;
static char numbuff[128] = { 0 };

#define DISTANCE 12
#define COLPOS (width - 20)
#define LNPOS (COLPOS - DISTANCE)

void statusbar_init(void)
{
	width = console_width();
	pos_y = console_height() - 1;
}

void statusbar_draw(void)
{
	console_pos(0, pos_y);
	console_col(console_mix(Col_Black, Col_LightGrey));
	console_repeat(' ', width);
	console_pos(LNPOS - 3, pos_y);
	console_writen("Ln", 2);
	console_pos(COLPOS - 4, pos_y);
	console_writen("Col", 3);
}

void statusbar_update(size_t col, size_t row)
{
	console_col(console_mix(Col_Black, Col_LightGrey));

	itoa(row + 1, numbuff, 10);
	console_pos(LNPOS, pos_y);
	console_write(numbuff);
	console_repeat(' ', 6);

	itoa(col + 1, numbuff, 10);
	console_pos(COLPOS, pos_y);
	console_write(numbuff);
	console_repeat(' ', 6);

	console_col(console_mix(Col_White, Col_Black));
}