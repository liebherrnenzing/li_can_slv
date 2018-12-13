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
 * @file li_can_slv.h
 * @brief header to li_can_slv.h module
 * @addtogroup lcs_core
 * @{
 */

#ifndef LI_CAN_SLV_H_
#define LI_CAN_SLV_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>

#include <li_can_slv/api/li_can_slv_api.h>

#include <li_can_slv/async/io_can_async.h>
#include <li_can_slv/async/io_can_async_handler.h>

#include <li_can_slv/config/io_can_config.h>

#include <li_can_slv/core/io_can.h>
#include <li_can_slv/core/io_can_main.h>
#include <li_can_slv/core/io_can_main_handler.h>

#include <li_can_slv/core/io_can_mon.h>

#include <li_can_slv/error/io_can_error.h>
#include <li_can_slv/error/io_can_errno.h>

#include <li_can_slv/sync/io_can_sync.h>
#include <li_can_slv/sync/io_can_sync_handler.h>

#include <li_can_slv/sys/io_can_sys.h>

#include <li_can_slv/xload/li_can_slv_xload.h>
#include <li_can_slv/xload/li_can_slv_xload_api.h>
#include <li_can_slv/xload/li_can_slv_xload_buffer.h>
#include <li_can_slv/xload/li_can_slv_xload_info.h>

#include "li_can_slv_port.h"

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

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef LI_CAN_SLV_H_

/** @} */
