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
 * @file li_can_slv_selftest.c
 * @addtogroup uinttest
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "unity_config.h"
#include "unity.h"
#include "xtfw.h"

#include "li_can_slv_mem.h"

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#define AVAILABLE_BLOCKS 3

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/
li_can_slv_pool_allocator_t allocator;
li_can_slv_pool_allocator_block buffer[AVAILABLE_BLOCKS];

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
// setUp will be called before each test
void setUp(void)
{
	li_can_slv_init_pool_allocator(&allocator, buffer, AVAILABLE_BLOCKS);
}

// tearDown will be called after each test
void tearDown(void)
{

}

void test_mem_init(void)
{
	// Check that the memory list is constructed correctly.
	XTFW_ASSERT_EQUAL_PTR(&buffer[0], allocator.free_list);
	XTFW_ASSERT_EQUAL_PTR(&buffer[1], allocator.free_list->next);
	XTFW_ASSERT_EQUAL_PTR(&buffer[2], allocator.free_list->next->next);
	XTFW_ASSERT_EQUAL_PTR(NULL, allocator.free_list->next->next->next);

	// Check statistics
	XTFW_ASSERT_EQUAL(AVAILABLE_BLOCKS, allocator.statistics.capacity_blocks);
	XTFW_ASSERT_EQUAL(0, allocator.statistics.current_usage_blocks);
	XTFW_ASSERT_EQUAL(0, allocator.statistics.peak_usage_blocks);
}

void test_mem_alloc(void)
{
	void *block = li_can_slv_allocate_block(&allocator);

	// Check that the first free memory block was used and that the next block is ready.
	XTFW_ASSERT_EQUAL_PTR(&buffer[0], block);
	XTFW_ASSERT_EQUAL_PTR(&buffer[1], allocator.free_list);

	// Check statistics
	XTFW_ASSERT_EQUAL(AVAILABLE_BLOCKS, allocator.statistics.capacity_blocks);
	XTFW_ASSERT_EQUAL(1, allocator.statistics.current_usage_blocks);
	XTFW_ASSERT_EQUAL(1, allocator.statistics.peak_usage_blocks);
}

void test_mem_free(void)
{
	void *block = li_can_slv_allocate_block(&allocator);

	li_can_slv_free_block(&allocator, block);

	// Check that the block was added back to the beginning
	XTFW_ASSERT_EQUAL_PTR(&buffer[0], allocator.free_list);
	XTFW_ASSERT_EQUAL_PTR(&buffer[1], allocator.free_list->next);

	// Check statistics
	XTFW_ASSERT_EQUAL(AVAILABLE_BLOCKS, allocator.statistics.capacity_blocks);
	XTFW_ASSERT_EQUAL(0, allocator.statistics.current_usage_blocks);
	XTFW_ASSERT_EQUAL(1, allocator.statistics.peak_usage_blocks);
}

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/


/** @} */
