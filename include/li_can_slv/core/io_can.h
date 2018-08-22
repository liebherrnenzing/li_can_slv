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
 * @file io_can.h
 * @brief header to io_can module
 * @addtogroup lcs_core
 * @{
 */

#ifndef IO_CAN_H
#define IO_CAN_H

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/core/li_can_slv.h>

#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/core/li_can_slv_core_api.h>
#include <li_can_slv/core/io_can_main_types.h>
#include <li_can_slv/error/li_can_slv_error_types.h>
#include <li_can_slv/config/io_can_config_types.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/
extern can_mainmon_type_t can_mainmon_type;

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t li_can_slv_init(can_config_bdr_t baudrate);
li_can_slv_errorcode_t li_can_slv_deinit(void);
li_can_slv_errorcode_t li_can_slv_process(void);
li_can_slv_errorcode_t can_transceiver_disable(void);

/**
 * @param new_mode New #li_can_slv_mode_t to set
 */
void li_can_slv_set_mode(li_can_slv_mode_t new_mode);

/**
 * @return the current #li_can_slv_mode_t
 */
li_can_slv_mode_t li_can_slv_get_mode(void);

#ifdef CAN_HW_DUMP_MSG_OBJECTS
li_can_slv_errorcode_t can_dump_msg_objects(uint16_t start_idx, uint16_t end_idx);
#endif // #ifdef CAN_HW_DUMP_MSG_OBJECTS

#ifdef __cplusplus
}// closing brace for extern "C"
#endif

#endif // #ifndef IO_CAN_H
/** @} */

