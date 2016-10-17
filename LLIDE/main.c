#include <stdio.h>
#include <windows.h>
#include "console.h"
#include "menu_bar.h"
#include "textbox.h"
#include "vector.h"
#include "status_bar.h"
#include "regexp.h"

int main(void)
{
	int i;
	char* post;
	char buff[256];
	regexp_state* start;

	regexp_init();
	scanf("%s", &buff);
	buff[strlen(buff)] = 0;
	post = regexp_infx2postfx(buff);
	start = regexp_postfx2nfa(post);

	for (;;)
	{
		scanf("%s", &buff);
		buff[strlen(buff)] = 0;
		printf("%s\n", regexp_match(dstate_start(start), buff) ? "true" : "false");
	}

#if 0
	console_init();
	statusbar_init();

	menu_draw();
	statusbar_draw();

	textbox_init();
	textbox_activate();
#endif

	getchar();
	getchar();
	return 0;
}