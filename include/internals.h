#ifndef MELLOW_INTERNALS_H
#define MELLOW_INTERNALS_H

#define _XOPEN_SOURCE 500
#define _DEFAULT_SOURCE  // needed for MAP_ANONYMOUS
#include <unistd.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <mellow/mellow.h>

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

struct zone
{
    size_t pages;
    size_t bytes;
};

struct mellow_internals
{
    block_t *heap;
    size_t   heap_size;
    block_t *free_list;
};

extern struct mellow_internals mw_internals;

/*
 * small is the page size
 * tiny  is the page size >> 4
 * If getpagesize() doesn't work use these macro instead
 */

// #define SMALL_DEFAULT_BLOCK_SIZE 4096
// #define TINY_DEFAULT_BLOCK_SIZE 256

// #define ZONE_BLOCK_COUNT 100

/*
** 3 zones:
**   - tiny  > 0   <= 256
**   - small > 256 <= page size
**   - large > page size
**
**   - tiny  preallocate 100 max size block
**   - small preallocate 100 max size block
**   - large malloc is just mmap
**
**   if tiny or small zone are full reallocate a new zone
*/

void    block_set_size(block_t *block, size_t new_size);
bool    block_available(block_t *block);
size_t  block_size(block_t *block);
size_t  block_payload_size(block_t *block);
void   *block_end(block_t *block);
size_t *block_footer(block_t *block);
void   *block_payload(block_t *block);

#endif
