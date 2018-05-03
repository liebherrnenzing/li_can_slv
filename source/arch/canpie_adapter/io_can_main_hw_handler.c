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
#include "io_can_sync_handler.h"

#ifdef LI_CAN_SLV_ASYNC
#include "io_can_async_handler.h"
#endif // #ifdef LI_CAN_SLV_ASYNC

#include "io_can.h"

#include "io_can_types.h"
#include "io_can_error.h"
#include "io_can_main.h"

#ifdef LI_CAN_SLV_ASYNC
#include "io_can_async.h"
#endif // #ifdef LI_CAN_SLV_ASYNC

#include "io_can_config.h"

#ifdef LI_CAN_SLV_SYNC
#include "io_can_sync.h"
#endif // #ifdef LI_CAN_SLV_SYNC

#ifdef LI_CAN_SLV_RECONNECT
#include "io_can_reconnect.h"
#endif // #ifdef LI_CAN_SLV_RECONNECT

#include "io_can_sys.h"

#include "io_can_main_hw.h"
#include "io_can_main_hw_inline.h"

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
	/** @todo add workaround for the aligned problem for tricore cpu */
	/* uint8_t __attribute__ ((aligned(2))) data[8]; */
	uint16_t canid;
	uint8_t dlc;

#ifdef LI_CAN_SLV_RECONNECT
	// on canpie adapter layer we use no error interrupt and no software triggered call of this function
	// so if we get here we are receiving any valid message
	li_can_slv_reconnect_main_node_msg_pending = CAN_INTID_MSG;

	if (li_can_slv_reconnect_get_state() != CAN_RECONNECT_STATE_OFF)
	{
#if 0
		li_can_slv_reconnect_process(li_can_slv_reconnect_main_node_msg_pending, CAN_LEC_NO_ERROR);
#endif
		return (LI_CAN_SLV_ERR_OK);
	}
#endif // #ifdef LI_CAN_SLV_RECONNECT

	canid = CpMsgGetStdId(ptsCanMsgV);
	CpCoreBufferGetDlc(&can_port_main, ubBufferIdxV, &dlc);
#if CP_VERSION_MAJOR <= 2
	CpCoreBufferGetData(&can_port_main, ubBufferIdxV, &(data[0]));
#else // #if CP_VERSION_MAJOR <= 2
	CpCoreBufferGetData(&can_port_main, ubBufferIdxV, &(data[0]), 0, dlc);
#endif // #if CP_VERSION_MAJOR <= 2

#if CP_VERSION_MAJOR <= 2
	ubBufferIdxV = ubBufferIdxV - 1;
#endif // #if CP_VERSION_MAJOR <= 2

#ifdef LI_CAN_SLV_DEBUG_CAN_MAIN_HW_HANDLER
	LI_CAN_SLV_DEBUG_PRINT("\n\nrx obj: %d id: 0x%x", ubBufferIdxV, canid);
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

/**
 * @param ptsCanMsgV
 * @param ubBufferIdxV
 * @return
 */
uint8_t can_main_hw_handler_tx(CpCanMsg_ts *ptsCanMsgV, uint8_t ubBufferIdxV)
{
	uint8_t data[8];
	/** @todo add workaround for the aligned problem for tricore cpu */
	/* uint8_t __attribute__ ((aligned(2))) data[8]; */
	uint16_t canid;
	uint8_t dlc;

	canid = CpMsgGetStdId(ptsCanMsgV);
	CpCoreBufferGetDlc(&can_port_main, ubBufferIdxV, &dlc);
#if CP_VERSION_MAJOR <= 2
	CpCoreBufferGetData(&can_port_main, ubBufferIdxV, &(data[0]));
#else // #if CP_VERSION_MAJOR <= 2
	CpCoreBufferGetData(&can_port_main, ubBufferIdxV, &(data[0]), 0, dlc);
#endif // #if CP_VERSION_MAJOR <= 2

#if CP_VERSION_MAJOR <= 2
	ubBufferIdxV = ubBufferIdxV - 1;
#endif // #if CP_VERSION_MAJOR <= 2

#ifdef LI_CAN_SLV_DEBUG_CAN_MAIN_HW_HANDLER
	LI_CAN_SLV_DEBUG_PRINT("\n\ntx obj: %d id: 0x%x", ubBufferIdxV, canid);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_MAIN_HW_HANDLER

#ifndef LI_CAN_SLV_BOOT
#if defined (LI_CAN_SLV_SYNC)
	(void) can_sync_handler_tx(ubBufferIdxV, dlc, canid);
#endif // #if defined (LI_CAN_SLV_SYNC)
#endif // #ifndef LI_CAN_SLV_BOOT

	return 0;
}

/**
 * @return
 */
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
#ifdef HISTORY
		hist_addx(HISTORY_CAN, "err->state:", cp_state.ubCanErrState);
		hist_addx(HISTORY_CAN, "type:", cp_state.ubCanErrType);
		hist_addx(HISTORY_CAN, "txcnt:", cp_state.ubCanTrmErrCnt);
		hist_addx(HISTORY_CAN, "rxcnt:", cp_state.ubCanRcvErrCnt);
#endif // #ifdef HISTORY

#ifdef LI_CAN_SLV_MAIN_MON
		if (CAN_MAINMON_TYPE_MAIN == can_mainmon_type)
		{
#endif // #ifdef LI_CAN_SLV_MAIN_MON
			if (CAN_RECONNECT_STATE_OFF == li_can_slv_reconnect_get_state())
			{
				if (CANPIE_STATE_BUS_OFF == cp_state.ubCanErrState)
				{
#if 0
					can_config_get_baudrate(&current_baudrate);
					can_config_set_baudrate_listen_only(current_baudrate);
#else

					can_main_hw_enable_listen_only();
#endif
					li_can_slv_reconnect_on_main_node_recovery(1);
				}

				if ((cp_state.ubCanErrState == CANPIE_STATE_BUS_PASSIVE))
					// if ((cp_state.ubCanErrType != CANPIE_ERR_TYPE_NONE))
					// if (((cp_state.ubCanErrState == CANPIE_STATE_BUS_PASSIVE)) && (cp_state.ubCanErrType != CANPIE_ERR_TYPE_NONE)) // || (cp_state.ubCanErrType == CANPIE_ERR_TYPE_FORM))
					// if ((cp_state.ubCanErrState == CANPIE_STATE_BUS_PASSIVE) || (cp_state.ubCanErrType != CANPIE_ERR_TYPE_NONE))
					// if ((cp_state.ubCanErrState == CANPIE_STATE_BUS_PASSIVE) ||  ((cp_state.ubCanErrState == CANPIE_STATE_BUS_ACTIVE) && (cp_state.ubCanErrType == CANPIE_ERR_TYPE_FORM)))
				{
#if 0
					can_config_get_baudrate(&current_baudrate);
					can_config_set_baudrate_listen_only(current_baudrate);
#else
					can_main_hw_enable_listen_only();
#endif
					li_can_slv_reconnect_on_main_node_online(1);
				}

			}
#ifdef LI_CAN_SLV_MAIN_MON
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

