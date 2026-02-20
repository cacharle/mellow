#include <stdint.h>
#include <errno.h>

#include <criterion/criterion.h>

#include "mellow/mellow.h"
#include "mellow/debug.h"
#include "internals.h"
#include "utils.h"

Test(mw_malloc, basic)
{
    size_t len = 10;
    size_t payload_size = sizeof(int) * len;
    int   *p = mw_malloc(payload_size);
    for (int i = 0; i < len; i++)
        p[i] = i * i;
    heap_layout_t heap_layout = {
        {OCCUPIED,  .payload_size = payload_size, .payload = p   },
        {AVAILABLE, .payload_size = -1,           .payload = NULL}
    };
    ASSERT_HEAP_EQ(heap_layout);
}

Test(mw_malloc, array_2d)
{
    size_t len = 4;
    size_t payload_size = sizeof(int *) * len;
    size_t row_payload_size = sizeof(int) * len;
    int  **p = mw_malloc(payload_size);
    for (int i = 0; i < len; i++)
        p[i] = mw_malloc(row_payload_size);
    for (int i = 0; i < len; i++)
        for (int j = 0; j < len; j++)
            p[i][j] = i * j;
    heap_layout_t heap_layout = {
        {OCCUPIED,  .payload_size = payload_size,     .payload = p   },
        {OCCUPIED,  .payload_size = row_payload_size, .payload = p[0]},
        {OCCUPIED,  .payload_size = row_payload_size, .payload = p[1]},
        {OCCUPIED,  .payload_size = row_payload_size, .payload = p[2]},
        {OCCUPIED,  .payload_size = row_payload_size, .payload = p[3]},
        {AVAILABLE, .payload_size = -1,               .payload = NULL}
    };
    ASSERT_HEAP_EQ(heap_layout);
}

Test(mw_malloc, alignment_8)
{
    size_t size = 31;
    cr_assert_not_null(mw_malloc(size));
    heap_layout_t heap_layout = {
        //                          vv resized to 32
        {OCCUPIED,  .payload_size = 32, .payload = NULL},
        {AVAILABLE, .payload_size = -1, .payload = NULL}
    };
    ASSERT_HEAP_EQ(heap_layout);
}

Test(mw_malloc, alignment_16)
{
    size_t size = 10;
    cr_assert_not_null(mw_malloc(size));
    heap_layout_t heap_layout = {
        {OCCUPIED,  .payload_size = 16, .payload = NULL},
        {AVAILABLE, .payload_size = -1, .payload = NULL}
    };
    ASSERT_HEAP_EQ(heap_layout);
}

Test(mw_malloc, alignment_32)
{
    size_t size = 25;
    cr_assert_not_null(mw_malloc(size));
    heap_layout_t heap_layout = {
        {OCCUPIED,  .payload_size = 32, .payload = NULL},
        {AVAILABLE, .payload_size = -1, .payload = NULL}
    };
    ASSERT_HEAP_EQ(heap_layout);
}

Test(mw_malloc, zero_size)
{
    cr_assert_not_null(mw_malloc(0));
    heap_layout_t heap_layout = {
        // 16 is the minimum payload size (for the free list pointers)
        {OCCUPIED,  .payload_size = 16, .payload = NULL},
        {AVAILABLE, .payload_size = -1, .payload = NULL}
    };
    ASSERT_HEAP_EQ(heap_layout);
}

Test(mw_malloc, split_non_first_free_list)
{
    void    *p1 = mw_malloc(32);
    void    *p2 = mw_malloc(32);
    void    *p3 = mw_malloc(32);
    uint8_t *p4 = mw_malloc(64);
    memset(p4, 42, 64);
    (void)p1;
    (void)p3;
    mw_free(p4);  // free_list -> p3
    mw_free(p2);  // free_list -> p2 -> p3
    void *p5 = mw_malloc(64);
    cr_assert_eq(p5, p4);
    heap_layout_t heap_layout = {
        {OCCUPIED,  .payload_size = 32, .payload = NULL},
        {AVAILABLE, .payload_size = 32, .payload = NULL},
        {OCCUPIED,  .payload_size = 32, .payload = NULL},
        {OCCUPIED,  .payload_size = 64, .payload = p4  },
        {AVAILABLE, .payload_size = -1, .payload = NULL},
    };
    ASSERT_HEAP_EQ(heap_layout);
}

