#include "mellow/mellow.h"
#include "internals.h"

static void *
allocate(void *addr, size_t size)
{
    void *ret = mmap(addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, -1, 0);
    if (ret == MAP_FAILED)
    {
        errno = ENOMEM;
        return NULL;
    }
    return ret;
}

struct mellow_internals mw_internals = {.heap = NULL,
                                        .free_lists[MW_LIST_TINY] = NULL,
                                        .free_lists[MW_LIST_SMALL] = NULL,
                                        .free_lists[MW_LIST_LARGE] = NULL,
                                        .small_max = 0,
                                        .tiny_max = 0};

static bool
heap_init(void)
{
    size_t size;

    mw_internals.small_max = getpagesize();
    mw_internals.tiny_max = mw_internals.small_max >> 4;

    size = (mw_internals.tiny_max + sizeof(block_t) + sizeof(size_t)) * ZONE_BLOCKS;
    mw_internals.free_lists[MW_LIST_TINY] = allocate(NULL, size);
    mw_internals.free_lists[MW_LIST_TINY]->size = size;
    mw_internals.free_lists[MW_LIST_TINY]->next = NULL;
    mw_internals.free_lists[MW_LIST_TINY]->prev = NULL;

    size = (mw_internals.small_max + sizeof(block_t) + sizeof(size_t)) * ZONE_BLOCKS;
    mw_internals.free_lists[MW_LIST_SMALL] =
        allocate(mw_internals.free_lists[MW_LIST_TINY] +
                     mw_internals.free_lists[MW_LIST_TINY]->size,
                 size);
    mw_internals.free_lists[MW_LIST_SMALL]->size = size;
    mw_internals.free_lists[MW_LIST_SMALL]->next = NULL;
    mw_internals.free_lists[MW_LIST_SMALL]->prev = NULL;

    mw_internals.free_lists[MW_LIST_LARGE] = NULL;
    mw_internals.heap = mw_internals.free_lists[MW_LIST_TINY];
    mw_internals.heap_last = mw_internals.free_lists[MW_LIST_SMALL] +
                             mw_internals.free_lists[MW_LIST_SMALL]->size;
    return (true);
}

const size_t mw_alignment_size = 8;

static size_t
align(size_t x)
{
    return (x + (mw_alignment_size - 1)) & ~(mw_alignment_size - 1);
}

static void
split_block(block_t *block, size_t new_size)
{
    block_t *rest = block + new_size;
    block_set_size(rest, block->size - new_size);
    rest->prev = block;
    rest->next = block->next;
    block_set_size(block, new_size);
    block->next = rest;
}

static block_t *
find_fit(size_t size)
{
    block_t *block;
    if (size <= mw_internals.tiny_max)
        block = mw_internals.free_lists[MW_LIST_TINY];
    else if (size <= mw_internals.small_max)
        block = mw_internals.free_lists[MW_LIST_SMALL];
    else
        block = mw_internals.free_lists[MW_LIST_LARGE];
    for (; block != NULL; block = block->next)
    {
        if (block_available(block) && block_size(block) >= size)
            return block;
    }
    return NULL;
}

static bool
grow_heap(void)
{
    if (g_heap == NULL)
    {
        if ((g_heap = allocate(NULL, CHUNK_SIZE)) == NULL)
            return (false);
        g_heap->size = 0;
        g_heap->next = g_heap;
        g_heap->prev = g_heap;
    }
    block_t *last = g_heap;
    while (last->next != NULL)
        last = last->next;
    last->next = allocate(g_heap + CHUNK_SIZE, CHUNK_SIZE);
    if (last->next == NULL)
        return (false);
    last->next->prev = last;
    last->next->next = NULL;
    return (true);
}

void *
mw_malloc(size_t size)
{
    if (size == 0)
        return NULL;
    if (mw_internals.heap == NULL)
        heap_init();
    size = align(size);
    block_t *block = find_fit(size);

    /* if (block == NULL) */
    /* { */
    /* 	if (!grow_heap()) */
    /* 		return (NULL); */
    /* 	block = find_fit(size); */
    /* } */

    split_block(block, size);
    block_set_size(block, block->size | 1);
    return block + sizeof(size_t);
}


void
mw_free(void *ptr)
{
    // mark block as freed
    // if block before is free coalesce
    // if block after is free coalesce
}

/*
void	*realloc(void *ptr, size_t size)
{
    block_t	*block;
    void	*ret;

    block = ptr - sizeof(block_t);
    if (block->size >= size)
        return (ptr);
    ret = malloc(size);
    ft_memcpy(ret, ptr, block->size);
    return (ret);
}  */
