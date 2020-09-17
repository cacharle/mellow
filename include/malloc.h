/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charles <me@cacharle.xyz>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/17 08:39:26 by charles           #+#    #+#             */
/*   Updated: 2020/09/17 16:41:13 by charles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_H
# define MALLOC_H

# include <unistd.h>
# include <errno.h>
# include <stddef.h>
# include <stdbool.h>
# include <sys/mman.h>
# include <sys/resource.h>

void	*malloc(size_t size);
void	free(void *ptr);
void	*realloc(void *ptr, size_t size);

// coalesce blocks

typedef struct		s_block
{
	size_t			size;
	struct s_block	*prev;
	struct s_block	*next;
}					t_block;

typedef struct
{
	char			*name;
	size_t			max_size;
}					t_size_class;

void	show_alloc_mem(void);

#define ALIGNMENT 8
#define CHUNK_SIZE 1024

#endif
