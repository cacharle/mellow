#include <ctype.h>
#include <stdint.h>

#include "internals.h"
#include "mellow/debug.h"

void mw_debug_show(void)
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
                "Block %zu: %p -> %p (%p -> %p) | %zu (%zu) %s\n",
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

void mw_debug_show_memory(void)
{
    block_t *curr;

    curr = mw_internals.heap;
    if (curr == NULL)
    {
        fputs("mellow: couldn't show uninitalized heap", stderr);
        return;
    }
    for (size_t i = 0;
         (void *)curr < (void *)mw_internals.heap + mw_internals.heap_size;
         curr = block_end(curr), i++)
    {
        fprintf(stderr,
                "Block %zu: %p -> %p | %zu %s\n",
                i,
                (void *)curr + sizeof(size_t),
                block_end(curr) - sizeof(size_t),
                block_payload_size(curr),
                block_available(curr) ? "(available)" : "(occupied)");
        void  *payload = block_payload(curr);
        size_t payload_size = block_payload_size(curr);
        if (payload_size > 256)
            break;
        for (size_t offset = 0; offset < payload_size; offset++)
        {
            if (offset % 64 == 0)
            {
                if (offset != 0)
                    fputc('\n', stderr);
                fprintf(stderr, "| %4zu | ", offset);
            }
            int c = ((uint8_t *)payload)[offset];
            if (isalnum(c))
                fprintf(stderr, "%c", c);
            else
                fputc('.', stderr);
        }
        fputs("\n-------------------------------------------------\n", stderr);
    }
}

void mw_debug_show_free_list(void)
{
    for (block_t *block = mw_internals.free_list; block != NULL; block = block->next)
    {
        fprintf(stderr,
                "Available block: %14p <- %p -> %14p | %zu\n",
                block->prev,
                block,
                block->next,
                block_payload_size(block));
    }
}
