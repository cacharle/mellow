#ifndef MELLOW_INTERNALS_H
#define MELLOW_INTERNALS_H

#define _XOPEN_SOURCE 500
// #define _POSIX_C_SOURCE 201112L
#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/resource.h>

typedef struct block
{
    size_t        size;
    struct block *prev;
    struct block *next;
} block_t;

typedef struct
{
    char  *name;
    size_t max_size;
} size_class_t;

struct mellow_internals
{
    block_t *heap;
    void    *heap_last;
    block_t *free_lists[3];
    size_t   small_max;
    size_t   tiny_max;
};

extern struct mellow_internals mw_internals;

enum
{
    MW_LIST_TINY = 0,
    MW_LIST_SMALL,
    MW_LIST_LARGE,
};

/*
** small is the page size
** tiny  is the page size >> 4
** If getpagesize() doesn't work use these macro instead
*/

#define SMALL_DEFAULT_BLOCK_SIZE 4096
#define TINY_DEFAULT_BLOCK_SIZE 256

/*
** Number of block in a zone
*/

#define ZONE_BLOCKS 100

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

void
block_set_size(block_t *block, size_t new_size);
bool
block_available(block_t *block);
size_t
block_size(block_t *block);

#endif
