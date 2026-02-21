#include <string.h>

#include "mellow/mellow.h"
#include "mellow/debug.h"

int main(void)
{
    // void *p = mw_malloc(10);
    // mw_free(p);
    // mw_debug_show();
    // int *p = mw_malloc(10 * sizeof(int));
    // mw_debug_show();
    // printf("p=%p\n", p);
    // for (int i = 0; i < 10; i++)
    //     p[i] = i * i;
    // for (int i = 0; i < 10; i++)
    //     printf("%d \n", p[i]);
    // putc('\n', stdout);
    // void *p2 = mw_malloc(11);
    // void *p3 = mw_malloc(12);
    // void *p4 = mw_malloc(13);
    // mw_debug_show();
    // printf("================= free1 =============\n");
    // mw_free(p2);
    // mw_debug_show();
    // printf("================= free2 =============\n");
    // mw_free(p);
    // mw_debug_show();
    // void *p1 = mw_malloc(32);
    // void *p2 = mw_malloc(32);
    // void *p3 = mw_malloc(32);
    // void *p4 = mw_malloc(32);
    // mw_free(p1);  // free_list -> p1(32)
    // mw_free(p3);  // free_list -> p3(32) -> p1(32)
    // //                                //               v-- split_block with
    // //                                block->next != NULL
    // void *p5 = mw_malloc(16);  // free_list -> (16) -> (32)
    // memset(p5, 'a', 8);
    // memset(p5 + 8, 'b', 8);
    // // mw_debug_show();
    // mw_debug_show_memory();
    // (void)p2;
    // (void)p4;
    // (void)p5;
    return 0;
}
