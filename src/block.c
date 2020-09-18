/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   block.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charles <me@cacharle.xyz>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/18 15:46:15 by charles           #+#    #+#             */
/*   Updated: 2020/09/18 15:50:17 by charles          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

void	block_set_size(t_block *block, size_t new_size)
{
	block->size = new_size;
	*((uint8_t*)block + block->size - sizeof(size_t)) = block->size;
}
