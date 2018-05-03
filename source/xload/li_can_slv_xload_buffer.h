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
 * @file li_can_slv_xload_buffer.h
 * @brief header to li_can_slv_xload_buffer.h module
 * @addtogroup lcs_xload_buffer
 * @{
 */

#ifndef LI_CAN_SLV_XLOAD_BUFFER_H_
#define LI_CAN_SLV_XLOAD_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include "li_can_slv.h"
#include "io_can_types.h"

#include "li_can_slv_xload_buffer_types.h"
#include "li_can_slv_error_types.h"

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/
/**
 * @todo remove global variables
 */
extern uint16_t dload_bytes_cnt;

#ifdef LI_CAN_SLV_DLOAD_BUFFER_INTERNAL
extern li_can_slv_xload_buffer_t dload_buffer;
#endif // #ifdef LI_CAN_SLV_DLOAD_BUFFER_INTERNAL

extern uint16_t bytes_of_block_cnt; /**< bytes of block from CAN-bus */

extern uint16_t nr_of_can_objs_counter; /**< number of CAN objects from CAN-bus */

extern li_can_slv_xload_component_t xload_component; /**< details about the current component*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
/*!
 * \brief take a hex number definition from a string and convert it into a 32Bit number\n
 * if we detect a non hex character or end of string before the expected number of nibbles was converted, we return the hex number converted until now
 * \param hex_string zero terminated C string containing hex number\n
 * "0..9", "a..f", "A..F" are allowed
 * \param	nibbles number of nibbles/characters to be taken from string for conversion
 * \return	converted 32 Bit number (unsigned of course)
 */
extern uint32_t dload_hexchar_to_uint32_internal(const char *hex_string, uint16_t nibbles);

#ifndef dload_hexchar_to_uint32
/**
 * If no dload_hexchar_to_uint32 function has been set, the function @ref dload_hexchar_to_uint32_internal shall be used.
 * @sa @ref dload_hexchar_to_uint32_internal
 */
#define dload_hexchar_to_uint32(hex_str, n) dload_hexchar_to_uint32_internal(hex_str, n)
#endif // #ifndef dload_hexchar_to_uint32

#ifdef LI_CAN_SLV_DLOAD_BUFFER_INTERNAL
li_can_slv_errorcode_t dload_add_byte_to_buffer(byte_t byte);

li_can_slv_errorcode_t xload_buffer_get_byte(byte_t *data);
li_can_slv_errorcode_t dload_get_str_byte(byte_t *data);
li_can_slv_errorcode_t dload_get_str_uint16(uint16_t *data);
li_can_slv_errorcode_t dload_get_str_uint32(uint32_t *data);
li_can_slv_errorcode_t xload_buffer_get_len(uint16_t *nr_of_bytes);
li_can_slv_errorcode_t dload_store_buf_state(void);
li_can_slv_errorcode_t dload_restore_buf_state(void);
li_can_slv_errorcode_t dload_remove_bytes(uint16_t num);

#if defined(OUTER) || defined(OUTER_APP)
li_can_slv_errorcode_t dload_add_str_to_buffer(byte_t *src, uint16_t max_length);
#endif // #if defined(OUTER) || defined(OUTER_APP)
#endif // #ifdef LI_CAN_SLV_DLOAD_BUFFER_INTERNAL

#ifdef __cplusplus
} // closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef LI_CAN_SLV_XLOAD_BUFFER_H_
/** @} */

