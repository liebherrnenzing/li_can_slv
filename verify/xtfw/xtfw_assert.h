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

/*
 * xtfw_assert.h
 *
 */

#ifndef XTFW_ASSERT_H_
#define XTFW_ASSERT_H_

#include "xtfw_adapter.h"

//-------------------------------------------------------
// basics, fail and ignore
//-------------------------------------------------------
#define XTFW_MESSAGE(message)																	__XTFW_ADAPTER_TEST_MESSAGE(message)
#define XTFW_FAIL_MESSAGE(message)																__XTFW_ADAPTER_TEST_FAIL_MESSAGE(message)
#define XTFW_FAIL()																				__XTFW_ADAPTER_TEST_FAIL()
#define XTFW_IGNORE_MESSAGE(message)															__XTFW_ADAPTER_TEST_IGNORE_MESSAGE(message)
#define XTFW_IGNORE()																			__XTFW_ADAPTER_TEST_IGNORE()

//-------------------------------------------------------
// test asserts (simple)
//-------------------------------------------------------

// boolean
#define XTFW_ASSERT(condition)																	__XTFW_ADAPTER_TEST_ASSERT(condition)
#define XTFW_ASSERT_TRUE(condition)																__XTFW_ADAPTER_TEST_ASSERT_TRUE(condition)
#define XTFW_ASSERT_UNLESS(condition)															__XTFW_ADAPTER_TEST_ASSERT_UNLESS(condition)
#define XTFW_ASSERT_FALSE(condition)															__XTFW_ADAPTER_TEST_ASSERT_FALSE(condition)
#define XTFW_ASSERT_NULL(pointer)																__XTFW_ADAPTER_TEST_ASSERT_NULL(pointer)
#define XTFW_ASSERT_NOT_NULL(pointer)															__XTFW_ADAPTER_TEST_ASSERT_NOT_NULL(pointer)

// integers (of all sizes)
#define XTFW_ASSERT_EQUAL_INT(expected, actual)													__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT(expected, actual)
#define XTFW_ASSERT_EQUAL_INT8(expected, actual)												__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT8(expected, actual)
#define XTFW_ASSERT_EQUAL_INT16(expected, actual)												__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT16(expected, actual)
#define XTFW_ASSERT_EQUAL_INT32(expected, actual)												__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT32(expected, actual)
#define XTFW_ASSERT_EQUAL_INT64(expected, actual)												__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT64(expected, actual)
#define XTFW_ASSERT_EQUAL(expected, actual)														__XTFW_ADAPTER_TEST_ASSERT_EQUAL(expected, actual)
#define XTFW_ASSERT_NOT_EQUAL(expected, actual)													__XTFW_ADAPTER_TEST_ASSERT_NOT_EQUAL(expected, actual)
#define XTFW_ASSERT_EQUAL_UINT(expected, actual)												__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT(expected, actual)
#define XTFW_ASSERT_EQUAL_UINT8(expected, actual)												__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT8(expected, actual)
#define XTFW_ASSERT_EQUAL_UINT16(expected, actual)												__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT16(expected, actual)
#define XTFW_ASSERT_EQUAL_UINT32(expected, actual)												__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT32(expected, actual)
#define XTFW_ASSERT_EQUAL_UINT64(expected, actual)												__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT64(expected, actual)
#define XTFW_ASSERT_EQUAL_HEX(expected, actual)													__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX(expected, actual)
#define XTFW_ASSERT_EQUAL_HEX8(expected, actual)												__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX8(expected, actual)
#define XTFW_ASSERT_EQUAL_HEX16(expected, actual)												__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX16(expected, actual)
#define XTFW_ASSERT_EQUAL_HEX32(expected, actual)												__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX32(expected, actual)
#define XTFW_ASSERT_EQUAL_HEX64(expected, actual)												__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX64(expected, actual)
#define XTFW_ASSERT_BITS(mask, expected, actual)												__XTFW_ADAPTER_TEST_ASSERT_BITS(mask, expected, actual)
#define XTFW_ASSERT_BITS_HIGH(mask, actual)														__XTFW_ADAPTER_TEST_ASSERT_BITS_HIGH(mask, actual)
#define XTFW_ASSERT_BITS_LOW(mask, actual)														__XTFW_ADAPTER_TEST_ASSERT_BITS_LOW(mask, actual)
#define XTFW_ASSERT_BIT_HIGH(bit, actual)														__XTFW_ADAPTER_TEST_ASSERT_BIT_HIGH(bit, actual)
#define XTFW_ASSERT_BIT_LOW(bit, actual)														__XTFW_ADAPTER_TEST_ASSERT_BIT_LOW(bit, actual)

