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
 * @file canpie_adapter/io_can_mon_hw_handler.c
 * @addtogroup lcs_arch_canpie_adapter
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "io_can_mon_hw_handler.h"
#include <li_can_slv/config/li_can_slv_config_internal.h>

#ifdef LI_CAN_SLV_MON
#include <li_can_slv/sync/io_can_sync_handler.h>

#ifdef LI_CAN_SLV_ASYNC
#include <li_can_slv/async/io_can_async_handler.h>
#endif // #ifdef LI_CAN_SLV_ASYNC

#include <li_can_slv/core/io_can.h>
#include <li_can_slv/error/io_can_error.h>
#include <li_can_slv/core/io_can_mon.h>

#ifdef LI_CAN_SLV_SYNC
#include <li_can_slv/sync/io_can_sync.h>
#endif // #ifdef LI_CAN_SLV_SYNC

#ifdef LI_CAN_SLV_RECONNECT
#include <li_can_slv/core/io_can_reconnect.h>
#endif // #ifdef LI_CAN_SLV_RECONNECT

#include <li_can_slv/sys/io_can_sys.h>

#include "io_can_mon_hw.h"
#include <li_can_slv/arch/io_can_mon_hw_inline.h>

#include "cp_core.h"
#include "cp_msg.h"

#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_debug.h"
#endif // #ifdef LI_CAN_SLV_DEBUG

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#if CP_VERSION_MAJOR <= 2
#define CANPIE_STATE_BUS_ACTIVE	(CP_STATE_BUS_ACTIVE)
#define CANPIE_STATE_BUS_PASSIVE	(CP_STATE_BUS_PASSIVE)
#define CANPIE_STATE_BUS_OFF	(CP_STATE_BUS_OFF)
#define CANPIE_ERR_TYPE_NONE	(CP_ERR_TYPE_NONE)
#define CANPIE_ERR_TYPE_FORM	(CP_ERR_TYPE_FORM)
#else // #if CP_VERSION_MAJOR <= 2
#define CANPIE_STATE_BUS_ACTIVE	(eCP_STATE_BUS_ACTIVE)
#define CANPIE_STATE_BUS_PASSIVE	(eCP_STATE_BUS_PASSIVE)
#define CANPIE_STATE_BUS_OFF	(eCP_STATE_BUS_OFF)
#define CANPIE_ERR_TYPE_NONE	(eCP_ERR_TYPE_NONE)
#define CANPIE_ERR_TYPE_FORM	(eCP_ERR_TYPE_FORM)
#endif // #if CP_VERSION_MAJOR <= 2

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/
//static li_can_slv_msg_obj_t can_mon_hw_msg_obj;

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
/**
 * @param ptsCanMsgV
 * @param ubBufferIdxV
 * @return
 */
uint8_t can_mon_hw_handler_rx(CpCanMsg_ts *ptsCanMsgV, uint8_t ubBufferIdxV)
{
	uint8_t data[8];
	uint16_t canid;
	uint8_t dlc;

	canid = CpMsgGetStdId(ptsCanMsgV);
	CpCoreBufferGetDlc(&can_port_mon, ubBufferIdxV, &dlc);
#if CP_VERSION_MAJOR <= 2
	CpCoreBufferGetData(&can_port_mon, ubBufferIdxV, &(data[0]));
#else // #if CP_VERSION_MAJOR <= 2
	CpCoreBufferGetData(&can_port_mon, ubBufferIdxV, &(data[0]), 0, dlc);
#endif // #if CP_VERSION_MAJOR <= 2

#if CP_VERSION_MAJOR <= 2
	ubBufferIdxV = ubBufferIdxV - 1;
#endif // #if CP_VERSION_MAJOR <= 2

#ifdef LI_CAN_SLV_DEBUG_CAN_MON_HW_HANDLER
	LI_CAN_SLV_DEBUG_PRINT("rx obj: %d id: 0x%x\n", ubBufferIdxV, canid);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_MON_HW_HANDLER

#if defined (LI_CAN_SLV_SYNC) && defined (LI_CAN_SLV_MON)
	(void) can_sync_handler_rx_mon(ubBufferIdxV, dlc, canid, data);
#endif // #if defined (LI_CAN_SLV_SYNC) && defined (LI_CAN_SLV_MON)

	return 0;
}

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#endif // #ifdef LI_CAN_SLV_MON

/** @} */

