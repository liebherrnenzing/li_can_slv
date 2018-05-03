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
 * @file li_can_slv_xload_buffer_types.h
 * @brief header to li_can_slv_xload_buffer_types.h module
 * @addtogroup lcs_xload
 * @{
 */

#ifndef LI_CAN_SLV_XLOAD_BUFFER_TYPES_H_
#define LI_CAN_SLV_XLOAD_BUFFER_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include "li_can_slv.h"
#include "io_can_types.h"

#include "li_can_slv_xload_buffer_defines.h"
#include "li_can_slv_xload_defines.h"
#include "li_can_slv_config_defines.h"

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
/**
 * @brief struct for download (component data is stored)
 * @struct li_can_slv_xload_component_tag
 * @typedef struct li_can_slv_xload_component_tag li_can_slv_xload_component_t
 */
typedef struct li_can_slv_xload_component_tag
{
	li_can_slv_module_nr_t module_nr; /**< system info download module number */
	char_t name[DLOAD_COMPONENT_NAME_LENGTH + 1]; /**< component name inclusive NULL-character */
	uint16_t crc; /**< 16 bit crc of the component download */
	uint32_t download_id; /**< ID of component to download */
	byte_t index_of_request; /**< index of request */
	char_t module_type[CAN_CONFIG_TYPE_STRING_LENGTH]; /**< module type */
} li_can_slv_xload_component_t;

#ifdef LI_CAN_SLV_DLOAD_BUFFER_INTERNAL
/**
 * @brief struct for download and handling of data buffer
 * @struct dload_buffer_tag
 * @typedef struct dload_buffer_tag li_can_slv_xload_buffer_t
 */
typedef struct dload_buffer_tag
{
	uint16_t bytes_cnt_of_block; /**< bytes of block from CAN-bus */
#ifdef LI_CAN_SLV_DLOAD
	uint16_t nr_of_can_objs; /**< number of CAN objects from CAN-bus */
#endif // #ifdef LI_CAN_SLV_DLOAD
	char_t data[LI_CAN_SLV_XLOAD_BUFFER_SIZE_DLOAD]; /**< character buffer for data to download */
	uint16_t read; /**< index for reading from data buffer */
	uint16_t read_storage; /**< index for reading from data buffer */
	uint16_t write; /**< index for writing to data buffer */
	uint16_t nr_bytes; /**< number of bytes */
	uint16_t nr_bytes_storage; /**< number of bytes */
	uint16_t nr_bytes_end; /**< number of the end byte */
#if defined(OUTER) || defined(OUTER_APP)
	uint16_t delay_between_objs_msec; /**< time delay */
	uint16_t max_bytes_per_block; /**< maximum bytes per block */
	uint32_t start_address; /**< start address */
	uint16_t block_status; /**< block status */
#endif	// #if defined(OUTER) || defined(OUTER_APP)
} li_can_slv_xload_buffer_t;
#endif // #ifdef LI_CAN_SLV_DLOAD_BUFFER_INTERNAL

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

#endif // #ifndef LI_CAN_SLV_XLOAD_BUFFER_TYPES_H_
/** @} */

