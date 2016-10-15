#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdlib.h>

typedef struct
{
	int* data;
	size_t used;
	size_t size;
} vector;

void vector_new(vector*, size_t);
void vector_insert(vector*, int);
void vector_add(vector*, int);
void vector_del(vector*);

#endif /* __VECTOR_H__ */