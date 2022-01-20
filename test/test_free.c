#include <criterion/criterion.h>
#include "mellow/mellow.h"
#include "internals.h"
#include "utils.h"

Test(mw_free, basic)
{
    void *p = mw_malloc(10);
    mw_free(p);
    cr_assert_not_null(mw_internals.free_list);
    heap_layout_t heap_layout = {{AVAILABLE, .payload_size = -1, .payload = NULL}};
    ASSERT_HEAP_EQ(heap_layout);
}

Test(mw_free, coalesce_before)
{
    void *p1 = mw_malloc(16);
    void *p2 = mw_malloc(32);
    void *p3 = mw_malloc(16);
    mw_free(p2);
    mw_free(p1);
    memset(p3, 42, 10);
    heap_layout_t heap_layout = {
        {AVAILABLE, .payload_size = 48 + 2 * sizeof(size_t), .payload = NULL},
        {OCCUPIED, .payload_size = 16, .payload = p3},
        {AVAILABLE, .payload_size = -1, .payload = NULL},
    };
    ASSERT_HEAP_EQ(heap_layout);
}

Test(mw_free, coalesce_after)
{
    void *p1 = mw_malloc(16);
    void *p2 = mw_malloc(32);
    void *p3 = mw_malloc(16);
    mw_free(p1);
    mw_free(p2);
    memset(p3, 42, 10);
    heap_layout_t heap_layout = {
        {AVAILABLE, .payload_size = 48 + 2 * sizeof(size_t), .payload = NULL},
        {OCCUPIED, .payload_size = 16, .payload = p3},
        {AVAILABLE, .payload_size = -1, .payload = NULL},
    };
    ASSERT_HEAP_EQ(heap_layout);
}

Test(mw_free, coalesce_before_after)
{
    void *p1 = mw_malloc(16);
    void *p2 = mw_malloc(32);
    void *p3 = mw_malloc(16);
    mw_free(p1);
    mw_free(p3);
    mw_free(p2);
    heap_layout_t heap_layout = {
        {AVAILABLE, .payload_size = -1, .payload = NULL},
    };
    ASSERT_HEAP_EQ(heap_layout);
}
