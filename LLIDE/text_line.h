#ifndef __TEXT_LINE_H__
#define __TEXT_LINE_H__

#include "gap_buffer.h"

typedef struct __line
{
	gapbuf buffer;
	size_t len;
	struct __line* prev;
	struct __line* next;
} line;

void line_new(line*, line*, line*);
void line_del(line*);

#endif /* __TEXT_LINE_H__ */