// integer ranges (of all sizes)
#define XTFW_ASSERT_INT_WITHIN(delta, expected, actual)											__XTFW_ADAPTER_TEST_ASSERT_INT_WITHIN(delta, expected, actual)
#define XTFW_ASSERT_UINT_WITHIN(delta, expected, actual)										__XTFW_ADAPTER_TEST_ASSERT_UINT_WITHIN(delta, expected, actual)
#define XTFW_ASSERT_HEX_WITHIN(delta, expected, actual)											__XTFW_ADAPTER_TEST_ASSERT_HEX_WITHIN(delta, expected, actual)
#define XTFW_ASSERT_HEX8_WITHIN(delta, expected, actual)										__XTFW_ADAPTER_TEST_ASSERT_HEX8_WITHIN(delta, expected, actual)
#define XTFW_ASSERT_HEX16_WITHIN(delta, expected, actual)										__XTFW_ADAPTER_TEST_ASSERT_HEX16_WITHIN(delta, expected, actual)
#define XTFW_ASSERT_HEX32_WITHIN(delta, expected, actual)										__XTFW_ADAPTER_TEST_ASSERT_HEX32_WITHIN(delta, expected, actual)
#define XTFW_ASSERT_HEX64_WITHIN(delta, expected, actual)										__XTFW_ADAPTER_TEST_ASSERT_HEX64_WITHIN(delta, expected, actual)

// ptr, strings and memory
#define XTFW_ASSERT_EQUAL_PTR(expected, actual)													__XTFW_ADAPTER_TEST_ASSERT_EQUAL_PTR(expected, actual)
#define XTFW_ASSERT_EQUAL_STRING(expected, actual)												__XTFW_ADAPTER_TEST_ASSERT_EQUAL_STRING(expected, actual)
#define XTFW_ASSERT_EQUAL_MEMORY(expected, actual, len)											__XTFW_ADAPTER_TEST_ASSERT_EQUAL_MEMORY(expected, actual, len)

// arrays
#define XTFW_ASSERT_EQUAL_INT_ARRAY(expected, actual, num_elements)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, num_elements)
#define XTFW_ASSERT_EQUAL_INT8_ARRAY(expected, actual, num_elements)							__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT8_ARRAY(expected, actual, num_elements)
#define XTFW_ASSERT_EQUAL_INT16_ARRAY(expected, actual, num_elements)							__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT16_ARRAY(expected, actual, num_elements)
#define XTFW_ASSERT_EQUAL_INT32_ARRAY(expected, actual, num_elements)							__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT32_ARRAY(expected, actual, num_elements)
#define XTFW_ASSERT_EQUAL_INT64_ARRAY(expected, actual, num_elements)							__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT64_ARRAY(expected, actual, num_elements)
#define XTFW_ASSERT_EQUAL_UINT_ARRAY(expected, actual, num_elements)							__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT_ARRAY(expected, actual, num_elements)
#define XTFW_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, num_elements)							__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, num_elements)
#define XTFW_ASSERT_EQUAL_UINT16_ARRAY(expected, actual, num_elements)							__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT16_ARRAY(expected, actual, num_elements)
#define XTFW_ASSERT_EQUAL_UINT32_ARRAY(expected, actual, num_elements)							__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT32_ARRAY(expected, actual, num_elements)
#define XTFW_ASSERT_EQUAL_UINT64_ARRAY(expected, actual, num_elements)							__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, actual, num_elements)
#define XTFW_ASSERT_EQUAL_HEX_ARRAY(expected, actual, num_elements)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX_ARRAY(expected, actual, num_elements)
#define XTFW_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, num_elements)							__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, num_elements)
#define XTFW_ASSERT_EQUAL_HEX16_ARRAY(expected, actual, num_elements)							__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX16_ARRAY(expected, actual, num_elements)
#define XTFW_ASSERT_EQUAL_HEX32_ARRAY(expected, actual, num_elements)							__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, actual, num_elements)
#define XTFW_ASSERT_EQUAL_HEX64_ARRAY(expected, actual, num_elements)							__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX64_ARRAY(expected, actual, num_elements)
#define XTFW_ASSERT_EQUAL_PTR_ARRAY(expected, actual, num_elements)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_PTR_ARRAY(expected, actual, num_elements)
#define XTFW_ASSERT_EQUAL_STRING_ARRAY(expected, actual, num_elements)							__XTFW_ADAPTER_TEST_ASSERT_EQUAL_STRING_ARRAY(expected, actual, num_elements)
#define XTFW_ASSERT_EQUAL_MEMORY_ARRAY(expected, actual, len, num_elements)						__XTFW_ADAPTER_TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, actual, len, num_elements)

