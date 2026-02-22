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

typedef struct chunk
{
    struct chunk *next;
    block_t       start;
} chunk_t;

struct mellow_internals
{
    chunk_t *chunks;
    block_t *free_list;
    size_t   page_size;
    // Blocks which don't fit in a chunk get a dedicated mmap
    large_block_t *large_blocks;
};

extern struct mellow_internals mw_internals;

#define MW_ALIGNMENT_SIZE 8
#define MW_CHUNK_SIZE (1 << 14)
#define MW_CHUNK_METADATA_SIZE sizeof(struct chunk *)
#if MW_CHUNK_SIZE % MW_ALIGNMENT_SIZE != 0
#error "Invalid heap size"
#endif
#define MW_CHUNK_MAX_BLOCK_SIZE (MW_CHUNK_SIZE - BLOCK_METADATA_SIZE)

#define MW_BLOCK_FLAG_OCCUPIED 1
#define MW_BLOCK_FLAG_CHUNK_BOUNDARY 1

size_t  block_size(block_t *block);
void    block_set_size(block_t *block, size_t new_size);
bool    block_available(block_t *block);
void    block_set_available(block_t *block);
bool    block_occupied(block_t *block);
void    block_set_occupied(block_t *block);
bool    block_chunk_boundary(block_t *block);
void    block_set_chunk_boundary(block_t *block);
void   *block_payload(block_t *block);
size_t  block_payload_size(block_t *block);
void   *block_end(block_t *block);
size_t *block_footer(block_t *block);

void *large_block_payload(large_block_t *block);

#endif
