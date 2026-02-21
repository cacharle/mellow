#include <limits.h>
#include <criterion/criterion.h>

#include "mellow/mellow.h"
#include "internals.h"
#include "mellow/debug.h"

Test(mw_calloc, basic)
{
    int *p = mw_calloc(10, sizeof(int));
    for (size_t i = 0; i < 10; i++)
        cr_assert_eq(p[i], 0);
}

Test(mw_calloc, reuse_memory)
{
    void *p = mw_malloc(64);
    memset(p, 'a', 64);
    mw_free(p);
    char *p2 = mw_calloc(64, sizeof(char));
    for (size_t i = 0; i < 64; i++)
        cr_assert_eq(p2[i], 0);
}

Test(mw_calloc, overflow)
{
    cr_assert_null(mw_calloc(ULONG_MAX - 10, ULONG_MAX - 10));
}

Test(mw_calloc, large_block)
{
    size_t size = MW_HEAP_CHUNK_SIZE * 2 - 124;
    char  *p = mw_calloc(size, sizeof(char));
    cr_assert_not_null(p);
    for (size_t i = 0; i < size; i++)
        cr_assert_eq(p[i], 0);
}
