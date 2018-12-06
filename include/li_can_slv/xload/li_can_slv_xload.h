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
 * @file li_can_slv_xload.h
 * @brief header to li_can_slv_xload.h module
 * @addtogroup lcs_xload
 * @{
 */

#ifndef LI_CAN_SLV_XLOAD_H_
#define LI_CAN_SLV_XLOAD_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>
#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/error/li_can_slv_error_types.h>
#include <li_can_slv/xload/li_can_slv_xload_types.h>
#include <li_can_slv/xload/li_can_slv_xload_defines.h>
#include <li_can_slv/xload/li_can_slv_xload_buffer_defines.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t li_can_slv_dload_start_ackn(li_can_slv_module_nr_t module_nr, uint16_t max_bytes_per_block, byte_t t_delay_objs);

li_can_slv_errorcode_t li_can_slv_dload_set_start_handle(li_can_slv_dload_start_handle_funcp_t handle);
li_can_slv_errorcode_t li_can_slv_dload_set_version_request_handle(li_can_slv_dload_version_request_handle_funcp_t handle);
li_can_slv_errorcode_t li_can_slv_dload_set_start_request_handle(li_can_slv_dload_start_request_handle_funcp_t handle);
li_can_slv_errorcode_t li_can_slv_dload_set_data_block_end_handle(li_can_slv_dload_data_block_end_handle_funcp_t handle);
li_can_slv_errorcode_t li_can_slv_dload_set_end_handle(li_can_slv_dload_end_handle_funcp_t handle);

li_can_slv_errorcode_t li_can_slv_dload_version_request_short(li_can_slv_module_nr_t module_nr, const byte_t *src);
li_can_slv_errorcode_t li_can_slv_dload_version_request_long1(li_can_slv_module_nr_t module_nr, const byte_t *src);
li_can_slv_errorcode_t li_can_slv_dload_version_request_long2(li_can_slv_module_nr_t module_nr, const byte_t *src);
li_can_slv_errorcode_t li_can_slv_dload_version_answer(li_can_slv_module_nr_t module_nr, byte_t index, uint16_t crc, uint32_t download_id);

li_can_slv_errorcode_t li_can_slv_dload_start_request1(li_can_slv_module_nr_t module_nr, const byte_t *src);
li_can_slv_errorcode_t li_can_slv_dload_start_request2(li_can_slv_module_nr_t module_nr, const byte_t *src);
li_can_slv_errorcode_t li_can_slv_dload_start(li_can_slv_xload_component_t *dload_component);

li_can_slv_errorcode_t li_can_slv_dload_data(li_can_slv_module_nr_t module_nr, const byte_t *src);
li_can_slv_errorcode_t li_can_slv_dload_data_block_end(li_can_slv_module_nr_t module_nr, const byte_t *src);
li_can_slv_errorcode_t li_can_slv_dload_end(li_can_slv_module_nr_t module_nr);
li_can_slv_errorcode_t li_can_slv_dload_termination(li_can_slv_module_nr_t module_nr, uint32_t error);

#if defined(OUTER) || defined(OUTER_APP)
li_can_slv_errorcode_t li_can_slv_dload_end_ackn(li_can_slv_module_nr_t module_nr);
#endif // #if defined(OUTER) || defined(OUTER_APP)

uint8_t li_can_slv_xload_active(void);

#ifdef LI_CAN_SLV_ULOAD
li_can_slv_errorcode_t li_can_slv_uload_set_info_request_handle(li_can_slv_uload_info_request_handle_funcp_t handle);
li_can_slv_errorcode_t li_can_slv_uload_set_start_request_handle(li_can_slv_uload_start_request_handle_funcp_t handle);
li_can_slv_errorcode_t li_can_slv_uload_set_uload_block_ack_handle(li_can_slv_uload_block_ack_handle_funcp_t handle);
#endif // #ifdef LI_CAN_SLV_ULOAD

#ifdef LI_CAN_SLV_ULOAD
li_can_slv_errorcode_t li_can_slv_uload_info_request_short(li_can_slv_module_nr_t module_nr, const byte_t *src);
li_can_slv_errorcode_t li_can_slv_uload_info_answer(li_can_slv_module_nr_t module_nr, byte_t index, uint32_t size_of_component);
li_can_slv_errorcode_t li_can_slv_uload_start_request1(li_can_slv_module_nr_t module_nr, const byte_t *src);
li_can_slv_errorcode_t li_can_slv_uload_start_request2(li_can_slv_module_nr_t module_nr, const byte_t *src);
li_can_slv_errorcode_t li_can_slv_uload_data_from_buffer(uint8_t *data, uint16_t data_length);
li_can_slv_errorcode_t li_can_slv_uload_block_ackn(li_can_slv_module_nr_t module_nr, const byte_t *src);
li_can_slv_errorcode_t li_can_slv_uload_end_ackn(li_can_slv_module_nr_t module_nr, const byte_t *src);
li_can_slv_errorcode_t li_can_slv_uload_termination(li_can_slv_module_nr_t module_nr, const byte_t *src, uint32_t *error);
li_can_slv_errorcode_t li_can_slv_uload_end(li_can_slv_module_nr_t module_nr);
li_can_slv_errorcode_t li_can_slv_uload_data_block_end(li_can_slv_module_nr_t module_nr);
#endif // #ifdef LI_CAN_SLV_ULOAD

#ifdef __cplusplus
} // closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef LI_CAN_SLV_XLOAD_H_
/** @} */

