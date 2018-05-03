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
 * xtfw_adapter.h
 */

#ifndef XTFW_ADAPTER_H_
#define XTFW_ADAPTER_H_

#include "CUnit.h"

//-------------------------------------------------------
// basics, fail and ignore
//-------------------------------------------------------
#define NOT_IMPL											#warning Not Implemented
#define __XTFW_ADAPTER_TEST_MESSAGE(message)				CU_PASS(message)
#define __XTFW_ADAPTER_TEST_FAIL_MESSAGE(message)			NOT_IMPL
#define __XTFW_ADAPTER_TEST_FAIL()							CU_FAIL()
#define __XTFW_ADAPTER_TEST_IGNORE_MESSAGE(message)			NOT_IMPL
#define __XTFW_ADAPTER_TEST_IGNORE()						NOT_IMPL

//-------------------------------------------------------
// test asserts (simple)
//-------------------------------------------------------

// boolean
#define __XTFW_ADAPTER_TEST_ASSERT(condition)			CU_ASSERT(condition)
#define __XTFW_ADAPTER_TEST_ASSERT_TRUE(condition)      CU_ASSERT_TRUE(condition)
#define __XTFW_ADAPTER_TEST_ASSERT_UNLESS(condition)    NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_FALSE(condition)     CU_ASSERT_FALSE(condition)
#define __XTFW_ADAPTER_TEST_ASSERT_NULL(pointer)        CU_ASSERT_PTR_NULL(pointer)
#define __XTFW_ADAPTER_TEST_ASSERT_NOT_NULL(pointer)    CU_ASSERT_PTR_NOT_NULL(pointer)

// integers (of all sizes) // Always use equal cause equal just use preprocessor. The preprocessor calculates the value of expression.
// f.e.: It carries out all calculations in the widest integer type known to the compiler; on most machines supported
// by GCC this is 64 bits
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT(expected, actual)				CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT8(expected, actual)             CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT16(expected, actual)            CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT32(expected, actual)            CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT64(expected, actual)            CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL(expected, actual)                  CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_NOT_EQUAL(expected, actual)              CU_ASSERT_NOT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT(expected, actual)             CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT8(expected, actual)            CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT16(expected, actual)           CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT32(expected, actual)           CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT64(expected, actual)           CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX(expected, actual)              CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX8(expected, actual)             CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX16(expected, actual)            CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX32(expected, actual)            CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX64(expected, actual)            CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_BITS(mask, expected, actual)             NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_BITS_HIGH(mask, actual)                  NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_BITS_LOW(mask, actual)                   NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_BIT_HIGH(bit, actual)                    NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_BIT_LOW(bit, actual)                     NOT_IMPL

// integer ranges (of all sizes)
#define __XTFW_ADAPTER_TEST_ASSERT_INT_WITHIN(delta, expected, actual)		CU_ASSERT_DOUBLE_EQUAL(actual, expected, delta)
#define __XTFW_ADAPTER_TEST_ASSERT_UINT_WITHIN(delta, expected, actual)     CU_ASSERT_DOUBLE_EQUAL(actual, expected, delta)
#define __XTFW_ADAPTER_TEST_ASSERT_HEX_WITHIN(delta, expected, actual)      CU_ASSERT_DOUBLE_EQUAL(actual, expected, delta)
#define __XTFW_ADAPTER_TEST_ASSERT_HEX8_WITHIN(delta, expected, actual)     CU_ASSERT_DOUBLE_EQUAL(actual, expected, delta)
#define __XTFW_ADAPTER_TEST_ASSERT_HEX16_WITHIN(delta, expected, actual)    CU_ASSERT_DOUBLE_EQUAL(actual, expected, delta)
#define __XTFW_ADAPTER_TEST_ASSERT_HEX32_WITHIN(delta, expected, actual)    CU_ASSERT_DOUBLE_EQUAL(actual, expected, delta)
#define __XTFW_ADAPTER_TEST_ASSERT_HEX64_WITHIN(delta, expected, actual)    CU_ASSERT_DOUBLE_EQUAL(actual, expected, delta)

// ptr, strings and memory
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_PTR(expected, actual)				ASSERT_PTR_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_STRING(expected, actual)           TEST_ASSERT_EQUAL_STRING(expected, actual)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_MEMORY(expected, actual, len)      NOT_IMPL

