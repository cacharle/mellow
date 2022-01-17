#include "mellow/mellow.h"
#include "mellow/debug.h"

int
main(void)
{
    mw_debug_show();
    void *p = mw_malloc(10);
    printf("p=%p\n", p);
    mw_debug_show();
    // mw_free(p);
    return 0;
}
