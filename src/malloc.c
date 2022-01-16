/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charles <me@cacharle.xyz>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/17 19:54:19 by charles           #+#    #+#             */
/*   Updated: 2020/09/18 15:56:02 by charles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mellow/mellow.h"

void *
allocate(void *addr, size_t size)
{
    void *ret;

    ret =
        mmap(addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ret == MAP_FAILED)
    {
        errno = ENOMEM;
        return (NULL);
    }
    return (ret);
}

t_malloc_internals g_internals = {.heap = NULL,
                                  .free_lists[LIST_TINY] = NULL,
                                  .free_lists[LIST_SMALL] = NULL,
                                  .free_lists[LIST_LARGE] = NULL,
                                  .small_max = 0,
                                  .tiny_max = 0};

bool
heap_init(void)
{
    size_t size;

    g_internals.small_max = getpagesize();
    g_internals.tiny_max = g_internals.small_max >> 4;

    size = (g_internals.tiny_max + sizeof(t_block) + sizeof(size_t)) * ZONE_BLOCKS;
    g_internals.free_lists[LIST_TINY] = allocate(NULL, size);
    g_internals.free_lists[LIST_TINY]->size = size;
    g_internals.free_lists[LIST_TINY]->next = NULL;
    g_internals.free_lists[LIST_TINY]->prev = NULL;

    size = (g_internals.small_max + sizeof(t_block) + sizeof(size_t)) * ZONE_BLOCKS;
    g_internals.free_lists[LIST_SMALL] = allocate(
        g_internals.free_lists[LIST_TINY] + g_internals.free_lists[LIST_TINY]->size,
        size);
    g_internals.free_lists[LIST_SMALL]->size = size;
    g_internals.free_lists[LIST_SMALL]->next = NULL;
    g_internals.free_lists[LIST_SMALL]->prev = NULL;

    g_internals.free_lists[LIST_LARGE] = NULL;
    g_internals.heap = g_internals.free_lists[LIST_TINY];
    g_internals.heap_last = g_internals.free_lists[LIST_SMALL] +
                            g_internals.free_lists[LIST_SMALL]->size;
    return (true);
}

size_t
align(size_t x)
{
    return (x + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

void
split_block(t_block *block, size_t new_size)
{
    t_block *rest;

    rest = block + new_size;
    block_set_size(rest, block->size - new_size);
    rest->prev = block;
    rest->next = block->next;
    block_set_size(block, new_size);
    block->next = rest;
}

t_block *
find_fit(size_t size)
{
    t_block *fit;

    if (size <= g_internals.tiny_max)
        fit = g_internals.free_lists[LIST_TINY];
    else if (size <= g_internals.small_max)
        fit = g_internals.free_lists[LIST_SMALL];
    else
        fit = g_internals.free_lists[LIST_LARGE];
    while (fit != NULL)
    {
        if (!(fit->size & 1) && (fit->size & ~1) - (2 * sizeof(size_t)) >= size)
            return (fit);
        fit = fit->next;
    }
    return (NULL);
}

void *
malloc(size_t size)
{
    t_block *block;

    if (size == 0)
        return (NULL);
    if (g_internals.heap == NULL)
        heap_init();
    size = align(size);
    block = find_fit(size);
    /* if (block == NULL) */
    /* { */
    /* 	if (!grow_heap()) */
    /* 		return (NULL); */
    /* 	block = find_fit(size); */
    /* } */
    split_block(block, size);
    block_set_size(block, block->size | 1);
    return (block + sizeof(size_t));
}

/* bool grow_heap(void) */
/* { */
/* 	t_block	*last; */
/*  */
/* 	if (g_heap == NULL) */
/* 	{ */
/* 		if ((g_heap = allocate(NULL, CHUNK_SIZE)) == NULL) */
/* 			return (false); */
/* 		g_heap->size = 0; */
/* 		g_heap->next = g_heap; */
/* 		g_heap->prev = g_heap; */
/* 	} */
/* 	last = g_heap; */
/* 	while (last->next != NULL) */
/* 		last = last->next; */
/* 	last->next = allocate(g_heap + CHUNK_SIZE, CHUNK_SIZE); */
/* 	if (last->next == NULL) */
/* 		return (false); */
/* 	last->next->prev = last; */
/* 	last->next->next = NULL; */
/* 	return (true); */
/* } */

/* void	free(void *ptr); */
/* { */
/* 	// mark block as freed */
/* 	// if block before is free coalesce */
/* 	// if block after is free coalesce */
/* } */
/*  */
/* void	*realloc(void *ptr, size_t size) */
/* { */
/* 	t_block	*block; */
/* 	void	*ret; */
/*  */
/* 	block = ptr - sizeof(t_block); */
/* 	if (block->size >= size) */
/* 		return (ptr); */
/* 	ret = malloc(size); */
/* 	ft_memcpy(ret, ptr, block->size); */
/* 	return (ret); */
/* } */
