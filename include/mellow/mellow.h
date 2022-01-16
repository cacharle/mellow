/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charles <me@cacharle.xyz>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/17 08:39:26 by charles           #+#    #+#             */
/*   Updated: 2020/09/18 15:51:26 by charles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_H
#define MALLOC_H

#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/resource.h>

void *
mw_malloc(size_t size);
void
mw_free(void *ptr);
void *
mw_realloc(void *ptr, size_t size);

// coalesce blocks

typedef struct s_block
{
    size_t          size;
    struct s_block *prev;
    struct s_block *next;
} t_block;

typedef struct
{
    char  *name;
    size_t max_size;
} t_size_class;

typedef struct
{
    t_block *heap;
    void    *heap_last;
    t_block *free_lists[3];
    size_t   small_max;
    size_t   tiny_max;
} t_malloc_internals;

extern t_malloc_internals g_internals;

enum
{
    LIST_TINY = 0,
    LIST_SMALL,
    LIST_LARGE,
};

#define ALIGNMENT 8

/*
** small is the page size
** tiny  is the page size >> 4
** If getpagesize() doesn't work use these macro instead
*/

#define SMALL_DEFAULT_BLOCK_SIZE 4096
#define TINY_DEFAULT_BLOCK_SIZE 256

/*
** Number of block in a zone
*/

#define ZONE_BLOCKS 100

/*
** 3 zones:
**   - tiny  > 0   <= 256
**   - small > 256 <= page size
**   - large > page size
**
**   - tiny  preallocate 100 max size block
**   - small preallocate 100 max size block
**   - large malloc is just mmap
**
**   if tiny or small zone are full reallocate a new zone
*/

/*
** block.c
*/

void
block_set_size(t_block *block, size_t new_size);

/*
** debug.c
*/

void
show_alloc_mem(void);
void
show_alloc_mem_ex(void);

#endif
