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
 * @file li_can_slv_assert.h
 * @brief header to li_can_slv_assert.h module
 * @addtogroup can_assert
 * @{
 */

#ifndef LI_CAN_SLV_ASSERT_H_
#define LI_CAN_SLV_ASSERT_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
#ifdef NDEBUG
#define LI_CAN_SLV_STATIC_ASSERT(e,m)
#define LI_CAN_SLV_ASSERT(cond)
#else // #ifndef NDEBUG
// Based on: http://www.pixelbeat.org/programming/gcc/static_assert.html
// LI_CAN_SLV_STATIC_ASSERT is for compile time assertions and LI_CAN_SLV_ASSERT
// is for runtime assertions.

#define LI_CAN_SLV_ASSERT_CONCAT_(a, b) a##b
#define LI_CAN_SLV_ASSERT_CONCAT(a, b) LI_CAN_SLV_ASSERT_CONCAT_(a, b)
/* These can't be used after statements in c89. */
#ifdef __COUNTER__
#define LI_CAN_SLV_STATIC_ASSERT(e,m) \
    ;enum { LI_CAN_SLV_ASSERT_CONCAT(static_assert_, __COUNTER__) = 1/(int)(!!(e)) }
#else
/* This can't be used twice on the same line so ensure if using in headers
 * that the headers are not included twice (by wrapping in #ifndef...#endif)
 * Note it doesn't cause an issue when used on same line of separate modules
 * compiled with gcc -combine -fwhole-program.  */
#define LI_CAN_SLV_STATIC_ASSERT(e,m) \
    ;enum { LI_CAN_SLV_ASSERT_CONCAT(assert_line_, __LINE__) = 1/(int)(!!(e)) }
#endif

/**
 * @brief LI_CAN_SLV_ASSERT is for runtime assertions.
 */
#define LI_CAN_SLV_ASSERT(cond) \
    if (cond) \
      { ; } \
    else \
      { li_can_slv_assert_failure(__FILE__, __LINE__); }

#endif // #ifndef NDEBUG

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
#ifndef NDEBUG
void li_can_slv_assert_failure(const char_t *file, int32_t line);
#endif // #ifndef NDEBUG

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef LI_CAN_SLV_ASSERT_H_

/** @} */
