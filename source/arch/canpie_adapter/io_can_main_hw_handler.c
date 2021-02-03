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
 * @file canpie_adapter/io_can_main_hw_handler.c
 * @addtogroup lcs_arch_canpie_adapter
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "io_can_main_hw_handler.h"
#include <li_can_slv/config/li_can_slv_config_internal.h>

#include <li_can_slv/sync/io_can_sync_handler.h>

#ifdef LI_CAN_SLV_ASYNC
#include <li_can_slv/async/io_can_async_handler.h>
#endif // #ifdef LI_CAN_SLV_ASYNC

#include <li_can_slv/core/io_can.h>

#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/error/io_can_error.h>
#include <li_can_slv/core/io_can_main.h>

#ifdef LI_CAN_SLV_ASYNC
#include <li_can_slv/async/io_can_async.h>
#endif // #ifdef LI_CAN_SLV_ASYNC

#include <li_can_slv/config/io_can_config.h>

#ifdef LI_CAN_SLV_SYNC
#include <li_can_slv/sync/io_can_sync.h>
#endif // #ifdef LI_CAN_SLV_SYNC

#ifdef LI_CAN_SLV_RECONNECT
#include <li_can_slv/core/io_can_reconnect.h>
#endif // #ifdef LI_CAN_SLV_RECONNECT

#include <li_can_slv/sys/io_can_sys.h>

#include "io_can_main_hw.h"
#include <li_can_slv/arch/io_can_main_hw_inline.h>

#include "cp_core.h"
#include "cp_msg.h"

#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_debug.h"
#endif // #ifdef LI_CAN_SLV_DEBUG

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#define CANPIE_STATE_BUS_ACTIVE	(eCP_STATE_BUS_ACTIVE)
#define CANPIE_STATE_BUS_PASSIVE	(eCP_STATE_BUS_PASSIVE)
#define CANPIE_STATE_BUS_OFF	(eCP_STATE_BUS_OFF)
#define CANPIE_ERR_TYPE_NONE	(eCP_ERR_TYPE_NONE)
#define CANPIE_ERR_TYPE_FORM	(eCP_ERR_TYPE_FORM)


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
//static li_can_slv_msg_obj_t can_main_hw_msg_obj;

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
uint8_t can_main_hw_handler_rx(CpCanMsg_ts *ptsCanMsgV, uint8_t ubBufferIdxV)
{
	uint8_t data[8];
	uint16_t canid;
	uint8_t dlc;

#ifdef LI_CAN_SLV_RECONNECT
	// on canpie adapter layer we use no error interrupt and no software triggered call of this function
	// so if we get here we are receiving any valid message
	li_can_slv_reconnect_main_node_msg_pending = CAN_RECONNECT_INTID_MSG;

	if (li_can_slv_reconnect_get_state() != CAN_RECONNECT_STATE_OFF)
	{
		//		li_can_slv_reconnect_process(li_can_slv_reconnect_main_node_msg_pending, CAN_LEC_NO_ERROR);
		return (LI_CAN_SLV_ERR_OK);
	}
#endif // #ifdef LI_CAN_SLV_RECONNECT

	// 29Bit is not handled here
	if (CpMsgIsExtended(ptsCanMsgV))
	{
		return (LI_CAN_SLV_ERR_OK);
	}

	canid = CpMsgGetStdId(ptsCanMsgV);
	CpCoreBufferGetDlc(&can_port_main, ubBufferIdxV, &dlc);
	CpCoreBufferGetData(&can_port_main, ubBufferIdxV, &(data[0]), 0, dlc);

#ifdef LI_CAN_SLV_DEBUG_CAN_MAIN_HW_HANDLER
	LI_CAN_SLV_DEBUG_PRINT("rx obj: %d id: 0x%x\n", ubBufferIdxV, canid);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_MAIN_HW_HANDLER

#if defined (LI_CAN_SLV_SYNC) || defined (LI_CAN_SLV_BOOT)
	(void) can_sync_handler_rx(ubBufferIdxV, dlc, canid, data);
#endif // #if defined (LI_CAN_SLV_SYNC) || defined (LI_CAN_SLV_BOOT)

#ifndef LI_CAN_SLV_BOOT
#ifdef LI_CAN_SLV_ASYNC
	(void)can_async_handler_rx(ubBufferIdxV, dlc, canid, data);
#endif // #ifdef LI_CAN_SLV_ASYNC
#ifdef LI_CAN_SLV_ASYNC_TUNNEL
	(void)can_async_handler_data_rx(ubBufferIdxV, dlc, canid, data);
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL
#endif // #ifndef LI_CAN_SLV_BOOT

	return 0;
}

#ifdef LI_CAN_SLV_ARCH_USE_CANPIE_ADAPTER_ERROR_HANDLER
/**
 * @return
 */
