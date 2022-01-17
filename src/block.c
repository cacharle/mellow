#include "internals.h"

void
block_set_size(block_t *block, size_t new_size)
{
    block->size = new_size;
    *block_footer(block) = new_size;
}

bool
block_available(block_t *block)
{
    return !(block->size & 1);
}

size_t
block_size(block_t *block)
{
    return block->size & ~1;
}

size_t
block_full_size(block_t *block)
{
    return block_size(block) + BLOCK_METADATA_SIZE;
}

void *
block_end(block_t *block)
{
    return (void *)block + block_full_size(block);
}

size_t *
block_footer(block_t *block)
{
    return (void *)block + sizeof(block_t) + block_size(block);
}

void *
block_data(block_t *block)
{
    return (void *)block + sizeof(block_t);
}
