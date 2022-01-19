#include "mellow/mellow.h"
#include "mellow/debug.h"

int
main(void)
{
    mw_debug_show();
    int *p = mw_malloc(10 * sizeof(int));
    mw_debug_show();
    printf("p=%p\n", p);
    for (int i = 0; i < 10; i++)
        p[i] = i * i;
    for (int i = 0; i < 10; i++)
        printf("%d \n", p[i]);
    putc('\n', stdout);
    void *p2 = mw_malloc(11);
    void *p3 = mw_malloc(12);
    void *p4 = mw_malloc(13);
    mw_debug_show();
    printf("================= free1 =============\n");
    mw_free(p2);
    mw_debug_show();
    printf("================= free2 =============\n");
    mw_free(p);
    mw_debug_show();
    return 0;
}