uint8_t _can_main_hw_handler_error(CpState_ts *ptsErrV)
{
	ptsErrV = ptsErrV;

	can_main_hw_handler_error();

	return 0;
}
#endif // #ifdef LI_CAN_SLV_ARCH_USE_CANPIE_ADAPTER_ERROR_HANDLER

uint8_t can_main_hw_handler_error(void)
{
	CpState_ts cp_state;
#ifdef LI_CAN_SLV_RECONNECT
	int16_t cp_lec;
#endif // #ifdef LI_CAN_SLV_RECONNECT
#if 0
	li_can_slv_config_bdr_t current_baudrate;
#endif

	(void) CpCoreCanState(&can_port_main, &cp_state);
#ifdef LI_CAN_SLV_RECONNECT
	cp_lec = cp_state.ubCanErrType;
#endif // #ifdef LI_CAN_SLV_RECONNECT

#ifdef LI_CAN_SLV_RECONNECT
	if ((cp_state.ubCanErrState != CANPIE_STATE_BUS_ACTIVE) || (cp_state.ubCanErrType != CANPIE_ERR_TYPE_NONE))
	{
#ifdef LI_CAN_SLV_DEBUG_CAN_ERROR
		DO_EVERY(20, LI_CAN_SLV_DEBUG_PRINT("CAN_ERR->state:%d\n", cp_state.ubCanErrState));
		DO_EVERY(20, LI_CAN_SLV_DEBUG_PRINT("CAN_ERR->type:%d\n", cp_state.ubCanErrType));
		DO_EVERY(20, LI_CAN_SLV_DEBUG_PRINT("CAN_ERR->txcnt:%d\n", cp_state.ubCanTrmErrCnt));
		DO_EVERY(20, LI_CAN_SLV_DEBUG_PRINT("CAN_ERR->rxcnt:%d\n", cp_state.ubCanRcvErrCnt));
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_ERROR

#ifdef LI_CAN_SLV_MAIN_MON
		if (CAN_MAINMON_TYPE_MAIN == can_mainmon_type)
		{
#endif // #ifdef LI_CAN_SLV_MAIN_MON
			if (CAN_RECONNECT_STATE_OFF == li_can_slv_reconnect_get_state())
			{
				if ((cp_state.ubCanErrState == CANPIE_STATE_BUS_OFF) || (cp_state.ubCanErrType != CANPIE_ERR_TYPE_NONE))
				{
					(void)li_can_slv_set_node_mode(LI_CAN_SLV_MODE_STOPPED);
					(void)li_can_slv_set_node_mode(LI_CAN_SLV_MODE_LISTEN_ONLY);

					li_can_slv_reconnect_on_main_node_recovery(1);
				}

				if ((cp_state.ubCanErrState == CANPIE_STATE_BUS_PASSIVE))
					// if ((cp_state.ubCanErrType != CANPIE_ERR_TYPE_NONE))
					// if (((cp_state.ubCanErrState == CANPIE_STATE_BUS_PASSIVE)) && (cp_state.ubCanErrType != CANPIE_ERR_TYPE_NONE)) // || (cp_state.ubCanErrType == CANPIE_ERR_TYPE_FORM))
					// if ((cp_state.ubCanErrState == CANPIE_STATE_BUS_PASSIVE) || (cp_state.ubCanErrType != CANPIE_ERR_TYPE_NONE))
					// if ((cp_state.ubCanErrState == CANPIE_STATE_BUS_PASSIVE) ||  ((cp_state.ubCanErrState == CANPIE_STATE_BUS_ACTIVE) && (cp_state.ubCanErrType == CANPIE_ERR_TYPE_FORM)))
				{


					CpCoreCanMode(&can_port_main, CANPIE_MODE_LISTEN_ONLY);

					//(void)li_can_slv_set_node_mode(LI_CAN_SLV_MODE_STOPPED);
					//(void)li_can_slv_set_node_mode(LI_CAN_SLV_MODE_LISTEN_ONLY);

					li_can_slv_reconnect_on_main_node_online(1);
				}

			}
#ifdef LI_CAN_SLV_MAIN_MON
		}
		else
		{
			(void)li_can_slv_set_node_mode(LI_CAN_SLV_MODE_STOPPED);
			(void)li_can_slv_set_node_mode(LI_CAN_SLV_MODE_LISTEN_ONLY);
		}
#endif // #ifdef LI_CAN_SLV_MAIN_MON
	}
#endif // #ifdef LI_CAN_SLV_RECONNECT

#ifdef LI_CAN_SLV_RECONNECT
	// can reconnect process
	if (CAN_RECONNECT_STATE_OFF != li_can_slv_reconnect_get_state())
	{
		li_can_slv_reconnect_process(li_can_slv_reconnect_main_node_msg_pending, cp_lec);
	}
#endif // #ifdef LI_CAN_SLV_RECONNECT
	return 0;
}

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/** @} */

