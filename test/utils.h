#ifndef MELLOW_TEST_UTILS_H
#define MELLOW_TEST_UTILS_H

#include <stddef.h>
#include <assert.h>
#include <criterion/criterion.h>
#include "internals.h"

enum block_state
{
    AVAILABLE,
    OCCUPIED,
};

struct heap_layout_block
{
    enum block_state state;
    size_t           payload_size;
    void            *payload;
};

typedef struct heap_layout_block heap_layout_t[];

void
assert_heap_eq(heap_layout_t heap_layout, size_t heap_layout_len);

#define ASSERT_HEAP_EQ(layout) \
    assert_heap_eq(layout, sizeof(layout) / sizeof(layout[0]))

#endif
