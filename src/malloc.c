#include "internals.h"

static const size_t alignment_size = 8;

static size_t
align(size_t x)
{
    return (x + (alignment_size - 1)) & ~(alignment_size - 1);
}

static void *
allocate(void *addr, size_t size)
{
    void *ret =
        mmap(addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    // TODO: limit process page to simulate failure
    if (ret == MAP_FAILED)
    {
        errno = ENOMEM;
        return NULL;
    }
    return ret;
}

// struct zone zones[2] = {
//     { .bytes = 256 },
//     { .bytes = 4096 },
// };

struct mellow_internals mw_internals = {
    .heap = NULL,
    .free_list = NULL,
};

static size_t heap_size = 1 << 11;

static bool
heap_init(void)
{
    heap_size = align(heap_size);
    mw_internals.heap = allocate(NULL, heap_size);
    if (mw_internals.heap == NULL)
        return false;
    mw_internals.heap_size = heap_size;
    mw_internals.free_list = mw_internals.heap;
    mw_internals.free_list->prev = NULL;
    mw_internals.free_list->next = NULL;
    block_set_size(mw_internals.free_list, heap_size);
    return true;
}

static block_t *
find_fit(size_t payload_size)
{
    for (block_t *block = mw_internals.free_list; block != NULL; block = block->next)
    {
        if (block_payload_size(block) >= payload_size)
            return block;
    }
    return NULL;
}

static void
split_block(block_t *block, size_t payload_size)
{
    size_t new_block_size = payload_size + BLOCK_METADATA_SIZE;
    size_t prev_block_size = block_size(block);
    block_t *block_prev = block->prev;
    block_t *block_next = block->next;
    block_set_size(block, new_block_size | 1);
    block_t *rest = block_end(block);
    block_set_size(rest, prev_block_size - new_block_size);
    rest->prev = block_prev;
    rest->next = block_next;
    if (rest->prev != NULL)
        rest->prev->next = rest;
    else
        mw_internals.free_list = rest;
    if (rest->next != NULL)
        rest->next->prev = rest;
}

void *
mw_malloc(size_t size)
{
    if (size == 0)
        return NULL;
    if (mw_internals.heap == NULL)
    {
        if (!heap_init())
            return NULL;
    }
    size = align(size);
    block_t *block = find_fit(size);
    if (block == NULL)
    {
        // if (!grow_heap())
        // 	return (NULL);
        // block = find_fit(size);
        return NULL;
    }
    split_block(block, size);
    return block_payload(block);
}
