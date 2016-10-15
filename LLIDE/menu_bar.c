#include "menu_bar.h"
#include "console.h"

static const char* options[] =
{
	"File",
	"Edit"
};

void menu_draw(void)
{
	size_t i;
	size_t done = 0;
	console_col(console_mix(Col_White, Col_Magenta));
	console_pos(0, 0);
	for (i = 0; i < sizeof(options) / sizeof(options[0]); i++)
	{
		done += console_write(options[i]) + 1;
		console_writec(' ');
	}
	console_repeat(' ', console_width() - done);
}