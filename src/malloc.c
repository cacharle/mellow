#include "internals.h"

static size_t align(size_t x)
{
    return (x + (MW_ALIGNMENT_SIZE - 1)) & ~(MW_ALIGNMENT_SIZE - 1);
}

#define MW_MMAP_PROTECTION_FLAGS (PROT_READ | PROT_WRITE)
#define MW_MMAP_FLAGS (MAP_PRIVATE | MAP_ANONYMOUS)

static void *mw_mmap(void *addr_hint, size_t size)
{
    void *ret =
        mmap(addr_hint, size, MW_MMAP_PROTECTION_FLAGS, MW_MMAP_FLAGS, -1, 0);
    if (ret == MAP_FAILED)
    {
        errno = ENOMEM;
        return NULL;
    }
    return ret;
}

static void *system_allocate(size_t size)
{
    return mw_mmap(NULL, size);
}

static void *
system_allocate_with_hint(void *addr_hint, size_t size, bool *hint_followed)
{
    // Using MAP_FIXED uses addr_hint as the start address no matter what, which can
    // clobber memory that was asigned to other things (threads, stacks, etc..)
    // Using MAP_FIXED_NOREPLACE is the same as MAP_FIXED but fails if addr_hint is
    // already in use
    // Using the addr_hint alone and handling when it's not followed is the best
    // course of action
    void *ret = mw_mmap(addr_hint, size);
    *hint_followed = ret == addr_hint;
    if (ret == NULL)
        return NULL;
    return ret;
}

struct mellow_internals mw_internals = {
    .chunks = NULL,
    .free_list = NULL,
    .page_size = 0,
    .large_blocks = NULL,
};

static bool heap_init(void)
{
    mw_internals.page_size = getpagesize();
    assert(MW_CHUNK_SIZE % mw_internals.page_size == 0);
    mw_internals.chunks = system_allocate(MW_CHUNK_SIZE);
    if (mw_internals.chunks == NULL)
        return false;
    mw_internals.chunks->size = MW_CHUNK_SIZE;
    mw_internals.chunks->next = NULL;
    // mw_internals.chunks->start = (void*)mw_internals.chunks +
    // mw_internals.chunks->size; block_set_size(mw_internals.chunks->start,
    // MW_CHUNK_SIZE - MW_CHUNK_METADATA_SIZE);
    mw_internals.free_list = &mw_internals.chunks->start;
    mw_internals.free_list->prev = NULL;
    mw_internals.free_list->next = NULL;
    block_set_size(mw_internals.free_list, MW_CHUNK_SIZE - MW_CHUNK_METADATA_SIZE);
    return true;
}

// NOTE: could use available size flags to indicate wheather block is first/last
static bool grow_heap(void)
{
    assert(mw_internals.chunks != NULL);
    void *last_addr = ((void *)mw_internals.chunks) + mw_internals.chunks->size;
    assert((size_t)last_addr % mw_internals.page_size == 0);
    bool     hint_followed;
    chunk_t *new_chunk =
        system_allocate_with_hint(last_addr, MW_CHUNK_SIZE, &hint_followed);
    if (new_chunk == NULL)
        return false;
    new_chunk->size = MW_CHUNK_SIZE;
    new_chunk->next = mw_internals.chunks;
    mw_internals.chunks = new_chunk;
    // The allocated addess is right after the current heap
    if (hint_followed)
    {
        // If the last block is available, extend that block with the new chunk
        size_t last_block_size = *((size_t *)(last_addr - sizeof(size_t)));
        if (!(last_block_size & 1))
        {
            block_t *last_block = last_addr - last_block_size;
            block_set_size(last_block, last_block_size + MW_CHUNK_SIZE);
        }
        // Otherwise, the new chunk is a new available block and we add it to the
        // free list
        else
        {
            block_t *new_chunk_block = &new_chunk->start;
            block_set_size(new_chunk_block, MW_CHUNK_SIZE - MW_CHUNK_METADATA_SIZE);
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
        block_t *new_chunk_block = &new_chunk->start;
        block_set_size(new_chunk_block, MW_CHUNK_SIZE - MW_CHUNK_METADATA_SIZE);
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
    // block and remove ourselfs from the free list
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
    block_set_size(rest, rest_size);
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
    // Allocate a large block if size cannot fit in one chunk
    if (size > MW_CHUNK_MAX_BLOCK_SIZE)
    {
        large_block_t *large_block = system_allocate(size + sizeof(large_block_t));
        if (large_block == NULL)
            return NULL;
        large_block->size = size;
        large_block->prev = NULL;
        large_block->next = mw_internals.large_blocks;
        if (mw_internals.large_blocks != NULL)
            mw_internals.large_blocks->prev = large_block;
        mw_internals.large_blocks = large_block;
        return large_block_payload(large_block);
    }

    if (mw_internals.chunks == NULL)
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
