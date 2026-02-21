#include <string.h>

#include "internals.h"

// TODO: look into mremap for large allocations

void *mw_realloc(void *ptr, size_t size)
{
    block_t *block;
    void    *ret;

    if (ptr == NULL)
        return mw_malloc(size);
    if (size == 0)
    {
        mw_free(ptr);
        return NULL;
    }
    block = ptr - sizeof(size_t);
    if (block_size(block) >= size)
        return ptr;

    // TODO: use internals to try to expand the block if it can be done
    // if (next block available) {
    //      split next block and expand the current one with the first half
    // }

    ret = mw_malloc(size);
    memcpy(ret, ptr, block_payload_size(block));
    mw_free(ptr);
    return ret;
}
