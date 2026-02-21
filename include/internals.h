#ifndef MELLOW_INTERNALS_H
#define MELLOW_INTERNALS_H

#define _XOPEN_SOURCE 500
#define _DEFAULT_SOURCE  // needed for MAP_ANONYMOUS
#include <assert.h>
#include <errno.h>
#include <mellow/mellow.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <unistd.h>

// An available block has a prev and next pointer to the previous and next available
// blocks If the block is occupied, the prev/next pointers are part of the payload
typedef struct block
{
    size_t        size;
    struct block *prev;
    struct block *next;
} block_t;

// Metadata of an occupied payload (size of it at the start and end of it
#define BLOCK_METADATA_SIZE (2 * sizeof(size_t))
#define BLOCK_AVAILABLE_METADATA_SIZE \
    (2 * sizeof(size_t) + 2 * sizeof(struct block *))
#define BLOCK_AVAILABLE_MIN_PAYLOAD_SIZE (2 * sizeof(struct block *))

typedef struct large_block
{
    struct large_block *prev;
    struct large_block *next;
    size_t size;  // HAS to be the last element to be compatible with an allocated
                  // block (which starts at ptr - sizeof(size_t)
} large_block_t;

struct mellow_internals
{
    block_t *heap;
    size_t   heap_size;
    block_t *free_list;
    size_t   page_size;
    large_block_t
        *large_blocks;  // Blocks which don't fit in a chunk get a dedicated mmap
};

extern struct mellow_internals mw_internals;

#define MW_ALIGNMENT_SIZE 8
#define MW_HEAP_CHUNK_SIZE (1 << 14)
#if MW_HEAP_CHUNK_SIZE % MW_ALIGNMENT_SIZE != 0
#error "Invalid heap size"
#endif
#define MW_CHUNK_MAX_BLOCK_SIZE (MW_HEAP_CHUNK_SIZE - BLOCK_METADATA_SIZE)

void    block_set_size(block_t *block, size_t new_size);
bool    block_available(block_t *block);
size_t  block_size(block_t *block);
size_t  block_payload_size(block_t *block);
void   *block_end(block_t *block);
size_t *block_footer(block_t *block);
void   *block_payload(block_t *block);
void   *large_block_payload(large_block_t *block);

#endif
