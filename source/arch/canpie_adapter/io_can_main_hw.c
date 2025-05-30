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

#include "li_can_slv_port.h"

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#define CAN_MAIN_TX_FIFO_SIZE	(16u)

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
static CpFifo_ts tx_fifo;
static CpCanMsg_ts tx_fifo_messages[CAN_MAIN_TX_FIFO_SIZE];

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
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	CpCoreDriverInit(LI_CAN_SLV_MAIN_ARCH, &can_port_main, 0);

#ifdef LI_CAN_SLV_ARCH_USE_CANPIE_ADAPTER_ERROR_HANDLER
	CpCoreIntFunctions(&can_port_main,  can_main_hw_handler_rx, 0L, _can_main_hw_handler_error);
#else // #ifdef LI_CAN_SLV_ARCH_USE_CANPIE_ADAPTER_ERROR_HANDLER
	CpCoreIntFunctions(&can_port_main, can_main_hw_handler_rx, 0L, 0L);
#endif // #ifdef LI_CAN_SLV_ARCH_USE_CANPIE_ADAPTER_ERROR_HANDLER

	CpFifoInit(&tx_fifo, &tx_fifo_messages[0], CAN_MAIN_TX_FIFO_SIZE);
	CpCoreBufferConfig(&can_port_main, li_can_slv_sync_main_tx_msg_obj, (uint32_t) 0, CP_MASK_STD_FRAME, CP_MSG_FORMAT_CBFF, eCP_BUFFER_DIR_TRM);
	CpCoreFifoConfig(&can_port_main, li_can_slv_sync_main_tx_msg_obj, &tx_fifo);

	return err;
}

li_can_slv_errorcode_t can_main_hw_deinit(void)
{
	uint16_t i;

	CpCoreDriverRelease(&can_port_main);
	for (i = 0; i < LI_CAN_SLV_MAIN_NODE_MAX_NOF_MSG_OBJ; i++)
	{
		msg_obj_used[i] = FALSE;
	}

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
	//CpCoreBufferRelease(&can_port_main, li_can_slv_sync_main_tx_msg_obj.msg_obj);
	return (LI_CAN_SLV_ERR_OK);
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

li_can_slv_errorcode_t can_main_hw_get_next_free_msg_obj(uint16_t *msg_obj)
{
	uint16_t i;

#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
	LI_CAN_SLV_DEBUG_PRINT("can_hw_get_nex_free_msg_obj\n");
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
	LI_CAN_SLV_DEBUG_PRINT("ERR_MSG_CAN_NO_MSG_OBJ_FREE\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW

	// no msg object is free
	return (ERR_MSG_CAN_NO_MSG_OBJ_FREE);
}

li_can_slv_errorcode_t can_main_hw_enable(void)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if (CpCoreCanMode(&can_port_main, CANPIE_MODE_START) != eCP_ERR_NONE)
	{
		err = LI_CAN_SLV_ERR_NOT_IMPLEMENTED;
	}

	return err;
}

#ifdef LI_CAN_SLV_RECONNECT
li_can_slv_errorcode_t can_main_hw_enable_listen_only(void)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if (CpCoreCanMode(&can_port_main, CANPIE_MODE_LISTEN_ONLY) != eCP_ERR_NONE)
	{
		err = LI_CAN_SLV_ERR_NOT_IMPLEMENTED;
	}

	return err;
}
#endif // #ifdef LI_CAN_SLV_RECONNECT

li_can_slv_errorcode_t can_main_hw_disable(void)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if (CpCoreCanMode(&can_port_main, CANPIE_MODE_STOP) != eCP_ERR_NONE)
	{
		err = LI_CAN_SLV_ERR_NOT_IMPLEMENTED;
	}

	return err;
}