// arrays
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, num_elements)				NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT8_ARRAY(expected, actual, num_elements)             NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT16_ARRAY(expected, actual, num_elements)            NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT32_ARRAY(expected, actual, num_elements)            NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT64_ARRAY(expected, actual, num_elements)            NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT_ARRAY(expected, actual, num_elements)             NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, num_elements)            NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT16_ARRAY(expected, actual, num_elements)           NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT32_ARRAY(expected, actual, num_elements)           NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, actual, num_elements)           NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX_ARRAY(expected, actual, num_elements)              NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, num_elements)             NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX16_ARRAY(expected, actual, num_elements)            NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, actual, num_elements)            NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX64_ARRAY(expected, actual, num_elements)            NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_PTR_ARRAY(expected, actual, num_elements)              NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_STRING_ARRAY(expected, actual, num_elements)           NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, actual, len, num_elements)      NOT_IMPL

// floating point
#ifdef XTFW_FLOAT_POINT
#define __XTFW_ADAPTER_TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)					CU_ASSERT_DOUBLE_EQUAL(actual, expected, delta)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_FLOAT(expected, actual)                            NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, num_elements)        NOT_IMPL

#define __XTFW_ADAPTER_TEST_ASSERT_FLOAT_IS_INF(actual)										NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_FLOAT_IS_NEG_INF(actual)									NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_FLOAT_IS_NAN(actual)										NOT_IMPL
#endif // #ifdef XTFW_FLOAT_POINT

// double pint
#ifdef XTFW_DOUBLE_POINT
#define __XTFW_ADAPTER_TEST_ASSERT_DOUBLE_WITHIN(delta, expected, actual)                 	CU_ASSERT_DOUBLE_EQUAL(actual, expected, delta)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_DOUBLE(expected, actual)                         	NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, actual, num_elements)     	NOT_IMPL

#define __XTFW_ADAPTER_TEST_ASSERT_DOUBLE_IS_INF(actual)									NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_DOUBLE_IS_NEG_INF(actual)                                NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_DOUBLE_IS_NAN(actual)                                    NOT_IMPL
#endif // #ifdef XTFW_DOUBLE_POINT

//-------------------------------------------------------
// test asserts (with additional messages)
//-------------------------------------------------------
#ifdef XTFW_WITH_MESSAGE

// boolean
#define __XTFW_ADAPTER_TEST_ASSERT_MESSAGE(condition, message)				TEST_ASSERT_MESSAGE(condition, message)
#define __XTFW_ADAPTER_TEST_ASSERT_TRUE_MESSAGE(condition, message)         TEST_ASSERT_TRUE_MESSAGE(condition, message)
#define __XTFW_ADAPTER_TEST_ASSERT_UNLESS_MESSAGE(condition, message)       TEST_ASSERT_UNLESS_MESSAGE(condition, message)
#define __XTFW_ADAPTER_TEST_ASSERT_FALSE_MESSAGE(condition, message)        TEST_ASSERT_FALSE_MESSAGE(condition, message)
#define __XTFW_ADAPTER_TEST_ASSERT_NULL_MESSAGE(pointer, message)           TEST_ASSERT_NULL_MESSAGE(pointer, message)
#define __XTFW_ADAPTER_TEST_ASSERT_NOT_NULL_MESSAGE(pointer, message)       TEST_ASSERT_NOT_NULL_MESSAGE(pointer, message)

// integers (of all sizes)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, message)				CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT8_MESSAGE(expected, actual, message)            CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT16_MESSAGE(expected, actual, message)           CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT32_MESSAGE(expected, actual, message)           CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT64_MESSAGE(expected, actual, message)           CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_MESSAGE(expected, actual, message)                 CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_NOT_EQUAL_MESSAGE(expected, actual, message)             CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT_MESSAGE(expected, actual, message)            CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT8_MESSAGE(expected, actual, message)           CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT16_MESSAGE(expected, actual, message)          CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT32_MESSAGE(expected, actual, message)          CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT64_MESSAGE(expected, actual, message)          CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX_MESSAGE(expected, actual, message)             CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX8_MESSAGE(expected, actual, message)            CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX16_MESSAGE(expected, actual, message)           CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX32_MESSAGE(expected, actual, message)           CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX64_MESSAGE(expected, actual, message)           CU_ASSERT_EQUAL(actual, expected)
#define __XTFW_ADAPTER_TEST_ASSERT_BITS_MESSAGE(mask, expected, actual, message)            NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_BITS_HIGH_MESSAGE(mask, actual, message)                 NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_BITS_LOW_MESSAGE(mask, actual, message)                  NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_BIT_HIGH_MESSAGE(bit, actual, message)                   NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_BIT_LOW_MESSAGE(bit, actual, message)                    NOT_IMPL

