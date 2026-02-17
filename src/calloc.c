#include <assert.h>
#include <stdckdint.h>

#include "internals.h"

/*
 * calloc isn't always the same as malloc+memset
 * - TODO: on big allocations (when mmap forced), the system zeroes
 *   the allocation by itself already so calling memset is a wasteful
 *   (on small allocation, it is equivalent to malloc+memset)
 *   the system zeroes the pages for security reasons
 *   (sensitive information could still be there)
 */
void *
mw_calloc(size_t nmemb, size_t size)
{
    // nmemb or size can be 0, from the man page:
    // > If n or size is 0, then calloc() returns a unique pointer value that can
    // > later be successfully passed to free()
    size_t ret_size;
    bool overflowed = ckd_mul(&ret_size, nmemb, size);
    if (overflowed)
        return NULL;
    void *ret = mw_malloc(ret_size);
    if (ret == NULL)
        return NULL;
    return memset(ret, 0, ret_size);
}
