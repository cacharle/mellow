// #include "mellow/mellow.h"
//
// void
// show_alloc_mem(void)
// {
//     t_block *curr;
//
//     curr = g_internals.heap;
//     if (curr == NULL)
//     {
//         ft_putendl_fd("malloc: couldn't show uninitalized heap", STDERR_FILENO);
//         return;
//     }
//     while ((void *)curr < g_internals.heap_last)
//     {
//         ft_putnbr_base(curr, 16);
//         ft_putstr(" - ");
//         ft_putnbr_base(curr + curr->size, 16);
//         ft_putstr(" : ");
//         ft_putnbr_base(curr->size & ~1, 10);
//         ft_putendl(" bytes");
//         curr = curr + curr->size;
//     }
// }
//
// void
// show_alloc_mem_ex(void)
// {
//     t_block *curr;
//
//     curr = g_internals.heap;
//     if (curr == NULL)
//     {
//         ft_putendl_fd("malloc: couldn't show uninitalized heap", STDERR_FILENO);
//         return;
//     }
//     while ((void *)curr < g_internals.heap_last)
//     {
//         ft_putnbr_base(curr, 16);
//         ft_putstr(" - ");
//         ft_putnbr_base(curr + curr->size, 16);
//         ft_putstr(" : ");
//         ft_putnbr_base(curr->size & ~1, 10);
//         ft_putendl(" bytes");
//
//         uint8_t *data = (uint8_t *)curr;
//         size_t   offset = 0;
//         while (offset < curr->size)
//         {
//             if (offset % 16 == 0)
//             {
//                 ft_putstr("| ");
//                 ft_putnbr_base(offset, 16);
//                 ft_putstr("  ");
//             }
//             ft_putnbr_base(data[offset], 16);
//             if (offset % 8 == 0)
//                 ft_putchar(' ');
//             offset++;
//         }
//         curr = curr + curr->size;
//     }
// }
