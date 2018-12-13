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
 * @file canpie_adapter/io_can_main_hw.c
 * @addtogroup lcs_arch_canpie_adapter
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "io_can_main_hw.h"
#include <li_can_slv/config/li_can_slv_config_internal.h>

#ifdef LI_CAN_SLV_SYNC
#include <li_can_slv/sync/io_can_sync.h>
#endif // #ifdef LI_CAN_SLV_SYNC

#include <li_can_slv/core/io_can_main.h>
#include <li_can_slv/core/io_can.h>

#include <li_can_slv/error/io_can_error.h>
#include <li_can_slv/error/io_can_errno.h>

#include <li_can_slv/config/io_can_config_types.h>
#include "io_can_main_hw_handler.h"

#include "canpie.h"
#include "cp_core.h"
#include "cp_msg.h"

#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_debug.h"
#endif // #ifdef LI_CAN_SLV_DEBUG

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#if CP_VERSION_MAJOR <= 2
#define CANPIE_MODE_START	(CP_MODE_START)
#define CANPIE_MODE_STOP	(CP_MODE_STOP)
#define CANPIE_MODE_LISTEN_ONLY	(CP_MODE_LISTEN_ONLY)
#define CANPIE_BUFFER_DIR_TX	(CP_BUFFER_DIR_TX)
#define CANPIE_BUFFER_DIR_RX	(CP_BUFFER_DIR_RX)
#else // #if CP_VERSION_MAJOR <= 2
#define CANPIE_MODE_START	(eCP_MODE_START)
#define CANPIE_MODE_STOP	(eCP_MODE_STOP)
#define CANPIE_MODE_LISTEN_ONLY	(eCP_MODE_LISTEN_ONLY)
#define CANPIE_BUFFER_DIR_TX	(eCP_BUFFER_DIR_TRM)
#define CANPIE_BUFFER_DIR_RX	(eCP_BUFFER_DIR_RCV)
#endif // #if CP_VERSION_MAJOR <= 2

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/
CpPort_ts can_port_main; // CAN interface

