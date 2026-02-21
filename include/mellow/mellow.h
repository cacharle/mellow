#ifndef MELLOW_H
#define MELLOW_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void *mw_malloc(size_t size);
    void  mw_free(void *ptr);
    void *mw_calloc(size_t nmemb, size_t size);
    void *mw_realloc(void *ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif
