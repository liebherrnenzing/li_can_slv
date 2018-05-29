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
 * @file li_can_slv_mem.h
 * @brief header to li_can_slv_mem.h module
 * @addtogroup can_memory
 * @{
 */

#ifndef UTIL_LI_CAN_SLV_MEM_H_
#define UTIL_LI_CAN_SLV_MEM_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include "io_can_types.h"

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
/// The size of a memory block in bytes.
#define CANARD_MEM_BLOCK_SIZE                       32

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/
/**
 * INTERNAL DEFINITION, DO NOT USE DIRECTLY.
 * A memory block used in the memory block allocator.
 */
typedef union li_can_slv_pool_allocator_block_u
{
	char bytes[CANARD_MEM_BLOCK_SIZE];
	union li_can_slv_pool_allocator_block_u *next;
} li_can_slv_pool_allocator_block;

/**
 * This structure provides usage statistics of the memory pool allocator.
 * This data helps to evaluate whether the allocated memory is sufficient for the application.
 */
typedef struct
{
	uint16_t capacity_blocks;               /**< Pool capacity in number of blocks */
	uint16_t current_usage_blocks;          /**< Number of blocks that are currently allocated by the library */
	uint16_t peak_usage_blocks;             /**< Maximum number of blocks used since initialization */
} li_can_slv_pool_allocator_stat;

/**
 * INTERNAL DEFINITION, DO NOT USE DIRECTLY.
 */
typedef struct
{
	li_can_slv_pool_allocator_block *free_list;
	li_can_slv_pool_allocator_stat statistics;
} li_can_slv_pool_allocator_t;

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
void li_can_slv_init_pool_allocator(li_can_slv_pool_allocator_t *allocator, li_can_slv_pool_allocator_block *buf, uint16_t buf_len);
void *li_can_slv_allocate_block(li_can_slv_pool_allocator_t *allocator);
void li_can_slv_free_block(li_can_slv_pool_allocator_t *allocator, void *p);

#ifdef __cplusplus
}             // closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef UTIL_LI_CAN_SLV_MEM_H_

/** @} */
