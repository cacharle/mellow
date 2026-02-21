#include <assert.h>
#include <stdckdint.h>

#include "internals.h"
#include <string.h>

void *mw_calloc(size_t nmemb, size_t size)
{
    // nmemb or size can be 0, from the man page:
    // > If n or size is 0, then calloc() returns a unique pointer value that can
    // > later be successfully passed to free()
    size_t ret_size;
    bool   overflowed = ckd_mul(&ret_size, nmemb, size);
    if (overflowed)
        return NULL;
    void *ret = mw_malloc(ret_size);
    if (ret == NULL)
        return NULL;
    if (size > MW_CHUNK_MAX_BLOCK_SIZE)  // No need to zero out the memory for large
                                         // blocks since mmap does it for us
        return ret;
    return memset(ret, 0, ret_size);
}
