#include <assert.h>
#include <errno.h>

#include "internals.h"

static size_t align(size_t x)
{
    return (x + (MW_ALIGNMENT_SIZE - 1)) & ~(MW_ALIGNMENT_SIZE - 1);
}

#define MW_MMAP_PROTECTION_FLAGS (PROT_READ | PROT_WRITE)
#define MW_MMAP_FLAGS (MAP_PRIVATE | MAP_ANONYMOUS)

static void *system_allocate(size_t size)
{
    void *ret = mmap(NULL, size, MW_MMAP_PROTECTION_FLAGS, MW_MMAP_FLAGS, -1, 0);
    if (ret == MAP_FAILED)
    {
        errno = ENOMEM;
        return NULL;
    }
    return ret;
}

// TODO: consider using MAP_FIXED or MAP_FIXED_VALIDATE since we're supposed to
// be the only one using mmap
static void *
system_allocate_with_hint(void *addr_hint, size_t size, bool *hint_followed)
{
    void *ret = mmap(
        addr_hint, size, MW_MMAP_PROTECTION_FLAGS, MW_MMAP_FLAGS | MAP_FIXED, -1, 0);
    if (ret != MAP_FAILED)
    {
        *hint_followed = true;
        return ret;
    }
    else
    {
        *hint_followed = false;
        return system_allocate(size);
    }
}

// struct zone zones[2] = {
//     { .bytes = 256 },
//     { .bytes = 4096 },
// };

struct mellow_internals mw_internals = {
    .heap = NULL,
    .free_list = NULL,
};

static bool heap_init(void)
{
    mw_internals.heap = system_allocate(MW_HEAP_CHUNK_SIZE);
    if (mw_internals.heap == NULL)
        return false;
    mw_internals.heap_size = MW_HEAP_CHUNK_SIZE;
    mw_internals.free_list = mw_internals.heap;
    mw_internals.free_list->prev = NULL;
    mw_internals.free_list->next = NULL;
    block_set_size(mw_internals.free_list, MW_HEAP_CHUNK_SIZE);
    return true;
}

// NOTE: could use available size flags to indicate wheather block is first/last
static bool grow_heap(void)
{
    assert(mw_internals.heap != NULL);
    void *last_addr = mw_internals.heap + mw_internals.heap_size;
    bool  hint_followed;
    void *new_chunk =
        system_allocate_with_hint(last_addr, MW_HEAP_CHUNK_SIZE, &hint_followed);
    if (new_chunk == NULL)
        return false;
    // The allocated addess is right after the current heap
    if (hint_followed)
    {
        // If the last block is available, extend that block with the new chunk
        size_t last_block_size = *((size_t *)(last_addr - sizeof(size_t)));
        if (!(last_block_size & 1))
        {
            block_t *last_block = last_addr - last_block_size;
            block_set_size(last_block, last_block_size + MW_HEAP_CHUNK_SIZE);
        }
        // Otherwise, the new chunk is a new available block and we add it to the
        // free list
        else
        {
            block_t *new_chunk_block = new_chunk;
            block_set_size(new_chunk_block, MW_HEAP_CHUNK_SIZE);
            // Push it to the start of the free list
            new_chunk_block->prev = NULL;
            new_chunk_block->next = mw_internals.free_list;
            new_chunk_block->next->prev = new_chunk_block;
            mw_internals.free_list = new_chunk_block;
        }
    }
    // When the hint isn't followed, we also just add the chunk to the free list
    else
    {
        block_t *new_chunk_block = new_chunk;
        block_set_size(new_chunk_block, MW_HEAP_CHUNK_SIZE);
        // Push it to the start of the free list
        new_chunk_block->prev = NULL;
        new_chunk_block->next = mw_internals.free_list;
        new_chunk_block->next->prev = new_chunk_block;
        mw_internals.free_list = new_chunk_block;
    }
    return true;
}

// Iterate over the available blocks, returns the first one that has enough space for
// the payload size
static block_t *find_fit(size_t payload_size)
{
    for (block_t *block = mw_internals.free_list; block != NULL; block = block->next)
    {
        if (block_payload_size(block) >= payload_size)
            return block;
    }
    return NULL;
}

static void split_block(block_t *block, size_t payload_size)
{
    size_t   new_block_size = payload_size + BLOCK_METADATA_SIZE;
    size_t   current_block_size = block_size(block);
    size_t   rest_size = current_block_size - new_block_size;
    block_t *block_prev = block->prev;
    block_t *block_next = block->next;
    // If the would-be rest is too small, just take ownership of the whole current
    // block and remove outselfs from the free list
    if (rest_size < BLOCK_AVAILABLE_METADATA_SIZE)
    {
        new_block_size = current_block_size;
        block_set_size(block, new_block_size | 1);
        if (block_prev != NULL)
            block_prev->next = block_next;
        if (block_next != NULL)
            block_next->prev = block_prev;
        if (block_prev == NULL)
            mw_internals.free_list = block_next;
        return;
    }

    // Resizing the block and marking it as occupied
    block_set_size(block, new_block_size | 1);
    block_t *rest = block_end(block);
    block_set_size(rest, current_block_size - new_block_size);
    // Setting the link to prev/next blocks of the one we're currently splitting to
    // the space we don't use at the end.
    rest->prev = block_prev;
    rest->next = block_next;
    // Also updating the prev/next blocks pointer to the rest
    if (rest->prev != NULL)
        rest->prev->next = rest;
    if (rest->next != NULL)
        rest->next->prev = rest;
    // Set the rest block as the start of the free list if its prev pointer is NULL
    if (rest->prev == NULL)
        mw_internals.free_list = rest;
}

void *mw_malloc(size_t size)
{
    if (mw_internals.heap == NULL)
    {
        if (!heap_init())
            return NULL;
    }
    size = align(size);
    // Ensure there is always space for the free list pointers in the payload once
    // the block is freed
    if (size < BLOCK_AVAILABLE_MIN_PAYLOAD_SIZE)
        size = BLOCK_AVAILABLE_MIN_PAYLOAD_SIZE;
    block_t *block = find_fit(size);
    if (block == NULL)
    {
        if (!grow_heap())
            return NULL;
        block = find_fit(size);
        if (block == NULL)
            return NULL;
    }
    split_block(block, size);
    return block_payload(block);
}
