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
 * @file li_can_slv_xload_types.h
 * @brief header to li_can_slv_xload_types.h module
 * @addtogroup lcs_xload_types
 * @{
 */

#ifndef LI_CAN_SLV_XLOAD_TYPES_H_
#define LI_CAN_SLV_XLOAD_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>
#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/error/li_can_slv_error_types.h>
#include <li_can_slv/xload/li_can_slv_xload_buffer_types.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/
/**
 * The enumeration of the status handle
 */
typedef enum
{
	LI_CAN_SLV_XLOAD_STACK_END_HANDLING = 0, /**< status to send an end acknowledge after end handle function call */
	LI_CAN_SLV_XLOAD_NO_STACK_END_HANDLING = 1 /**< status to NOT send an end acknowledge end after handle function call */
} li_can_slv_xload_end_handle_status_t;

/**
 * @enum li_can_slv_xload_progress_tag
 * @typedef enum li_can_slv_xload_progress_tag li_can_slv_xload_progress_t
 */
typedef enum li_can_slv_xload_progress_tag
{
	LI_CAN_SLV_XLOAD_IN_PROGRESS = 1,
	LI_CAN_SLV_XLOAD_IDLE = 2
} li_can_slv_xload_progress_t;

typedef li_can_slv_errorcode_t (*li_can_slv_dload_start_handle_funcp_t)(li_can_slv_xload_component_t *dload_component);
typedef li_can_slv_errorcode_t (*li_can_slv_dload_version_request_handle_funcp_t)(li_can_slv_xload_component_t *dload_component, li_can_slv_xload_end_handle_status_t *status);
typedef li_can_slv_errorcode_t (*li_can_slv_dload_start_request_handle_funcp_t)(li_can_slv_xload_component_t *dload_component);
typedef li_can_slv_errorcode_t (*li_can_slv_dload_data_block_end_handle_funcp_t)(li_can_slv_xload_component_t *dload_component);
typedef li_can_slv_errorcode_t (*li_can_slv_dload_end_handle_funcp_t)(li_can_slv_xload_component_t *dload_component, li_can_slv_xload_end_handle_status_t *status);

#ifdef LI_CAN_SLV_ULOAD
typedef int16_t (*li_can_slv_uload_read_byte_funcp_t)(void);
typedef li_can_slv_errorcode_t (*li_can_slv_uload_info_request_handle_funcp_t)(li_can_slv_xload_component_t *dload_component, uint32_t *size_of_component, li_can_slv_xload_end_handle_status_t *status);
typedef li_can_slv_errorcode_t (*li_can_slv_uload_start_request_handle_funcp_t)(li_can_slv_xload_component_t *dload_component);
typedef li_can_slv_errorcode_t (*li_can_slv_uload_block_ack_handle_funcp_t)(li_can_slv_xload_component_t *dload_component);
#endif // #ifdef LI_CAN_SLV_ULOAD

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef LI_CAN_SLV_XLOAD_TYPES_H_
/** @} */