Test(mw_malloc, block_no_free_list_last)
{
    void *p1 = mw_malloc(64);
    void *p2 = mw_malloc(64);
    void *p3 = mw_malloc(64);
    void *p4 = mw_malloc(64);
    (void)p2;
    mw_free(p1);  // free_list -> p1(32)
    mw_free(p3);  // free_list -> p3(32) -> p1(32)
                  //               v-- split_block with block->next != NULL
    uint64_t *p5 = mw_malloc(32);  // free_list -> (16) -> (32)
    memset(p5, 42, 32);
    heap_layout_t heap_layout = {
        {AVAILABLE, .payload_size = 64,           .payload = NULL}, // previous p1
        {OCCUPIED,  .payload_size = 64,           .payload = NULL}, // p2
        {OCCUPIED,
         .payload_size = 32,
         .payload = p5                                           }, //                      | both combined were p3
        {AVAILABLE,
         .payload_size = 32 - 2 * sizeof(size_t),
         .payload = NULL                                         }, // splitted rest by p5  |
        {OCCUPIED,  .payload_size = 64,           .payload = p4  },
        {AVAILABLE, .payload_size = -1,           .payload = NULL},
    };
    ASSERT_HEAP_EQ(heap_layout);
}

Test(mw_malloc, split_block_no_zero_size)
{
    void *p1 = mw_malloc(32);
    void *p2 = mw_malloc(32);
    void *p3 = mw_malloc(32);
    void *p4 = mw_malloc(32);
    memset(p1, '1', 32);
    memset(p2, '2', 32);
    memset(p3, '3', 32);
    memset(p4, '4', 32);
    (void)p2;
    mw_free(p1);                   // free_list -> p1(32)
    mw_free(p3);                   // free_list -> p3(32) -> p1(32)
                                   // v-- split_block with block->next != NULL
    uint64_t *p5 = mw_malloc(16);  // free_list -> (16) -> (32)
    memset(p5, '5', 32);
    heap_layout_t heap_layout = {
        {AVAILABLE, .payload_size = 32, .payload = NULL}, // previous p1
        {OCCUPIED,  .payload_size = 32, .payload = NULL}, // p2
        // supposed to be split but the rest would have a payload size of 0
        {OCCUPIED,  .payload_size = 32, .payload = p5  },
        {OCCUPIED,  .payload_size = 32, .payload = p4  },
        {AVAILABLE, .payload_size = -1, .payload = NULL},
    };
    ASSERT_HEAP_EQ(heap_layout);
    check_valid_free_list();
}

Test(mw_malloc, out_of_memory)
{
    const struct rlimit rlimit = {
        .rlim_cur = 32,
        .rlim_max = 32,
    };
    setrlimit(RLIMIT_AS, &rlimit);
    void *p = mw_malloc(64);
    cr_assert_null(p);
    cr_assert_eq(errno, ENOMEM);
}

Test(mw_malloc, grow_heap_if_block_doesnt_fit_with_some_other_blocks_before)
{
    (void)mw_malloc(256);
    (void)mw_malloc(256);
    // Fits in a chunk so allocating a new chunk will work
    void *p = mw_malloc(MW_HEAP_CHUNK_SIZE - 128);
    cr_assert_not_null(p);
    check_valid_free_list();
}

Test(mw_malloc, grow_heap_with_a_lot_of_mid_size_allocations)
{
    size_t size = MW_HEAP_CHUNK_SIZE / 4;
    cr_assert_not_null(mw_malloc(size));
    cr_assert_not_null(mw_malloc(size));
    cr_assert_not_null(mw_malloc(size));
    cr_assert_not_null(mw_malloc(size));  // Should grow here
    cr_assert_not_null(mw_malloc(size));
    cr_assert_not_null(mw_malloc(size));
    cr_assert_not_null(mw_malloc(size));  // Should grow here again
}

// Test(mw_malloc, grow_heap_with_2_times_the_chunk_size)
// {
//     void *p = mw_malloc(MW_HEAP_CHUNK_SIZE * 2 - 124);
//     cr_assert_not_null(p);
//     check_valid_free_list();
// }
