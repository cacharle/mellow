#include <limits.h>
#include <criterion/criterion.h>
#include "mellow/mellow.h"

Test(mw_calloc, basic)
{
    int *p = mw_calloc(10, sizeof(int));
    for (size_t i = 0; i < 10; i++)
        cr_assert_eq(p[i], 0);
}

Test(mw_calloc, overflow)
{
    cr_assert_null(mw_calloc(ULONG_MAX - 10, ULONG_MAX - 10));
}
