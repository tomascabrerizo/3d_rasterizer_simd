#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include "types.h"

// TODO: program customs allocators
#include <stdlib.h>

struct ArrayHeader 
{
    u32 capacity;
    u32 size;
};

#define array_header(array) ((ArrayHeader *)(array) - 1)
#define array_size(array) array_header(array)->size
#define array_capacity(array) array_header(array)->capacity
#define array_push(array, item) do{ array_may_grow(array, sizeof(*array)); (array)[array_header(array)->size++] = (item); }while(0)
#define array_pop(array)
#define array_clear(array) (array_header(array)->size = 0)

#define array_free(array) (free((u8 *)array - sizeof(ArrayHeader)), array = 0;)

inline void array_grow(void *array, size_t element_size)
{
    if(!array)
    {
        u32 capacity = 4;
        array = realloc(array, capacity * element_size + sizeof(ArrayHeader));
        array_header(array)->capacity = capacity;
        array_header(array)->size = 0;
    }
    else
    {
        u32 new_capacity = array_header(array)->capacity * 2;
        array = realloc((u8 *)array - sizeof(ArrayHeader), new_capacity * element_size + sizeof(ArrayHeader));
        array_header(array)->capacity = new_capacity;
    }
}

inline void array_may_grow(void *array, size_t element_size)
{
    if(!array || (array_size(array) + element_size) >= array_capacity(array))
    {
        array_grow(array, element_size);
    }
}

#endif // DATA_STRUCTURES_H