// floating point
#ifdef XTFW_FLOAT_POINT
#define XTFW_ASSERT_FLOAT_WITHIN(delta, expected, actual)										__XTFW_ADAPTER_TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)
#define XTFW_ASSERT_EQUAL_FLOAT(expected, actual)												__XTFW_ADAPTER_TEST_ASSERT_EQUAL_FLOAT(expected, actual)
#define XTFW_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, num_elements)							__XTFW_ADAPTER_TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, num_elements)

#define XTFW_ASSERT_FLOAT_IS_INF(actual)														__XTFW_ADAPTER_TEST_ASSERT_FLOAT_IS_INF(actual)
#define XTFW_ASSERT_FLOAT_IS_NEG_INF(actual)													__XTFW_ADAPTER_TEST_ASSERT_FLOAT_IS_NEG_INF(actual)
#define XTFW_ASSERT_FLOAT_IS_NAN(actual)														__XTFW_ADAPTER_TEST_ASSERT_FLOAT_IS_NAN(actual)
#endif // #ifdef XTFW_FLOAT_POINT

// double pint
#ifdef XTFW_DOUBLE_POINT
#define XTFW_ASSERT_DOUBLE_WITHIN(delta, expected, actual)										__XTFW_ADAPTER_TEST_ASSERT_DOUBLE_WITHIN(delta, expected, actual)
#define XTFW_ASSERT_EQUAL_DOUBLE(expected, actual)												__XTFW_ADAPTER_TEST_ASSERT_EQUAL_DOUBLE(expected, actual)
#define XTFW_ASSERT_EQUAL_DOUBLE_ARRAY(expected, actual, num_elements)							__XTFW_ADAPTER_TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, actual, num_elements)

#define XTFW_ASSERT_DOUBLE_IS_INF(actual)														__XTFW_ADAPTER_TEST_ASSERT_DOUBLE_IS_INF(actual)
#define XTFW_ASSERT_DOUBLE_IS_NEG_INF(actual)													__XTFW_ADAPTER_TEST_ASSERT_DOUBLE_IS_NEG_INF(actual)
#define XTFW_ASSERT_DOUBLE_IS_NAN(actual)														__XTFW_ADAPTER_TEST_ASSERT_DOUBLE_IS_NAN(actual)
#endif // #ifdef XTFW_DOUBLE_POINT

//-------------------------------------------------------
// test asserts (with additional messages)
//-------------------------------------------------------
#ifdef XTFW_WITH_MESSAGE

// boolean
#define XTFW_ASSERT_MESSAGE(condition, message)													__XTFW_ADAPTER_TEST_ASSERT_MESSAGE(condition, message)
#define XTFW_ASSERT_TRUE_MESSAGE(condition, message)											__XTFW_ADAPTER_TEST_ASSERT_TRUE_MESSAGE(condition, message)
#define XTFW_ASSERT_UNLESS_MESSAGE(condition, message)											__XTFW_ADAPTER_TEST_ASSERT_UNLESS_MESSAGE(condition, message)
#define XTFW_ASSERT_FALSE_MESSAGE(condition, message)											__XTFW_ADAPTER_TEST_ASSERT_FALSE_MESSAGE(condition, message)
#define XTFW_ASSERT_NULL_MESSAGE(pointer, message)												__XTFW_ADAPTER_TEST_ASSERT_NULL_MESSAGE(pointer, message)
#define XTFW_ASSERT_NOT_NULL_MESSAGE(pointer, message)											__XTFW_ADAPTER_TEST_ASSERT_NOT_NULL_MESSAGE(pointer, message)

