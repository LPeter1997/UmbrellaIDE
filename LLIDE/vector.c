#include "vector.h"

void vector_new(vector* vec, size_t init)
{
	vec->data = (int*)malloc(init * sizeof(int));
	vec->used = 0;
	vec->size = init;
}

void vector_insert(vector* vec, int el)
{
	if (vec->used == vec->size)
	{
		vec->size *= 2;
		vec->data = (int*)realloc(vec->data, vec->size * sizeof(int));
	}
	vec->data[vec->used++] = el;
}

inline void vector_add(vector* vec, int el)
{
	vector_insert(vec, vec->used);
}

void vector_del(vector* vec)
{
	free(vec->data);
	vec->data = NULL;
	vec->used = vec->size = 0;
}