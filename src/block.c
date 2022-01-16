#include "internals.h"

void
block_set_size(block_t *block, size_t new_size)
{
    block->size = new_size;
    *((uint8_t *)block + block->size - sizeof(size_t)) = block->size;
}

bool
block_available(block_t *block)
{
    return !(block->size & 1);
}

size_t
block_size(block_t *block)
{
    return (block->size & ~1) - (2 * sizeof(size_t));
}
