#include <stdio.h>
#include <windows.h>
#include "console.h"
#include "menu_bar.h"
#include "textbox.h"

int main(void)
{
	int i = 0;
	console_init();
	menu_draw();
	textbox_init();
	textbox_draw();
	console_pos(2, 0);
	console_pos(2, 1);
	textbox_activate();

	getchar();
	return 0;
}