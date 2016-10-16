#include <string.h>
#include <windows.h>
#include "console.h"

static HANDLE console;
static CONSOLE_SCREEN_BUFFER_INFO buffer_info;
static size_t width;
static size_t height;

void console_init(void)
{
	console = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console, &buffer_info);
	width = buffer_info.srWindow.Right - buffer_info.srWindow.Left + 1;
	height = buffer_info.srWindow.Bottom - buffer_info.srWindow.Top + 1;
}

void console_pos(size_t x, size_t y)
{
	COORD c = { x, y };
	SetConsoleCursorPosition(console, c);
}

void console_col(uint8_t c)
{
	SetConsoleTextAttribute(console, c);
}

uint8_t console_mix(Color fg, Color bg)
{
	return ((uint8_t)bg << 4) | (uint8_t)fg;
}

size_t console_width(void)
{
	return width;
}

size_t console_height(void)
{
	return height;
}

void console_putc(size_t x, size_t y, char ch)
{
	console_pos(x, y);
	putchar(ch);
}

void console_putn(size_t x, size_t y, char* str, size_t len)
{
	int i;
	console_pos(x, y);
	WriteConsoleA(console, str, len, &i, NULL);
}

size_t console_put(size_t x, size_t y, char* str)
{
	size_t i;
	size_t len = strlen(str);
	console_pos(x, y);
	WriteConsoleA(console, str, len, &i, NULL);
	return i;
}

void console_repeat_at(size_t x, size_t y, char ch, size_t cnt)
{
	console_pos(x, y);
	while (cnt--)
		putchar(ch);
}

void console_writec(char ch)
{
	putchar(ch);
}

void console_writen(char* str, size_t len)
{
	int i;
	WriteConsoleA(console, str, len, &i, NULL);
}

size_t console_write(char* str)
{
	size_t i;
	size_t len = strlen(str);
	WriteConsoleA(console, str, len, &i, NULL);
	return i;
}

void console_repeat(char ch, size_t cnt)
{
	while (cnt--)
		putchar(ch);
}

size_t console_x(void)
{
	GetConsoleScreenBufferInfo(console, &buffer_info);
	return buffer_info.dwCursorPosition.X;
}

size_t console_y(void)
{
	GetConsoleScreenBufferInfo(console, &buffer_info);
	return buffer_info.dwCursorPosition.Y;
}