li_can_slv_errorcode_t can_main_hw_define_msg_obj(uint16_t msg_obj, uint16_t can_id, uint16_t acceptance_mask, byte_t dlc, byte_t dir, can_main_service_id_t service_id)
{
	uint8_t ubBufferIdxV;
	CpStatus_tv status;
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	enum CpBufferDir_e msg_dir;

	/* TODO: unused */	(void)dlc;
	ubBufferIdxV = (uint8_t) msg_obj;

#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
	LI_CAN_SLV_DEBUG_PRINT("hw def msgobj:\n");
	LI_CAN_SLV_DEBUG_PRINT(" obj:%d, id:%d\n", msg_obj, can_id);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW

	// set message object to used state
	msg_obj_used[msg_obj] = TRUE;


	if (dir == CAN_CONFIG_DIR_TX)
	{
		msg_dir = CANPIE_BUFFER_DIR_TX;
	}
	else
	{
		msg_dir = CANPIE_BUFFER_DIR_RX;
	}


	status = CpCoreBufferConfig(&can_port_main, ubBufferIdxV, can_id, acceptance_mask, CP_MSG_FORMAT_CBFF, msg_dir);

	if (eCP_ERR_NONE == status)
	{
		switch (service_id)
		{
			case CAN_MAIN_SERVICE_ID_TX:
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
				LI_CAN_SLV_DEBUG_PRINT(" SERVICE_ID_TX\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
				break;

			case CAN_MAIN_SERVICE_ID_RX:
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
				LI_CAN_SLV_DEBUG_PRINT(" SERVICE_ID_RX\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
				break;

			case CAN_MAIN_ASYNC_SERVICE_ID_TX:
			case CAN_MAIN_ASYNC_SERVICE_ID_RX:
#ifdef LI_CAN_SLV_BOOT
			case CAN_MAIN_ASYNC_CTRL_SERVICE_ID_RX:
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
				LI_CAN_SLV_DEBUG_PRINT(" ASYNC_CTRL_SERVICE_ID_RX\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
#endif // #ifdef LI_CAN_SLV_BOOT
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
				LI_CAN_SLV_DEBUG_PRINT(" ASYNC_SERVICE_ID_TX or RX\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
				break;
#if defined(OUTER) || defined(OUTER_APP)
			case CAN_MAIN_ASYNC_CTRL_SERVICE_ID_RX:
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
				LI_CAN_SLV_DEBUG_PRINT(" ASYNC_CTRL_SERVICE_ID_RX\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
				break;
#endif // #if defined(OUTER) || defined(OUTER_APP)

			default:
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
				LI_CAN_SLV_DEBUG_PRINT(" ERR_MSG_CAN_MAIN_UNDEFINED_ISR_ID\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
				err =  ERR_MSG_CAN_MAIN_UNDEFINED_ISR_ID;
				break;
		}
	}
	else
	{
		err = ERR_MSG_CAN_INIT_FAILED;
	}
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
	LI_CAN_SLV_DEBUG_PRINT("\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW

	return err;
}

li_can_slv_errorcode_t can_main_hw_set_baudrate(can_config_bdr_tab_t *bdr_tab_entry)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if (LI_CAN_SLV_MODE_INIT == li_can_slv_get_node_mode() || LI_CAN_SLV_MODE_STOPPED == li_can_slv_get_node_mode())
	{
		switch (bdr_tab_entry->baudrate)
		{
			case 125:
				CpCoreBitrate(&can_port_main, eCP_BITRATE_125K, eCP_BITRATE_NONE);
				break;

			case 250:
				CpCoreBitrate(&can_port_main, eCP_BITRATE_250K, eCP_BITRATE_NONE);
				break;

			case 500:
				CpCoreBitrate(&can_port_main, eCP_BITRATE_500K, eCP_BITRATE_NONE);
				break;

			case 1000:
				CpCoreBitrate(&can_port_main, eCP_BITRATE_1M, eCP_BITRATE_NONE);
				break;

			default:
				err = ERR_MSG_CAN_CONFIG_SET_INVALID_BAUDRATE;
				break;
		}
	}
	else
	{
		/* LI_CAN_SLV_MODE_INIT != li_can_slv_get_node_mode() */
		err = ERR_MSG_CAN_CONFIG_SET_INVALID_BAUDRATE;
	}

	return (err);
}

li_can_slv_errorcode_t can_main_hw_send_msg(uint16_t can_id, uint16_t dlc, const volatile byte_t *data)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	CpCanMsg_ts can_msg = {0};
	uint32_t tx_cnt;
	CpStatus_tv ret;

	CpMsgClear(&can_msg);
	CpMsgInit(&can_msg, CP_MSG_FORMAT_CBFF);

	CpMsgSetDlc(&can_msg, dlc);
	CpMsgSetIdentifier(&can_msg, can_id);

	li_can_slv_port_memory_cpy(&can_msg.tuMsgData, (uint8_t *)data, dlc);

	tx_cnt = 1;
	ret = CpCoreFifoWrite(&can_port_main, li_can_slv_sync_main_tx_msg_obj,  &can_msg, &tx_cnt);

	if (eCP_ERR_NONE != ret)
	{
		err = ERR_MSG_CAN_MSG_SEND;
	}

	return err;
}

li_can_slv_errorcode_t can_main_hw_reserve_msg_obj(uint16_t msg_obj)
{
	li_can_slv_errorcode_t err;

	err = ERR_MSG_CAN_NO_MSG_OBJ_FREE;

	if ((msg_obj < LI_CAN_SLV_MAIN_NODE_MAX_NOF_MSG_OBJ) && (FALSE == msg_obj_used[msg_obj]))
	{
		msg_obj_used[msg_obj] = TRUE;
		err = LI_CAN_SLV_ERR_OK;
	}

	return err;
}

/** @} */
