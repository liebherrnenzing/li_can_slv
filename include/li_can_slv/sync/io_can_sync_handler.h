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
 * @file io_can_sync_handler.h
 * @brief header to io_can_sync_handler.h module
 * @addtogroup can_sync
 * @{
 */

#ifndef IO_CAN_SYNC_HANDLER_H_
#define IO_CAN_SYNC_HANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>
#include <li_can_slv/core/io_can_types.h>

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
#if defined (LI_CAN_SLV_SYNC) || defined (LI_CAN_SLV_BOOT)
uint8_t can_sync_handler_rx(uint16_t msg_obj, uint8_t dlc, uint16_t canid, uint8_t *data);
#endif // #if defined (LI_CAN_SLV_SYNC) || defined (LI_CAN_SLV_BOOT)

#if defined (LI_CAN_SLV_SYNC) && defined (LI_CAN_SLV_MON)
uint8_t can_sync_handler_rx_mon(uint16_t msg_obj, uint8_t dlc, uint16_t canid, uint8_t *data);
#endif // #if defined (LI_CAN_SLV_SYNC) && defined (LI_CAN_SLV_MON)

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_CAN_SYNC_HANDLER_H_

/** @} */
