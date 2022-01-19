#include "utils.h"

void
assert_heap_eq(heap_layout_t heap_layout, size_t heap_layout_len)
{
    assert(heap_layout_len != 0);
    block_t *curr = mw_internals.heap;
    for (size_t i = 0;
         (void *)curr < (void *)mw_internals.heap + mw_internals.heap_size;
         curr = block_end(curr), i++)
    {
        if (heap_layout[i].payload_size != -1)
        {
            cr_assert_eq(
                block_payload_size(curr),
                heap_layout[i].payload_size,
                "block payload %zu != %zu",
                block_payload_size(curr),
                heap_layout[i].payload_size
            );
            cr_assert_eq(block_size(curr), heap_layout[i].payload_size + BLOCK_METADATA_SIZE);
        }
        if (heap_layout[i].state == AVAILABLE)
            cr_assert(block_available(curr));
        else if (heap_layout[i].state == OCCUPIED)
            cr_assert(!block_available(curr));
        if (heap_layout[i].payload != NULL)
        {
            cr_assert_eq(
                memcmp(
                    block_payload(curr),
                    heap_layout[i].payload,
                    heap_layout[i].payload_size
                ),
                0
            );
        }
    }
}
