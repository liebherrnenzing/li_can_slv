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
 * @file li_can_slv_xload_defines.h
 * @brief header to li_can_slv_xload_defines.h module
 * @addtogroup lcs_xload
 * @{
 */

#ifndef LI_CAN_SLV_XLOAD_DEFINES_H_
#define LI_CAN_SLV_XLOAD_DEFINES_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include "li_can_slv.h"

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
#define DLOAD_CRC_DEFAULT				0xFFFF /**< download default crc */
#define DLOAD_DATE_DEFAULT				0xFFFFFFFF /**< download default date */
#define DLOAD_COMPONENT_NAME_LENGTH		8 /**< length of component name to program in byte */

#ifdef DEBUG_DLOAD
#ifdef DEBUG_DLOAD_SLOW
#define IO_DLOAD_OBJ_TIME_DELAY_MS_HEX		10 /*!< time delay in ms between can objects for hex-file-downloads */
#else // #ifdef DEBUG_DLOAD_SLOW
#define IO_DLOAD_OBJ_TIME_DELAY_MS_HEX		0 /*!< time delay in ms between can objects for hex-file-downloads */
#endif // #ifdef DEBUG_DLOAD_SLOW
#define IO_DLOAD_OBJ_TIME_DELAY_MS_CNFG		10 /*!< time delay in ms between can objects for configuration-file-downloads (application is running) */
#define IO_DLOAD_OBJ_TIME_DELAY_MS_TPL		10 /*!< time delay in ms between can objects for tuple-file-downloads (application is running) */
#else // #ifdef DEBUG_DLOAD
#define IO_DLOAD_OBJ_TIME_DELAY_MS_HEX		0 /*!< time delay in ms between can objects for hex-file-downloads */
#define IO_DLOAD_OBJ_TIME_DELAY_MS_CNFG		5 /*!< time delay in ms between can objects for configuration-file-downloads (application is running) */
#define IO_DLOAD_OBJ_TIME_DELAY_MS_TPL		5 /*!< time delay in ms between can objects for tuple-file-downloads (application is running) */
#endif // #ifdef DEBUG_DLOAD

#define ULOAD_COMPONENT_NAME_LENGTH			DLOAD_COMPONENT_NAME_LENGTH /*!< length of component name to program in byte */
#define ULOAD_BLOCK_BUFFER_SIZE				LI_CAN_SLV_XLOAD_BLOCK_BUFFER_SIZE_DLOAD /*!< block buffer size for download */

#define DLOAD_SENDING_DATA		0x0001
#define	DLOAD_SUCCESS			0x0002
#define	DLOAD_END				0x0004

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

#endif // #ifndef LI_CAN_SLV_XLOAD_DEFINES_H_
/** @} */

