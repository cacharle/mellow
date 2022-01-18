#include "internals.h"
#include "mellow/debug.h"

void
mw_debug_show(void)
{
    block_t *curr = mw_internals.heap;
    if (curr == NULL)
    {
        fputs("mellow: cannot show uninitalized heap\n", stderr);
        return;
    }
    for (size_t i = 0;
         (void *)curr < (void *)mw_internals.heap + mw_internals.heap_size;
         curr = block_end(curr), i++)
    {
        fprintf(stderr,
                "mellow: block %zu: %p -> %p (%p -> %p) | %zu (%zu) %s\n",
                i,
                (void *)curr + sizeof(size_t),
                block_end(curr) - sizeof(size_t),
                curr,
                block_end(curr),
                block_payload_size(curr),
                block_size(curr),
                block_available(curr) ? "(available)" : "(occupied)");
    }
}

/* void
show_alloc_mem_ex(void)
{
    block_t *curr;

    curr = mw_internals.heap;
    if (curr == NULL)
    {
        ft_putendl_fd("malloc: couldn't show uninitalized heap", STDERR_FILENO);
        return;
    }
    while ((void *)curr < mw_internals.heap_last)
    {
        ft_putnbr_base(curr, 16);
        ft_putstr(" - ");
        ft_putnbr_base(curr + curr->size, 16);
        ft_putstr(" : ");
        ft_putnbr_base(curr->size & ~1, 10);
        ft_putendl(" bytes");

        uint8_t *data = (uint8_t *)curr;
        size_t   offset = 0;
        while (offset < curr->size)
        {
            if (offset % 16 == 0)
            {
                ft_putstr("| ");
                ft_putnbr_base(offset, 16);
                ft_putstr("  ");
            }
            ft_putnbr_base(data[offset], 16);
            if (offset % 8 == 0)
                ft_putchar(' ');
            offset++;
        }
        curr = curr + curr->size;
    }
} */
