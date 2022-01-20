#include "internals.h"

// TODO: look into mremap for large allocations

/*
void	*realloc(void *ptr, size_t size)
{
    block_t	*block;
    void	*ret;

    block = ptr - sizeof(block_t);
    if (block->size >= size)
        return (ptr);
    ret = malloc(size);
    ft_memcpy(ret, ptr, block->size);
    return (ret);
}  */
