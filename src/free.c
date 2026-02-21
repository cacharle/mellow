#include "internals.h"
#include <stdio.h>

// TODO: munmap when last free is called
void mw_free(void *ptr)
{
    block_t *block = ptr - sizeof(size_t);

    // Check if ptr is a large block
    if (block_size(block) > MW_CHUNK_MAX_BLOCK_SIZE)
    {
        large_block_t *large_block = ptr - sizeof(large_block_t);
        // Remove the block from the large blocks list
        if (large_block->prev != NULL)
            large_block->prev->next = large_block->next;
        if (large_block->next != NULL)
            large_block->next->prev = large_block->prev;
        if (large_block == mw_internals.large_blocks)
            mw_internals.large_blocks = NULL;
        munmap(large_block, large_block->size);
        return;
    }

    block_set_size(block, block_size(block));  // mark block as freed
    if (block != mw_internals.heap)
    {
        // if block before is free coalesce
        size_t prev_marked_size = *(size_t *)((void *)block - sizeof(size_t));
        if (!(prev_marked_size & 1))
        {
            size_t prev_size = prev_marked_size & ~1;
            size_t curr_size = block->size;
            // remove prev from free list
            block_t *prev_block = (void *)block - prev_size;
            if (prev_block->prev != NULL)
                prev_block->prev->next = prev_block->next;
            else
                mw_internals.free_list = prev_block->next;
            if (prev_block->next != NULL)
                prev_block->next->prev = prev_block->prev;
            block = prev_block;
            block_set_size(block, prev_size + curr_size);
        }
    }

    if (block_end(block) != (void *)mw_internals.heap + mw_internals.heap_size)
    {
        // if block after is free coalesce
        size_t next_marked_size = *(size_t *)block_end(block);
        if (!(next_marked_size & 1))
        {
            size_t   next_size = next_marked_size & ~1;
            size_t   curr_size = block->size;
            block_t *next_block = block_end(block);
            if (next_block->prev != NULL)
                next_block->prev->next = next_block->next;
            else
                mw_internals.free_list = next_block->next;
            if (next_block->next != NULL)
                next_block->next->prev = next_block->prev;
            block_set_size(block, next_size + curr_size);
        }
    }

    // insert block at the start free list
    block->prev = NULL;
    block->next = mw_internals.free_list;
    if (mw_internals.free_list != NULL)
        mw_internals.free_list->prev = block;
    mw_internals.free_list = block;
}
