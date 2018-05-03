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
 * @file canpie_adapter/io_can_hw.h
 * @brief header to io_can_hw.h module
 * @addtogroup lcs_arch_canpie_adapter
 * @{
 */

#ifndef IO_CAN_HW_H_
#define IO_CAN_HW_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include "io_can_hw_interface.h"
#include "io_can_port.h"

#include "cp_core.h"
/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
#if CP_VERSION_MAJOR <= 2
// This function checks whether the chosen message object is still executing a transmit request,
// or if the object can be accessed exclusively. 1 message object is busy 0 message object not busy
#define CAN_HW_MSG_OBJ_BUSY(msg_obj) ((CpCoreBufferBusy(&can_port_main, msg_obj+1) == CpErr_BUFFER_BUSY) ? 1 : 0)
#else // #if CP_VERSION_MAJOR <= 2
#define CAN_HW_MSG_OBJ_BUSY(msg_obj) (0)
#endif // #if CP_VERSION_MAJOR <= 2

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

#endif // #ifndef IO_CAN_HW_H_

/** @} */
