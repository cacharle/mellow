#include <criterion/criterion.h>

#include "mellow/mellow.h"
#include "mellow/debug.h"
#include "internals.h"
#include "utils.h"

Test(mw_realloc, increase_size)
{
    unsigned char *p1 = mw_malloc(32);
    unsigned char *p2 = mw_malloc(32);
    unsigned char *p3 = mw_malloc(32);
    memset(p1, 'a', 32);
    memset(p2, 'b', 32);
    memset(p3, 'c', 32);
    cr_assert_not_null(p1);
    cr_assert_not_null(p2);
    cr_assert_not_null(p3);
    p2 = mw_realloc(p2, 64);
    cr_assert_not_null(p2);
    for (int i = 0; i < 32; i++)
        cr_assert_eq(p2[i], 'b');
    memset(p2, '?', 64);
    // Check if previous neighbors were corrupted after the realloc
    for (int i = 0; i < 32; i++)
    {
        cr_assert_eq(p1[i], 'a');
        cr_assert_eq(p3[i], 'c');
    }
}

Test(mw_realloc, decrease_size)
{
    unsigned char *p1 = mw_malloc(128);
    unsigned char *p2 = mw_malloc(128);
    unsigned char *p3 = mw_malloc(128);
    memset(p1, 'a', 128);
    memset(p2, 'b', 128);
    memset(p3, 'c', 128);
    cr_assert_not_null(p1);
    cr_assert_not_null(p2);
    cr_assert_not_null(p3);
    p2 = mw_realloc(p2, 64);
    cr_assert_not_null(p2);
    for (int i = 0; i < 64; i++)
        cr_assert_eq(p2[i], 'b');
    memset(p2, '?', 64);
    // Check if previous neighbors were corrupted after the realloc
    for (int i = 0; i < 32; i++)
    {
        cr_assert_eq(p1[i], 'a');
        cr_assert_eq(p3[i], 'c');
    }
}

Test(mw_realloc, decrease_size_splits_block_if_enough_space)
{
    unsigned char *p1 = mw_malloc(128);
    unsigned char *p2 = mw_malloc(128);
    unsigned char *p3 = mw_malloc(128);
    p2 = mw_realloc(p2, 64);
    void *should_be_from_realloc_split = mw_malloc(32);
    cr_assert_gt(should_be_from_realloc_split, p1);
    cr_assert_lt(should_be_from_realloc_split, p3);
}

Test(mw_realloc, null_is_the_same_as_malloc)
{
    void *p = mw_realloc(NULL, 32);
    cr_assert_not_null(p);
    memset(p, 'a', 32);
    mw_free(p);
}

Test(mw_realloc, size_0_is_a_free)
{
    void *p = mw_malloc(32);
    cr_assert_not_null(p);
    void *ret = mw_realloc(p, 0);
    cr_assert_null(ret);
}
