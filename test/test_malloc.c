#include <assert.h>
#include <criterion/criterion.h>
#include "mellow/mellow.h"
#include "internals.h"

enum block_state {
    AVAILABLE,
    OCCUPIED,
};

struct heap_layout_block {
    enum block_state state;
    size_t payload_size;
    void *payload;
};

typedef struct heap_layout_block heap_layout_t[];

static void
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

#define ASSERT_HEAP_EQ(layout) assert_heap_eq(layout, sizeof(layout) / sizeof(layout[0]))

Test(mw_malloc, basic)
{
    size_t len = 10;
    size_t payload_size = sizeof(int) * len;
    int *p = mw_malloc(payload_size);
    for (int i = 0; i < len; i++)
        p[i] = i * i;
    heap_layout_t heap_layout = {
        {OCCUPIED,  .payload_size = payload_size, .payload = p},
        {AVAILABLE, .payload_size = -1, .payload = NULL}
    };
    ASSERT_HEAP_EQ(heap_layout);
}

Test(mw_malloc, array_2d)
{
    size_t len = 4;
    size_t payload_size = sizeof(int*) * len;
    size_t row_payload_size = sizeof(int) * len;
    int **p = mw_malloc(payload_size);
    for (int i = 0; i < len; i++)
        p[i] = mw_malloc(row_payload_size);
    for (int i = 0; i < len; i++)
        for (int j = 0; j < len; j++)
            p[i][j] = i * j;
    heap_layout_t heap_layout = {
        {OCCUPIED,  .payload_size = payload_size, .payload = p},
        {OCCUPIED,  .payload_size = row_payload_size, .payload = p[0]},
        {OCCUPIED,  .payload_size = row_payload_size, .payload = p[1]},
        {OCCUPIED,  .payload_size = row_payload_size, .payload = p[2]},
        {OCCUPIED,  .payload_size = row_payload_size, .payload = p[3]},
        {AVAILABLE, .payload_size = -1, .payload = NULL}
    };
    ASSERT_HEAP_EQ(heap_layout);
}

Test(mw_malloc, alignment_8)
{
    size_t size = 7;
    mw_malloc(size);
    heap_layout_t heap_layout = {
        {OCCUPIED,  .payload_size = 8, .payload = NULL},
        {AVAILABLE, .payload_size = -1, .payload = NULL}
    };
    ASSERT_HEAP_EQ(heap_layout);
}

Test(mw_malloc, alignment_16)
{
    size_t size = 10;
    mw_malloc(size);
    heap_layout_t heap_layout = {
        {OCCUPIED,  .payload_size = 16, .payload = NULL},
        {AVAILABLE, .payload_size = -1, .payload = NULL}
    };
    ASSERT_HEAP_EQ(heap_layout);
}

Test(mw_malloc, alignment_32)
{
    size_t size = 25;
    mw_malloc(size);
    heap_layout_t heap_layout = {
        {OCCUPIED,  .payload_size = 32, .payload = NULL},
        {AVAILABLE, .payload_size = -1, .payload = NULL}
    };
    ASSERT_HEAP_EQ(heap_layout);
}

Test(mw_malloc, zero_size)
{
    cr_assert_null(mw_malloc(0));
    heap_layout_t heap_layout = {
        {AVAILABLE, .payload_size = -1, .payload = NULL}
    };
    ASSERT_HEAP_EQ(heap_layout);
}

// Test(mw_free, basic)
// {
//     void *p = mw_malloc(10);
//     mw_free(p);
//     // check debug
// }
//
// Test(mw_free, coalesce_before)
// {
//     void *p1 = mw_malloc(10);
//     void *p2 = mw_malloc(20);
//     mw_free(p1);
//     mw_free(p2);
//     // check debug
// }
//
// Test(mw_free, coalesce_after)
// {
//     void *p1 = mw_malloc(10);
//     void *p2 = mw_malloc(20);
//     mw_free(p2);
//     mw_free(p1);
//     // check debug
// }
