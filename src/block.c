#include "internals.h"

void block_set_size(block_t *block, size_t new_size)
{
    block->size = new_size;
    *block_footer(block) = new_size;
}

bool block_available(block_t *block)
{
    return !(block->size & 1);
}

size_t block_size(block_t *block)
{
    return block->size & ~1;
}

size_t block_payload_size(block_t *block)
{
    return block_size(block) - BLOCK_METADATA_SIZE;
}

void *block_end(block_t *block)
{
    return (void *)block + block_size(block);
}

size_t *block_footer(block_t *block)
{
    return block_end(block) - sizeof(size_t);
}

void *block_payload(block_t *block)
{
    return (void *)block + sizeof(size_t);
}

// Large blocks have the previous and next pointers even when occupied
void *large_block_payload(large_block_t *block)
{
    return (void *)block + sizeof(large_block_t);
}
