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
 * @file io_can_async_handler.h
 * @brief header to io_can_async_handler.h module
 * @addtogroup can_async
 * @{
 */

#ifndef IO_CAN_ASYNC_HANDLER_H_
#define IO_CAN_ASYNC_HANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include "io_can_port.h"
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
#ifndef LI_CAN_SLV_BOOT
#ifdef LI_CAN_SLV_ASYNC
uint8_t can_async_handler_rx(uint16_t msg_obj, uint8_t dlc, uint16_t canid, uint8_t *data);
#endif // #ifdef LI_CAN_SLV_ASYNC
#ifdef LI_CAN_SLV_ASYNC_TUNNEL
uint8_t can_async_handler_data_rx(uint16_t msg_obj, uint8_t dlc, uint16_t canid, uint8_t *data);
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL
#endif // #ifndef LI_CAN_SLV_BOOT

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_CAN_ASYNC_HANDLER_H_

/** @} */
