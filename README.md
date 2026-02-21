# mellow

`malloc` rewrite based on the school 42 project.

## Explicit free list

Each free block has a pointer to the next and previous free blocks. Making a
doubly linked list of free blocks.

Layout of a free block:

```
+------+------+------+---------------------------------------------+------+
|      |      |      |                                             |      |
| size | prev | next |                payload                      | size |
|      |      |      |                                             |      |
+------+------+------+---------------------------------------------+------+
```

Layout of an allocated block:

```
+------+-----------------------------------------------------------+------+
|      |                                                           |      |
| size |                        payload                            | size |
|      |                                                           |      |
+------+-----------------------------------------------------------+------+
```

The `next` and `prev` pointers are no longer needed and can be used in the payload.

`size` is the size of the *whole* block, not just the payload. It is also
aligned on an 8 bytes boundary, this makes the 3 lower bits of it always `0`,
we can store additional flags in those.

The lowest bit of `size` is high if the block is free.

Doubly linked list:

```
root >----+  +-----------------------------------+  +--------------------+   +----> NULL
          |  |                                   |  |                    |   |
          v  ^                                   v  ^                    v   ^
          +---------------+-------+------------+---------+------------+----------+
          |   free        | alloc |    alloc   |  free   |    alloc   |   free   |
          +---------------+-------+------------+---------+------------+----------+
            v   ^                                  v  ^                    v
            |   |                                  |  |                    |
NULL <------+   +----------------------------------+  +--------------------+
```

### Allocation

Starting from `root`, iterate over the free list until a block has enough space
to hold the asked size.

Once found, remove the block from the free list and return the address just after the size header.

### Deallocation

Using the size header and footers of the next and previous blocks, we can check
if they are also free, if so, we can coalesce them with the current block.

There are various ways of inserting back a block in the free list, the simplest
one is to just insert it at the beginning.

### TODO:

- [x] Dedicated mmap for too large payloads
- [ ] Test if allocating with hint works
- [ ] Free everything with munmap at the end
- [ ] Keep track of allocated chunks
- [ ] Add a fuzzer to catch every edge case

#### Secondary

- [ ] Create benchmark comparing libc's malloc/free to mine (there is mimalloc and jmalloc aswell)
- [ ] Benchmark resident memory aswell since I assume it's easy to be fast if
  you just allocate a lot of memory beforehand
- [ ] Establish allocation zones (max payload size per zone)
- [ ] Optimize calloc with large payloads that need a dedicated mmap, the
  system already zeroes the memory for us
- [ ] Test realloc
- [ ] Optimize realloc
- [ ] reallocarray (same as realloc but checks for multiplication overflows)
