#include "ft_malloc.h"

void	block_set_size(t_block *block, size_t new_size)
{
	block->size = new_size;
	*((uint8_t*)block + block->size - sizeof(size_t)) = block->size;
}
