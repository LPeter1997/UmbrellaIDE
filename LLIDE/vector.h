#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdlib.h>
#include <stdint.h>

typedef struct
{
	uint32_t* data;
	size_t used;
	size_t size;
} vector;

void vector_new(vector*, size_t);
void vector_add(vector*, uint32_t);
void vector_del(vector*);

#endif /* __VECTOR_H__ */