#include "internals.h"

/*
 * calloc isn't always the same as malloc+memset
 * - it checks for multiplication overflow
 * - TODO: on big allocations (when mmap forced), the system zeroes
 *   the allocation by itself already so calling memset is a wasteful
 *   (on small allocation, it is equivalent to malloc+memset)
 *   the system zeroes the pages for security reasons
 *   (sensitive information could still be there)
 */
void *
mw_calloc(size_t nmemb, size_t size)
{
    size_t ret_size = nmemb * size;
    if (size == 0 || ret_size / size != nmemb)
        return NULL;
    void *ret = mw_malloc(ret_size);
    return memset(ret, 0, ret_size);
}
