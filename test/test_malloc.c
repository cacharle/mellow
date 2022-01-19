#include <criterion/criterion.h>
#include "mellow/mellow.h"
#include "internals.h"
#include "utils.h"

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

