#include "mellow/mellow.h"

int main(void)
{
    void *x = mw_malloc(10);
    mw_free(x);
    return 0;
}
