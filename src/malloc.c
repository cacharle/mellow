/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charles <me@cacharle.xyz>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/17 19:54:19 by charles           #+#    #+#             */
/*   Updated: 2020/09/17 19:54:20 by charles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

static t_block_header	*g_heap = NULL;

size_t	align(size_t x)
{
	return (x + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

t_block	*find_fit(size_t size)
{
	t_block	*fit;

	fit = g_heap;
	while (fit != NULL)
	{
		if (!(fit->size & 1) && (fit->size & ~1) - sizeof(t_block) >= size)
			return (fit);
		fit = fit->next;
	}
	return (NULL);
}

void	*malloc(size_t size)
{
	t_block	*block;

	if (size == 0)
		return (NULL);
	size = align(size);
	block = find_fit(size);
	if (block == NULL)
	{
		if (!grow_heap())
			return (NULL);
		block = find_fit(size);
	}
	block->size |= 1;
	return (block + sizeof(t_block))
}

bool grow_heap(void)
{
	t_block	*last;

	if (g_heap == NULL)
	{
		if ((g_heap = allocate(NULL, CHUNK_SIZE)) == NULL)
			return (false);
		g_heap->size = 0;
		g_heap->next = g_heap;
		g_heap->prev = g_heap;
	}
	last = g_heap;
	while (last->next != NULL)
		last = last->next;
	last->next = allocate(g_heap + CHUNK_SIZE, CHUNK_SIZE);
	if (last->next == NULL)
		return (false);
	last->next->prev = last;
	last->next->next = NULL;
	return (true);
}

void	*allocate(void *addr, size_t size)
{
	void	*ret;

	ret = mmap(addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (ret == MAP_FAILED)
	{
		errno = ENOMEM;
		return (NULL);
	}
	return (ret);
}

void	free(void *ptr);
{
	// mark block as freed
	// if block before is free coalesce
	// if block after is free coalesce
}

void	*realloc(void *ptr, size_t size)
{
	t_block	*block;
	void	*ret;

	block = ptr - sizeof(t_block);
	if (block->size >= size)
		return (ptr);
	ret = malloc(size);
	ft_memcpy(ret, ptr, block->size);
	return (ret);
}