const can_config_bdr_tab_t can_main_hw_default_baudrate_table[CAN_CONFIG_SIZE_OF_BDR_TAB] = /**< baud rate table */
{
	{ CAN_CONFIG_DEF_BDR, 0, 0, 0, 0, 0, 0 },
	{ 125, 0, 0, 0, 0, 0, 0 },
	{ 500, 0, 0, 0, 0, 0, 0 },
	{ 1000, 0, 0, 0, 0, 0, 0 }
};

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/
static uint8_t msg_obj_used[LI_CAN_SLV_MAIN_NODE_MAX_NOF_MSG_OBJ];

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t can_main_hw_init(void)
{
#if CP_VERSION_MAJOR <= 2
	CpCoreDriverInit(LI_CAN_SLV_MAIN_ARCH, &can_port_main);
#else // #if CP_VERSION_MAJOR <= 2
	CpCoreDriverInit(LI_CAN_SLV_MAIN_ARCH, &can_port_main, 0);
#endif // #if CP_VERSION_MAJOR <= 2
	/* CpCoreIntFunctions(&can_port_main,  can_main_hw_handler_rx, can_main_hw_handler_tx, can_main_hw_handler_error); */
	CpCoreIntFunctions(&can_port_main, can_main_hw_handler_rx, can_main_hw_handler_tx, 0L);

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_main_hw_deinit(void)
{
	CpCoreDriverRelease(&can_port_main);

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_main_hw_msg_obj_init(uint16_t msg_obj)
{
	msg_obj_used[msg_obj] = FALSE;
	CpCoreBufferRelease(&can_port_main, msg_obj);

	return (LI_CAN_SLV_ERR_OK);
}

#if defined(OUTER) || defined(OUTER_APP)
li_can_slv_errorcode_t can_main_hw_free_tx_objs(void)
{
	/**
	 * @todo check usage for canpie
	 */

	//uint16_t i;
	//
	//for (i = 0; i < li_can_slv_sync_main_tx_msg_obj_used; i++)
	//{
	//	//	CpCoreBufferRelease(&can_port_main, li_can_slv_sync_main_tx_msg_obj[i].msg_obj);
	//}

	return (LI_CAN_SLV_ERR_OK);
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

li_can_slv_errorcode_t can_main_hw_get_next_free_msg_obj(uint16_t *msg_obj)
{
	uint16_t i;

#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
	LI_CAN_SLV_DEBUG_PRINT("\n\ncan_hw_get_nex_free_msg_obj");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW

	for (i = 0; i < LI_CAN_SLV_MAIN_NODE_MAX_NOF_MSG_OBJ; i++)
	{
		if (msg_obj_used[i] == FALSE)
		{
			// the object is free;
			*msg_obj = i;
			return (LI_CAN_SLV_ERR_OK);
		}
	}

#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
	LI_CAN_SLV_DEBUG_PRINT("\n\nERR_MSG_CAN_NO_MSG_OBJ_FREE");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW

	// no msg object is free
	return (ERR_MSG_CAN_NO_MSG_OBJ_FREE);
}

li_can_slv_errorcode_t can_main_hw_enable(void)
{
	li_can_slv_mode_t mode;

	mode = li_can_slv_get_mode();
	if (LI_CAN_SLV_MODE_LISTEN_ONLY == mode)
	{
		CpCoreCanMode(&can_port_main, CANPIE_MODE_LISTEN_ONLY);
	}
	else
	{
		if (li_can_slv_get_mode() != LI_CAN_SLV_MODE_OPERATIONAL)
		{
			CpCoreCanMode(&can_port_main, CANPIE_MODE_START);
		}
	}
	return (LI_CAN_SLV_ERR_OK);
}

#ifdef LI_CAN_SLV_RECONNECT
li_can_slv_errorcode_t can_main_hw_enable_listen_only(void)
{
	CpCoreCanMode(&can_port_main, CANPIE_MODE_LISTEN_ONLY);
	return (LI_CAN_SLV_ERR_OK);
}
#endif // #ifdef LI_CAN_SLV_RECONNECT

li_can_slv_errorcode_t can_main_hw_disable(void)
{
	CpCoreCanMode(&can_port_main, CANPIE_MODE_STOP);

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_main_hw_define_msg_obj(uint16_t msg_obj, uint16_t can_id, uint16_t acceptance_mask, byte_t dlc, byte_t dir, can_main_service_id_t service_id)
{
	uint8_t ubBufferIdxV;
#if CP_VERSION_MAJOR <= 2
	CpCanMsg_ts can_msg;
	enum CP_BUFFER_DIR msg_dir;

	ubBufferIdxV = (uint8_t)(msg_obj + 1);
#else // #if CP_VERSION_MAJOR <= 2
	enum CpBufferDir_e msg_dir;

	ubBufferIdxV = (uint8_t) msg_obj;
#endif // #if CP_VERSION_MAJOR <= 2

#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
	LI_CAN_SLV_DEBUG_PRINT("\ncan hw def msgobj:%d, id:%d", msg_obj, can_id);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW

	// set message object to used state
	msg_obj_used[msg_obj] = TRUE;

#if CP_VERSION_MAJOR <= 2
	CpMsgClear(&can_msg);
	CpMsgSetStdId(&can_msg, can_id);
	CpMsgSetDlc(&can_msg, dlc);
#endif // #if CP_VERSION_MAJOR <= 2

	if (dir == CAN_CONFIG_DIR_TX)
	{
		msg_dir = CANPIE_BUFFER_DIR_TX;
	}
	else
	{
		msg_dir = CANPIE_BUFFER_DIR_RX;
	}

	/**
	 * @todo check if the acceptance_mask is correct!
	 * */
#if CP_VERSION_MAJOR <= 2
	CpCoreBufferInit(&can_port_main, &can_msg, ubBufferIdxV, msg_dir);
	CpCoreBufferAccMask(&can_port_main, ubBufferIdxV, acceptance_mask);
#else // #if CP_VERSION_MAJOR <= 2
	CpCoreBufferConfig(&can_port_main, ubBufferIdxV, can_id, acceptance_mask, CP_MSG_FORMAT_CBFF, msg_dir);
#endif // #if CP_VERSION_MAJOR <= 2

	switch (service_id)
	{
		case CAN_MAIN_SERVICE_ID_TX:
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
			LI_CAN_SLV_DEBUG_PRINT(" with CAN_MAIN_SERVICE_ID_TX");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
			break;

		case CAN_MAIN_SERVICE_ID_RX:
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
			LI_CAN_SLV_DEBUG_PRINT(" with CAN_MAIN_SERVICE_ID_RX");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
			break;

		case CAN_MAIN_ASYNC_SERVICE_ID_TX:
		case CAN_MAIN_ASYNC_SERVICE_ID_RX:
#ifdef LI_CAN_SLV_BOOT
		case CAN_MAIN_ASYNC_CTRL_SERVICE_ID_RX:
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
			LI_CAN_SLV_DEBUG_PRINT(" with CAN_MAIN_ASYNC_CTRL_SERVICE_ID_RX");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
#endif // #ifdef LI_CAN_SLV_BOOT
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
			LI_CAN_SLV_DEBUG_PRINT(" with CAN_MAIN_ASYNC_SERVICE_ID_TX or CAN_MAIN_ASYNC_SERVICE_ID_RX");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
			break;
#if defined(OUTER) || defined(OUTER_APP)
		case CAN_MAIN_ASYNC_CTRL_SERVICE_ID_RX:
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
			LI_CAN_SLV_DEBUG_PRINT(" with CAN_MAIN_ASYNC_CTRL_SERVICE_ID_RX");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
			break;
#endif // #if defined(OUTER) || defined(OUTER_APP)

		default:
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
			LI_CAN_SLV_DEBUG_PRINT(" with ERR_MSG_CAN_MAIN_UNDEFINED_ISR_ID");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
			return (ERR_MSG_CAN_MAIN_UNDEFINED_ISR_ID);
			break;
	}
	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_main_hw_set_baudrate(can_config_bdr_tab_t *bdr_tab_entry)
{
	CpCoreCanMode(&can_port_main, CANPIE_MODE_STOP);

	switch (bdr_tab_entry->baudrate)
	{
		case 125:
#if CP_VERSION_MAJOR <= 2
			CpCoreBaudrate(&can_port_main, CP_BAUD_125K);
#else // #if CP_VERSION_MAJOR <= 2
			CpCoreBitrate(&can_port_main, eCP_BITRATE_125K, eCP_BITRATE_NONE);
#endif // #if CP_VERSION_MAJOR <= 2
			break;

		case 250:
#if CP_VERSION_MAJOR <= 2
			CpCoreBaudrate(&can_port_main, CP_BAUD_250K);
#else // #if CP_VERSION_MAJOR <= 2
			CpCoreBitrate(&can_port_main, eCP_BITRATE_250K, eCP_BITRATE_NONE);
#endif // #if CP_VERSION_MAJOR <= 2
			break;

		case 500:
#if CP_VERSION_MAJOR <= 2
			CpCoreBaudrate(&can_port_main, CP_BAUD_500K);
#else // #if CP_VERSION_MAJOR <= 2
			CpCoreBitrate(&can_port_main, eCP_BITRATE_500K, eCP_BITRATE_NONE);
#endif // #if CP_VERSION_MAJOR <= 2
			break;

		case 1000:
#if CP_VERSION_MAJOR <= 2
			CpCoreBaudrate(&can_port_main, CP_BAUD_1M);
#else // #if CP_VERSION_MAJOR <= 2
			CpCoreBitrate(&can_port_main, eCP_BITRATE_1M, eCP_BITRATE_NONE);
#endif // #if CP_VERSION_MAJOR <= 2
			break;

		default:
			CpCoreCanMode(&can_port_main, CANPIE_MODE_START);
			return (ERR_MSG_CAN_CONFIG_SET_INVALID_BAUDRATE);
			break;
	}

	if (li_can_slv_get_mode() == LI_CAN_SLV_MODE_OPERATIONAL)
	{
		CpCoreCanMode(&can_port_main, CANPIE_MODE_START);
	}
	else
	{
		CpCoreCanMode(&can_port_main, CANPIE_MODE_LISTEN_ONLY);
	}

	return (LI_CAN_SLV_ERR_OK);
}

#ifdef LI_CAN_SLV_RECONNECT
li_can_slv_errorcode_t can_main_hw_set_baudrate_listen_only(can_config_bdr_tab_t *bdr_tab_entry)
{
	CpCoreCanMode(&can_port_main, CANPIE_MODE_STOP);

	switch (bdr_tab_entry->baudrate)
	{
		case 125:
#if CP_VERSION_MAJOR <= 2
			CpCoreBaudrate(&can_port_main, CP_BAUD_125K);
#else // #if CP_VERSION_MAJOR <= 2
			CpCoreBitrate(&can_port_main, eCP_BITRATE_125K, eCP_BITRATE_NONE);
#endif // #if CP_VERSION_MAJOR <= 2
			break;

		case 250:
#if CP_VERSION_MAJOR <= 2
			CpCoreBaudrate(&can_port_main, CP_BAUD_250K);
#else // #if CP_VERSION_MAJOR <= 2
			CpCoreBitrate(&can_port_main, eCP_BITRATE_250K, eCP_BITRATE_NONE);
#endif // #if CP_VERSION_MAJOR <= 2
			break;

		case 500:
#if CP_VERSION_MAJOR <= 2
			CpCoreBaudrate(&can_port_main, CP_BAUD_500K);
#else // #if CP_VERSION_MAJOR <= 2
			CpCoreBitrate(&can_port_main, eCP_BITRATE_500K, eCP_BITRATE_NONE);
#endif // #if CP_VERSION_MAJOR <= 2
			break;

		case 1000:
#if CP_VERSION_MAJOR <= 2
			CpCoreBaudrate(&can_port_main, CP_BAUD_1M);
#else // #if CP_VERSION_MAJOR <= 2
			CpCoreBitrate(&can_port_main, eCP_BITRATE_1M, eCP_BITRATE_NONE);
#endif // #if CP_VERSION_MAJOR <= 2
			break;

		default:
			CpCoreCanMode(&can_port_main, CANPIE_MODE_START);
			return (ERR_MSG_CAN_CONFIG_SET_INVALID_BAUDRATE);
			break;
	}

	CpCoreCanMode(&can_port_main, CANPIE_MODE_LISTEN_ONLY);

	return (LI_CAN_SLV_ERR_OK);
}
#endif // #ifdef LI_CAN_SLV_RECONNECT

li_can_slv_errorcode_t can_main_hw_send_msg_obj_blocking(uint16_t msg_obj, uint16_t can_id, uint16_t dlc, const volatile byte_t *src)
{
	uint8_t ubBufferIdxV;
#if CP_VERSION_MAJOR <= 2
	CpCanMsg_ts msg;

	ubBufferIdxV = (uint8_t)(msg_obj + 1);
#else // #if CP_VERSION_MAJOR <= 2
	CpStatus_tv ret = eCP_ERR_TRM_FULL;
	ubBufferIdxV = (uint8_t) msg_obj;
#endif // #if CP_VERSION_MAJOR <= 2

#if CP_VERSION_MAJOR <= 2
	CpMsgClear(&msg);
	CpMsgSetStdId(&msg, can_id);
	CpMsgSetDlc(&msg, dlc);

	while (CpCoreBufferBusy(&can_port_main, ubBufferIdxV) == CpErr_BUFFER_BUSY)
	{
		/** @todo implement any timeout handling here or search for a better solution */
	}

	CpCoreBufferInit(&can_port_main, &msg, ubBufferIdxV, CP_BUFFER_DIR_TX);
	CpCoreBufferSetData(&can_port_main, ubBufferIdxV, (uint8_t *)src);
	CpCoreBufferSend(&can_port_main, ubBufferIdxV);
#else // #if CP_VERSION_MAJOR <= 2
	CpCoreBufferConfig(&can_port_main, ubBufferIdxV, can_id, 0, CP_MSG_FORMAT_CBFF, eCP_BUFFER_DIR_TRM);
	CpCoreBufferSetData(&can_port_main, ubBufferIdxV, (uint8_t *) src, 0, dlc);
	CpCoreBufferSetDlc(&can_port_main, ubBufferIdxV, dlc);

	// tx message and wait until timeout
	ret = eCP_ERR_TRM_FULL;
	while (ret != eCP_ERR_NONE)
	{
		ret = CpCoreBufferSend(&can_port_main, ubBufferIdxV);
		/** @todo implement any timeout handling here or search for a better solution */
	}
#endif // #if CP_VERSION_MAJOR <= 2
	return (LI_CAN_SLV_ERR_OK);
}

#if defined(OUTER) || defined(OUTER_APP)
li_can_slv_errorcode_t can_main_hw_send_msg_obj_none_blocking(uint16_t msg_obj, uint16_t can_id, uint16_t dlc, const volatile byte_t *src)
{
	uint8_t ubBufferIdxV;
#if CP_VERSION_MAJOR <= 2
	CpCanMsg_ts msg;

	ubBufferIdxV = (uint8_t)(msg_obj + 1);
#else // #if CP_VERSION_MAJOR <= 2
	ubBufferIdxV = (uint8_t) msg_obj;
#endif // #if CP_VERSION_MAJOR <= 2

#if CP_VERSION_MAJOR <= 2
	CpMsgClear(&msg);
	CpMsgSetStdId(&msg, can_id);
	CpMsgSetDlc(&msg, dlc);

	CpCoreBufferInit(&can_port_main, &msg, ubBufferIdxV, CP_BUFFER_DIR_TX);
	CpCoreBufferSetData(&can_port_main, ubBufferIdxV, (uint8_t *)src);
	CpCoreBufferSend(&can_port_main, ubBufferIdxV);
#else // #if CP_VERSION_MAJOR <= 2
	CpCoreBufferConfig(&can_port_main, ubBufferIdxV, can_id, 0, CP_MSG_FORMAT_CBFF, eCP_BUFFER_DIR_TRM);
	CpCoreBufferSetData(&can_port_main, ubBufferIdxV, (uint8_t *) src, 0, dlc);
	CpCoreBufferSetDlc(&can_port_main, ubBufferIdxV, dlc);
	CpCoreBufferSend(&can_port_main, ubBufferIdxV);
#endif // #if CP_VERSION_MAJOR <= 2

	return (LI_CAN_SLV_ERR_OK);
}

#endif // #if defined(OUTER) || defined(OUTER_APP)

/** @} */