// integer ranges (of all sizes)
#define __XTFW_ADAPTER_TEST_ASSERT_INT_WITHIN_MESSAGE(delta, expected, actual, message)		NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_UINT_WITHIN_MESSAGE(delta, expected, actual, message)    NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_HEX_WITHIN_MESSAGE(delta, expected, actual, message)     NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_HEX8_WITHIN_MESSAGE(delta, expected, actual, message)    NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_HEX16_WITHIN_MESSAGE(delta, expected, actual, message)   NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_HEX32_WITHIN_MESSAGE(delta, expected, actual, message)   NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_HEX64_WITHIN_MESSAGE(delta, expected, actual, message)   NOT_IMPL

// ptr, strings and memory
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_PTR_MESSAGE(expected, actual, message)				NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, actual, message)          NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_MEMORY_MESSAGE(expected, actual, len, message)     NOT_IMPL

// arrays
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, num_elements, message)			NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(expected, actual, num_elements, message)		NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT16_ARRAY_MESSAGE(expected, actual, num_elements, message)		NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(expected, actual, num_elements, message)		NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_INT64_ARRAY_MESSAGE(expected, actual, num_elements, message)		NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT_ARRAY_MESSAGE(expected, actual, num_elements, message)		NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected, actual, num_elements, message)		NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT16_ARRAY_MESSAGE(expected, actual, num_elements, message)		NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(expected, actual, num_elements, message)		NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_UINT64_ARRAY_MESSAGE(expected, actual, num_elements, message)		NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX_ARRAY_MESSAGE(expected, actual, num_elements, message)			NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(expected, actual, num_elements, message)		NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX16_ARRAY_MESSAGE(expected, actual, num_elements, message)		NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX32_ARRAY_MESSAGE(expected, actual, num_elements, message)		NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_HEX64_ARRAY_MESSAGE(expected, actual, num_elements, message)		NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_PTR_ARRAY_MESSAGE(expected, actual, num_elements, message)			NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_STRING_ARRAY_MESSAGE(expected, actual, num_elements, message)		NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_MEMORY_ARRAY_MESSAGE(expected, actual, len, num_elements, message)	NOT_IMPL

// floating point
#ifdef XTFW_FLOAT_POINT
#define __XTFW_ADAPTER_TEST_ASSERT_FLOAT_WITHIN_MESSAGE(delta, expected, actual, message)					NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_FLOAT_MESSAGE(expected, actual, message)							NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(expected, actual, num_elements, message)		NOT_IMPL

#define __XTFW_ADAPTER_TEST_ASSERT_FLOAT_IS_INF_MESSAGE(actual, message)									NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_FLOAT_IS_NEG_INF_MESSAGE(actual, message)								NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_FLOAT_IS_NAN_MESSAGE(actual, message)									NOT_IMPL
#endif // #ifdef XTFW_FLOAT_POINT

// double point
#ifdef XTFW_DOUBLE_POINT
#define __XTFW_ADAPTER_TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(delta, expected, actual, message)				NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(expected, actual, message)                    	NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_EQUAL_DOUBLE_ARRAY_MESSAGE(expected, actual, num_elements, message)	NOT_IMPL

#define __XTFW_ADAPTER_TEST_ASSERT_DOUBLE_IS_INF_MESSAGE(actual, message)								NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_DOUBLE_IS_NEG_INF_MESSAGE(actual, message)							NOT_IMPL
#define __XTFW_ADAPTER_TEST_ASSERT_DOUBLE_IS_NAN_MESSAGE(actual, message)								NOT_IMPL
#endif // #ifdef XTFW_DOUBLE_POINT

#endif // #ifdef XTFW_WITH_MESSAGE

#endif /* XTFW_ADAPTER_H_ */