// integers (of all sizes)
#define XTFW_ASSERT_EQUAL_INT_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_INT8_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT8_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_INT16_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT16_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_INT32_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT32_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_INT64_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT64_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_MESSAGE(expected, actual, message)									__XTFW_ADAPTER_TEST_ASSERT_EQUAL_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_NOT_EQUAL_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_NOT_EQUAL_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_UINT_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_UINT8_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT8_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_UINT16_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT16_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_UINT32_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT32_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_UINT64_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT64_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_HEX_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_HEX8_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX8_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_HEX16_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX16_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_HEX32_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX32_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_HEX64_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX64_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_BITS_MESSAGE(mask, expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_BITS_MESSAGE(mask, expected, actual, message)
#define XTFW_ASSERT_BITS_HIGH_MESSAGE(mask, actual, message)									__XTFW_ADAPTER_TEST_ASSERT_BITS_HIGH_MESSAGE(mask, actual, message)
#define XTFW_ASSERT_BITS_LOW_MESSAGE(mask, actual, message)										__XTFW_ADAPTER_TEST_ASSERT_BITS_LOW_MESSAGE(mask, actual, message)
#define XTFW_ASSERT_BIT_HIGH_MESSAGE(bit, actual, message)										__XTFW_ADAPTER_TEST_ASSERT_BIT_HIGH_MESSAGE(bit, actual, message)
#define XTFW_ASSERT_BIT_LOW_MESSAGE(bit, actual, message)										__XTFW_ADAPTER_TEST_ASSERT_BIT_LOW_MESSAGE(bit, actual, message)

// integer ranges (of all sizes)
#define XTFW_ASSERT_INT_WITHIN_MESSAGE(delta, expected, actual, message)						__XTFW_ADAPTER_TEST_ASSERT_INT_WITHIN_MESSAGE(delta, expected, actual, message)
#define XTFW_ASSERT_UINT_WITHIN_MESSAGE(delta, expected, actual, message)						__XTFW_ADAPTER_TEST_ASSERT_UINT_WITHIN_MESSAGE(delta, expected, actual, message)
#define XTFW_ASSERT_HEX_WITHIN_MESSAGE(delta, expected, actual, message)						__XTFW_ADAPTER_TEST_ASSERT_HEX_WITHIN_MESSAGE(delta, expected, actual, message)
#define XTFW_ASSERT_HEX8_WITHIN_MESSAGE(delta, expected, actual, message)						__XTFW_ADAPTER_TEST_ASSERT_HEX8_WITHIN_MESSAGE(delta, expected, actual, message)
#define XTFW_ASSERT_HEX16_WITHIN_MESSAGE(delta, expected, actual, message)						__XTFW_ADAPTER_TEST_ASSERT_HEX16_WITHIN_MESSAGE(delta, expected, actual, message)
#define XTFW_ASSERT_HEX32_WITHIN_MESSAGE(delta, expected, actual, message)						__XTFW_ADAPTER_TEST_ASSERT_HEX32_WITHIN_MESSAGE(delta, expected, actual, message)
#define XTFW_ASSERT_HEX64_WITHIN_MESSAGE(delta, expected, actual, message)						__XTFW_ADAPTER_TEST_ASSERT_HEX64_WITHIN_MESSAGE(delta, expected, actual, message)

// ptr, strings and memory
#define XTFW_ASSERT_EQUAL_PTR_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_PTR_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_STRING_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_MEMORY_MESSAGE(expected, actual, len, message)						__XTFW_ADAPTER_TEST_ASSERT_EQUAL_MEMORY_MESSAGE(expected, actual, len, message)

