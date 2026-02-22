#include <assert.h>
#include "utils.h"
#include "internals.h"

void assert_heap_eq(heap_layout_t heap_layout, size_t heap_layout_len)
{
    assert(heap_layout_len != 0);
    block_t *curr = &mw_internals.chunks->start;
    for (size_t i = 0;
         (void *)curr < (void *)mw_internals.chunks + MW_CHUNK_SIZE;
         curr = block_end(curr), i++)
    {
        if (heap_layout[i].payload_size != -1)
        {
            cr_assert_eq(block_payload_size(curr),
                         heap_layout[i].payload_size,
                         "[%zu] block payload (actual) %zu != (expected) %zu",
                         i,
                         block_payload_size(curr),
                         heap_layout[i].payload_size);
            cr_assert_eq(block_size(curr),
                         heap_layout[i].payload_size + BLOCK_METADATA_SIZE);
        }
        if (heap_layout[i].state == AVAILABLE)
            cr_assert(block_available(curr));
        else if (heap_layout[i].state == OCCUPIED)
            cr_assert(!block_available(curr));
        if (heap_layout[i].payload != NULL)
        {
            cr_assert_eq(memcmp(block_payload(curr),
                                heap_layout[i].payload,
                                heap_layout[i].payload_size),
                         0);
        }
    }
}

void check_valid_free_list()
{
    // Passes if this doesn't crash :D
    for (block_t *block = mw_internals.free_list; block != NULL; block = block->next)
        ;
}
