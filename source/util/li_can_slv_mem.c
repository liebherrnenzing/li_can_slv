/****************************************************************************/
/*                                                                          */
/*                     Copyright (c) 2018, Liebherr PME1                    */
/*                         ALL RIGHTS RESERVED                              */
/*                                                                          */
/* This file is part of li_can_slv stack which is free software: you can    */
/* redistribute it and/or modify it under the terms of the GNU General      */
/* Public License as published by the Free Software Foundation, either      */
/* version 3 of the License, or (at your option) any later version.         */
/*                                                                          */
/* The li_can_slv stack is distributed in the hope that it will be useful,  */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General */
/* Public License for more details.                                         */
/*                                                                          */
/* You should have received a copy of the GNU General Public License which  */
/* should be located in the the root of the Stack. If not, contact Liebherr */
/* to obtain a copy.                                                        */
/****************************************************************************/

/**
 * @file li_can_slv_mem.c
 * @brief simple memory pool allocator
 * @addtogroup can_memory
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "li_can_slv_mem.h"
#include "io_can_port.h"
#include "li_can_slv_assert.h"

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
/*
 *  Pool Allocator functions
 */
void li_can_slv_init_pool_allocator(li_can_slv_pool_allocator_t *allocator, li_can_slv_pool_allocator_block *buf, uint16_t buf_len)
{
	size_t current_index = 0;
	li_can_slv_pool_allocator_block **current_block = &(allocator->free_list);
	while (current_index < buf_len)
	{
		*current_block = &buf[current_index];
		current_block = &((*current_block)->next);
		current_index++;
	}
	*current_block = NULL;

	allocator->statistics.capacity_blocks = buf_len;
	allocator->statistics.current_usage_blocks = 0;
	allocator->statistics.peak_usage_blocks = 0;
}

void *li_can_slv_allocate_block(li_can_slv_pool_allocator_t *allocator)
{
	// Check if there are any blocks available in the free list.
	if (allocator->free_list == NULL)
	{
		return NULL;
	}

	// Take first available block and prepares next block for use.
	void *result = allocator->free_list;
	allocator->free_list = allocator->free_list->next;

	// Update statistics
	allocator->statistics.current_usage_blocks++;
	if (allocator->statistics.peak_usage_blocks < allocator->statistics.current_usage_blocks)
	{
		allocator->statistics.peak_usage_blocks = allocator->statistics.current_usage_blocks;
	}

	return result;
}

void li_can_slv_free_block(li_can_slv_pool_allocator_t *allocator, void *p)
{
	li_can_slv_pool_allocator_block *block = (li_can_slv_pool_allocator_block *) p;

	block->next = allocator->free_list;
	allocator->free_list = block;

	LI_CAN_SLV_ASSERT(allocator->statistics.current_usage_blocks > 0u);
	allocator->statistics.current_usage_blocks--;
}

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/** @} */
