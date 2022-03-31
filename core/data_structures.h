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
#define array_size(array) ((array) ? array_header(array)->size : 0) 
#define array_capacity(array) ((array) ? array_header(array)->capacity : 0)

#define array_may_grow(array, count) ((!(array) || (array_size(array) + (count)) > array_capacity(array)) ? ((array) = array_grow_wrapper((array), sizeof(*(array))), 0) : 0)
#define array_push(array, item) (array_may_grow(array, 1), (array)[array_header(array)->size++] = (item))
#define array_pop(array) (array_header(array)->size--, (array)[array_header(array)->size]) 
#define array_clear(array) ((array) ? array_header(array)->size = 0 : 0)
#define array_free(array) ((array) ? free((void *)array_header(array)) : 0, array = 0)

template <typename T>
static T *array_grow_wrapper(T *array, size_t element_size)
{
    return (T *)array_grow((void *)array, element_size);
}

void *array_grow(void *array, size_t element_size)
{
    ArrayHeader debug = {};
    (void)debug;

    void *base = 0; 
    u32 capacity = array_capacity(array) * 2;

    if(capacity < 4)
    {
        capacity = 4; 
    }
    base = realloc((array) ? (void *)array_header(array) : 0, capacity * element_size + sizeof(ArrayHeader));
    base = (u8 *)base + sizeof(ArrayHeader);
    
    array_header(base)->capacity = capacity;
    if(!array)
    {
        array_header(base)->size = 0;
    }
    return base;
}

#endif // DATA_STRUCTURES_H
