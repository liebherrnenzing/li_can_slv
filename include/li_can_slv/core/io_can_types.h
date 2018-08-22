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
 * @file io_can_types.h
 * @brief header to io_can_types.h module
 * @addtogroup lcs_types
 * @{
 */

#ifndef IO_CAN_TYPES_H_
#define IO_CAN_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/core/li_can_slv.h>
#include "li_can_slv_port_types.h"

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/
#ifndef char_t
typedef char char_t; /*!< data type signed char (8 Bit) */
#define char_t char_t
#endif // #ifndef char_t

#ifndef uchar_t
typedef uint8_t uchar_t; /*!< data type unsigned char (8 Bit) */
#define uchar_t uchar_t
#endif // #ifndef uchar_t

#ifndef bit_t
typedef uint8_t bit_t; /*!< data type Bit (1 Bit) */
#define bit_t bit_t
#endif // #ifndef bit_t

#ifndef byte_t
typedef uint8_t byte_t; /*!< data type Byte (8 Bit) */
#define byte_t byte_t
#endif // #ifndef byte_t

#ifndef word_t
typedef uint16_t word_t; /*!< data type Word (16 Bit) */
#define word_t word_t
#endif // #ifndef word_t

#ifndef dword_t
typedef uint32_t dword_t; /*!< data type Double Word/Long Word (32 Bit) */
#define dword_t dword_t
#endif // #ifndef dword_t

#ifndef FALSE
#define FALSE			(0)		/*!< boolean FALSE statement */
#else
#if FALSE != 0
#error FALSE values do not match
#endif // #if FALSE != 0
#endif // #ifndef FALSE

#ifndef TRUE
#define TRUE			(1)		/*!< boolean TRUE statement */
#else
#if TRUE != 1
#error TRUE values do not match
#endif // #if TRUE != 1
#endif // #ifndef TRUE

typedef uint16_t li_can_slv_module_nr_t;  /**< module number type */

typedef char_t li_can_slv_module_type_t;  /**< module type type */

typedef struct
{
	uint8_t dlc;
	uint16_t id; /**< standard (11-bit) identifier */
	uint8_t data[8]; /**< 8-bit data bytes */
} li_can_slv_msg_obj_t;


/**
 * @typedef li_can_slv_mode_t
 * @brief defines the mode of the used can node
 */
typedef enum
{
	LI_CAN_SLV_MODE_INIT = 0,
	LI_CAN_SLV_MODE_OPERATIONAL,
	LI_CAN_SLV_MODE_LISTEN_ONLY
} li_can_slv_mode_t;

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_CAN_TYPES_H_

/** @} */
