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
 * @file li_can_slv_xload_buffer_defines.h
 * @brief header to li_can_slv_xload_buffer_defines.h module
 * @addtogroup lcs_xload
 * @{
 */

#ifndef LI_CAN_SLV_XLOAD_BUFFER_DEFINES_H_
#define LI_CAN_SLV_XLOAD_BUFFER_DEFINES_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_USE_ASYNC_DOWNLOAD_BUFFER_SIZE
#define LI_CAN_SLV_XLOAD_BLOCK_BUFFER_SIZE_DLOAD_USER_DEFINED (LI_CAN_SLV_USE_ASYNC_DOWNLOAD_BUFFER_SIZE)
#endif // #ifdef LI_CAN_SLV_USE_ASYNC_DOWNLOAD_BUFFER_SIZE

#ifdef LI_CAN_SLV_XLOAD_BLOCK_BUFFER_SIZE_DLOAD_USER_DEFINED
#define LI_CAN_SLV_XLOAD_BLOCK_BUFFER_SIZE_DLOAD (LI_CAN_SLV_XLOAD_BLOCK_BUFFER_SIZE_DLOAD_USER_DEFINED)
#else // #ifdef LI_CAN_SLV_XLOAD_BLOCK_BUFFER_SIZE_DLOAD_USER_DEFINED
#define LI_CAN_SLV_XLOAD_BLOCK_BUFFER_SIZE_DLOAD (140) /*!< block buffer size for download 20 can objects with 7 bytes each */
#endif // #ifdef LI_CAN_SLV_XLOAD_BLOCK_BUFFER_SIZE_DLOAD_USER_DEFINED

#define LI_CAN_SLV_XLOAD_BUFFER_SIZE_DLOAD	(LI_CAN_SLV_XLOAD_BLOCK_BUFFER_SIZE_DLOAD)
#define LI_CAN_SLV_XLOAD_MAX_BYTES_PER_BLOCK_DLOAD (LI_CAN_SLV_XLOAD_BLOCK_BUFFER_SIZE_DLOAD) /**< Default max per block for dload */
#define LI_CAN_SLV_XLOAD_MAX_BYTES_PER_BLOCK_HEX_DLOAD (LI_CAN_SLV_XLOAD_BLOCK_BUFFER_SIZE_DLOAD - 56) /**< Default max per block for dload - maximum hexline count (51 byte round to 56 (7 * 8) */

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef LI_CAN_SLV_XLOAD_BUFFER_DEFINES_H_
/** @} */