// arrays
#define XTFW_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, num_elements, message)
#define XTFW_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(expected, actual, num_elements, message)
#define XTFW_ASSERT_EQUAL_INT16_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT16_ARRAY_MESSAGE(expected, actual, num_elements, message)
#define XTFW_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(expected, actual, num_elements, message)
#define XTFW_ASSERT_EQUAL_INT64_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT64_ARRAY_MESSAGE(expected, actual, num_elements, message)
#define XTFW_ASSERT_EQUAL_UINT_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT_ARRAY_MESSAGE(expected, actual, num_elements, message)
#define XTFW_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected, actual, num_elements, message)
#define XTFW_ASSERT_EQUAL_UINT16_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT16_ARRAY_MESSAGE(expected, actual, num_elements, message)
#define XTFW_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(expected, actual, num_elements, message)
#define XTFW_ASSERT_EQUAL_UINT64_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT64_ARRAY_MESSAGE(expected, actual, num_elements, message)
#define XTFW_ASSERT_EQUAL_HEX_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX_ARRAY_MESSAGE(expected, actual, num_elements, message)
#define XTFW_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(expected, actual, num_elements, message)
#define XTFW_ASSERT_EQUAL_HEX16_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX16_ARRAY_MESSAGE(expected, actual, num_elements, message)
#define XTFW_ASSERT_EQUAL_HEX32_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX32_ARRAY_MESSAGE(expected, actual, num_elements, message)
#define XTFW_ASSERT_EQUAL_HEX64_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX64_ARRAY_MESSAGE(expected, actual, num_elements, message)
#define XTFW_ASSERT_EQUAL_PTR_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_PTR_ARRAY_MESSAGE(expected, actual, num_elements, message)
#define XTFW_ASSERT_EQUAL_STRING_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_STRING_ARRAY_MESSAGE(expected, actual, num_elements, message)
#define XTFW_ASSERT_EQUAL_MEMORY_ARRAY_MESSAGE(expected, actual, len, num_elements, message)	__XTFW_ADAPTER_TEST_ASSERT_EQUAL_MEMORY_ARRAY_MESSAGE(expected, actual, len, num_elements, message)

// floating point
#ifdef XTFW_FLOAT_POINT
#define XTFW_ASSERT_FLOAT_WITHIN_MESSAGE(delta, expected, actual, message)						__XTFW_ADAPTER_TEST_ASSERT_FLOAT_WITHIN_MESSAGE(delta, expected, actual, message)
#define XTFW_ASSERT_EQUAL_FLOAT_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_FLOAT_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(expected, actual, num_elements, message)

#define XTFW_ASSERT_FLOAT_IS_INF_MESSAGE(actual, message)										__XTFW_ADAPTER_TEST_ASSERT_FLOAT_IS_INF_MESSAGE(actual, message)
#define XTFW_ASSERT_FLOAT_IS_NEG_INF_MESSAGE(actual, message)									__XTFW_ADAPTER_TEST_ASSERT_FLOAT_IS_NEG_INF_MESSAGE(actual, message)
#define XTFW_ASSERT_FLOAT_IS_NAN_MESSAGE(actual, message)										__XTFW_ADAPTER_TEST_ASSERT_FLOAT_IS_NAN_MESSAGE(actual, message)
#endif // #ifdef XTFW_FLOAT_POINT

// double point
#ifdef XTFW_DOUBLE_POINT
#define XTFW_ASSERT_DOUBLE_WITHIN_MESSAGE(delta, expected, actual, message)						__XTFW_ADAPTER_TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(delta, expected, actual, message)
#define XTFW_ASSERT_EQUAL_DOUBLE_MESSAGE(expected, actual, message)								__XTFW_ADAPTER_TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(expected, actual, message)
#define XTFW_ASSERT_EQUAL_DOUBLE_ARRAY_MESSAGE(expected, actual, num_elements, message)			__XTFW_ADAPTER_TEST_ASSERT_EQUAL_DOUBLE_ARRAY_MESSAGE(expected, actual, num_elements, message)

#define XTFW_ASSERT_DOUBLE_IS_INF_MESSAGE(actual, message)										__XTFW_ADAPTER_TEST_ASSERT_DOUBLE_IS_INF_MESSAGE(actual, message)
#define XTFW_ASSERT_DOUBLE_IS_NEG_INF_MESSAGE(actual, message)									__XTFW_ADAPTER_TEST_ASSERT_DOUBLE_IS_NEG_INF_MESSAGE(actual, message)
#define XTFW_ASSERT_DOUBLE_IS_NAN_MESSAGE(actual, message)										__XTFW_ADAPTER_TEST_ASSERT_DOUBLE_IS_NAN_MESSAGE(actual, message)
#endif // #ifdef XTFW_DOUBLE_POINT

#endif // #ifdef XTFW_WITH_MESSAGE

#endif /* XTFW_ASSERT_H_ */


