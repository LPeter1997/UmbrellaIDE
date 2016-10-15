#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <stdint.h>

typedef enum
{
	Col_Black		 = 0,
	Col_Blue		 = 1,
	Col_Green		 = 2,
	Col_Cyan		 = 3,
	Col_Red			 = 4,
	Col_Magenta		 = 5,
	Col_Brown		 = 6,
	Col_LightGrey	 = 7,
	Col_DarkGrey	 = 8,
	Col_LightBlue	 = 9,
	Col_LightGreen	 = 10,
	Col_LightCyan	 = 11,
	Col_LightRed	 = 12,
	Col_LightMagenta = 13,
	Col_Yellow		 = 14,
	Col_White		 = 15,
} Color;

void console_init(void);
void console_pos(size_t, size_t);
void console_col(uint8_t);
uint8_t console_mix(Color, Color);
size_t console_width(void);
size_t console_height(void);

void console_putc(size_t, size_t, char);
void console_putn(size_t, size_t, char*, size_t);
size_t console_put(size_t, size_t, char*);
void console_repeat_at(size_t, size_t, char, size_t);

void console_writec(char);
void console_writen(char*, size_t);
size_t console_write(char*);
void console_repeat(char, size_t);

#endif /* __CONSOLE_H__ */