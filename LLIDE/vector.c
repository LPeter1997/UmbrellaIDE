#include "vector.h"

void vector_new(vector* vec, size_t init)
{
	vec->data = (uint32_t*)malloc(init * sizeof(uint32_t));
	vec->used = 0;
	vec->size = init;
}

void vector_add(vector* vec, uint32_t el)
{
	if (vec->used == vec->size)
	{
		vec->size *= 2;
		vec->data = (uint32_t*)realloc(vec->data, vec->size * sizeof(uint32_t));
	}
	vec->data[vec->used++] = el;
}

void vector_del(vector* vec)
{
	free(vec->data);
	vec->data = NULL;
	vec->used = vec->size = 0;
}