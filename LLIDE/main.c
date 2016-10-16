#include <stdio.h>
#include <windows.h>
#include "console.h"
#include "menu_bar.h"
#include "textbox.h"
#include "vector.h"

int main(void)
{
	int i;

	console_init();

	menu_draw();

	textbox_init();
	textbox_activate();

	getchar();
	return 0;
}