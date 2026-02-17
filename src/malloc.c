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
    // TODO: consider using MAP_FIXED or MAP_FIXED_VALIDATE since where supposed to
    // be the only one using mmap
    //       still compatible with libc if we replace malloc with mw_malloc
    void *ret =
        mmap(addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    // TODO: limit process page to simulate failure
    //       const struct rlimit rlimit = { .rlim_cur = 3000 };
    //       setrlimit(RLIMIT_DATA, &rlimit);
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

// NOTE: mallocng in musl uses some kind of meta area (should look into that)

// NOTE: could use available size flags to indicate wheather block is first/last
// bool
// grow_heap(void)
// {
//     block_t *last;
//
//     if (g_heap == NULL)
//     {
//         if ((g_heap = allocate(NULL, CHUNK_SIZE)) == NULL)
//             return (false);
//         g_heap->size = 0;
//         g_heap->next = g_heap;
//         g_heap->prev = g_heap;
//     }
//     last = g_heap;
//     while (last->next != NULL)
//         last = last->next;
//     last->next = allocate(g_heap + CHUNK_SIZE, CHUNK_SIZE);
//     if (last->next == NULL)
//         return (false);
//     last->next->prev = last;
//     last->next->next = NULL;
//     return (true);
// }

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
    size_t   new_block_size = payload_size + BLOCK_METADATA_SIZE;
    size_t   prev_block_size = block_size(block);
    block_t *block_prev = block->prev;
    block_t *block_next = block->next;
    block_set_size(block, new_block_size | 1);
    block_t *rest = block_end(block);
    // if rest size < sizeof(block_t) + sizeof(size_t) + 16
    //     don't split, allocate whole block
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
