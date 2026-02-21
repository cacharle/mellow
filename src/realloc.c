#include <string.h>

#include "internals.h"

// TODO: look into mremap for large allocations

void *mw_realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
        return mw_malloc(size);
    if (size == 0)
    {
        mw_free(ptr);
        return NULL;
    }

    block_t *block = ptr - sizeof(size_t);
    // Return the existing block if the size is reduced since there is already enough
    // space But try to split the rest into a new available block to save the space
    if (block_size(block) >= size)
    {
        size_t new_block_size = size + BLOCK_METADATA_SIZE;
        size_t rest_size = block_size(block) - new_block_size;
        // Split if there is enough space do to so
        if (rest_size >= BLOCK_AVAILABLE_METADATA_SIZE)
        {
            block_set_size(block, new_block_size | 1);
            block_t *rest = block_end(block);
            block_set_size(rest, rest_size);
            rest->next = mw_internals.free_list;
            if (mw_internals.free_list != NULL)
                mw_internals.free_list->prev = rest;
            mw_internals.free_list = rest;
        }
        return ptr;
    }

    // TODO: use internals to try to expand the block if it can be done
    // if (next block available) {
    //      split next block and expand the current one with the first half
    // }

    void *ret = mw_malloc(size);
    memcpy(ret, ptr, block_payload_size(block));
    mw_free(ptr);
    return ret;
}
