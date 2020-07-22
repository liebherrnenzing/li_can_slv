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
 * @file io_can_sync_handler.c
 * @addtogroup can_sync
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/sync/io_can_sync_handler.h>
#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/error/io_can_error.h>
#include <li_can_slv/core/io_can_main.h>

#include <li_can_slv/core/io_can.h>

#ifdef LI_CAN_SLV_ASYNC
#include <li_can_slv/async/io_can_async.h>
#endif // #ifdef LI_CAN_SLV_ASYNC

#include <li_can_slv/config/io_can_config.h>

#ifdef LI_CAN_SLV_SYNC
#include <li_can_slv/sync/io_can_sync.h>
#endif // #ifdef LI_CAN_SLV_SYNC

#include <li_can_slv/sys/io_can_sys.h>
#include <li_can_slv/core/io_can_main_handler.h>

#include <li_can_slv/arch/io_can_main_hw_inline.h>

#ifdef LI_CAN_SLV_MON
#include <li_can_slv/core/io_can_mon.h>
#include "io_can_mon_hw.h"
#include <li_can_slv/arch/io_can_mon_hw_inline.h>
#endif // #ifdef LI_CAN_SLV_MON

#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_debug.h"
#endif // #ifdef LI_CAN_SLV_DEBUG

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/

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

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
#if defined (LI_CAN_SLV_SYNC) || defined (LI_CAN_SLV_BOOT)
uint8_t can_sync_handler_rx(uint16_t msg_obj, uint8_t dlc, uint16_t canid, uint8_t *data)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint32_t rxi;
#ifndef LI_CAN_SLV_BOOT
#if defined(CAN_MAIN_SYSTEM_MSG_TX_QUEUE)
	uint32_t txi;
#endif // #if defined(CAN_MAIN_SYSTEM_MSG_TX_QUEUE)
#endif // #ifndef LI_CAN_SLV_BOOT
	li_can_slv_module_nr_t module_nr;

#if defined(OUTER) || defined(OUTER_APP)
	uint16_t table_pos;
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
	msg_code_t errnum;
	err_prio_t priority;
	byte_t add_info;
	byte_t group_subgroup;
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
#endif // #if defined(OUTER) || defined(OUTER_APP)
#ifdef LI_CAN_SLV_DEBUG_SYNC_RX_DATA
	uint16_t i;
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_RX_DATA

	rxi = msg_obj;
#ifndef LI_CAN_SLV_BOOT
#if defined(CAN_MAIN_SYSTEM_MSG_TX_QUEUE)
	txi = rxi;
#endif // #if defined(CAN_MAIN_SYSTEM_MSG_TX_QUEUE)
#endif // #ifndef LI_CAN_SLV_BOOT

	module_nr = ((canid >> 2) & 0x007F) + 1;

	//	if ((((1L << rxi) | (1L << txi)) & can_main_objs_mask) != 0)
	//	{
#if defined(OUTER) || defined(OUTER_APP)
	// synchrony data received
	if ((1L << rxi) & can_main_isr_inp_mask_rx)
	{
		err = can_main_synchron_rx((1L << rxi), &msg_obj, &table_pos);
#ifdef LI_CAN_SLV_DEBUG_SYNC_RX
		LI_CAN_SLV_DEBUG_PRINT("\nnode a sync rx: %lu", rxi);
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_RX

#ifdef LI_CAN_SLV_DEBUG_SYNC_RX_EXTENDED
		LI_CAN_SLV_DEBUG_PRINT(" msg_obj: %d, table_pos: %d,", msg_obj, table_pos);
		LI_CAN_SLV_DEBUG_PRINT(" err: %04x", err);
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_RX_EXTENDED
		if (err == LI_CAN_SLV_ERR_OK)
		{
#ifdef LI_CAN_SLV_DEBUG_SYNC_RX_EXTENDED
			LI_CAN_SLV_DEBUG_PRINT(" id: %x", canid);
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_RX_EXTENDED

			// call synchrony receive routine of main CAN controller
			can_sync_rx_data_main(table_pos, canid, dlc, data);
#ifdef CAN_MAIN_DIAGNOSE
			can_main_diagnose.syncrx++;
#endif // #ifdef CAN_MAIN_DIAGNOSE

#ifdef LI_CAN_SLV_DEBUG_SYNC_RX_DATA
			LI_CAN_SLV_DEBUG_PRINT("\ndata: ");
			for (i = 0; i < 8; ++i)
			{
				LI_CAN_SLV_DEBUG_PRINT("%x ", data[i]);
			}
			LI_CAN_SLV_DEBUG_PRINT("\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_RX_DATA
		}

#ifdef LI_CAN_SLV_DEBUG_SYNC_RX
		LI_CAN_SLV_DEBUG_PRINT("\n\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_RX

	}
#endif // #if defined(OUTER) || defined(OUTER_APP)

#if defined(OUTER) || defined(OUTER_APP)
	// process request received
	if (rxi == CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS)
	{
#ifdef LI_CAN_SLV_DEBUG_MAIN_PROCESS_HANDLER
		LI_CAN_SLV_DEBUG_PRINT("\nnode a process rx start");
#endif // #ifdef LI_CAN_SLV_DEBUG_MAIN_PROCESS_HANDLER

		msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS;

		li_can_slv_sync_process_request_rx();

#ifdef LI_CAN_SLV_MAIN_MON
		if (can_mainmon_type == CAN_MAINMON_TYPE_MAIN)
		{
#endif // #ifdef LI_CAN_SLV_MAIN_MON
			// send transmit data as fast as possible
			can_main_process_data_tx_set();
#ifdef LI_CAN_SLV_MAIN_MON
		}
#endif // #ifdef LI_CAN_SLV_MAIN_MON

		li_can_slv_sync_check_process_image();
		can_sync_rx_process_main(dlc, data);

#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
		// send system error if valid, one error per process, (sending only one error -> hold synchrony process)
		if (error_syserr_num() > 0)
		{
			err = error_syserr_get_full(&errnum, &add_info, &priority, &group_subgroup, &module_nr);
			if (err == LI_CAN_SLV_ERR_OK)
			{
				err = can_sys_send_error_full(module_nr, errnum, add_info, priority, group_subgroup);
			}
		}
#ifdef LI_CAN_SLV_DEBUG_MAIN_PROCESS_HANDLER
		LI_CAN_SLV_DEBUG_PRINT("\nnode a process rx end");
#endif // #ifdef LI_CAN_SLV_DEBUG_MAIN_PROCESS_HANDLER
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
	}
#endif // #if defined(OUTER) || defined(OUTER_APP)

	// system message received
#ifdef LI_CAN_SLV_BOOT
	if (rxi == CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS1)
#endif //#ifdef LI_CAN_SLV_BOOT
#if defined(OUTER) || defined(OUTER_APP)
		if ((rxi == CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS1) || (rxi == CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS2))
#endif // #if defined(OUTER) || defined(OUTER_APP)
		{
#ifdef LI_CAN_SLV_DEBUG_MAIN_SYS_HANDLER
			LI_CAN_SLV_DEBUG_PRINT("\nnode a sys rx");
#endif // #ifdef LI_CAN_SLV_DEBUG_MAIN_SYS_HANDLER

#ifdef LI_CAN_SLV_BOOT
			msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS1;
#endif // #ifdef LI_CAN_SLV_BOOT

			// handling of system message
#ifdef LI_CAN_SLV_DEBUG_MAIN_SYS_HANDLER_EXTENDED
			LI_CAN_SLV_DEBUG_PRINT(" , md_nr: %d, dlc: %d\n", module_nr, dlc);
#endif // #if LI_CAN_SLV_DEBUG_MAIN_SYS_HANDLER_EXTENDED

			err = can_sys_msg_rx(module_nr, dlc, data);

#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
			if (err != LI_CAN_SLV_ERR_OK)
			{
#ifdef LI_CAN_SLV_BOOT
				error_syserr_send(err, ERR_LVL_INFO, can_config_get_module_nr(), ERR_LVL_INFO);
#endif // #ifdef LI_CAN_SLV_BOOT
#if defined(OUTER) || defined(OUTER_APP)
				error_syserr_send(err, ERR_LVL_INFO, can_config_get_module_nr_main(), ERR_LVL_INFO);
#endif // #if defined(OUTER) || defined(OUTER_APP)
			}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR

#ifdef CAN_MAIN_DIAGNOSE
			can_main_diagnose.sysrx++;
#endif // #ifdef CAN_MAIN_DIAGNOSE
		}

#ifdef LI_CAN_SLV_BOOT
	// asynchronous control data received on obj_async_ctrl1
	// handling of async ctrl rx messages in outer core is done in a separate task
	if (rxi == CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL1_RX)
	{
		// asynchronous control data received on obj_async_ctrl1
		msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL1_RX;

		// recalculate module_nr for ASYNC
		module_nr = ((canid - CAN_CONFIG_ASYNC_CTRL_RX_SLAVE_ID) >> 2) + 1;

		err = can_async_send_data_to_async_ctrl_rx_queue(module_nr, data);

#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
		if (err != LI_CAN_SLV_ERR_OK)
		{
			error_syserr_send(err, ERR_LVL_INFO, can_config_get_module_nr(), ERR_LVL_INFO);
		}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
	}
#endif // #ifdef LI_CAN_SLV_BOOT

	/**
	 * @todo i think this could be a bug so the define is correct i think
	 * */
#if defined(CAN_MAIN_SYSTEM_MSG_TX_QUEUE)
	// system message sent
	if (txi == CAN_CONFIG_MSG_MAIN_OBJ_TX_SYS)
	{
		msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_TX_SYS;
		// request to the CAN tx task it is possible that an other system message should be send
		can_main_handler_tx(410);
#ifdef CAN_MAIN_DIAGNOSE
		can_main_diagnose.systx++;
#endif // #ifdef CAN_MAIN_DIAGNOSE
	}
#endif // #if defined(CAN_MAIN_SYSTEM_MSG_TX_QUEUE)

#if defined(CAN_ASYNC_CTRL_TX_QUEUE)
	// asynchronous control data and asynchronous data sent
	if (txi & (1L << CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_TX))
	{
		msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_TX;
		// request to the CAN tx task
		can_main_handler_tx(420);
#ifdef CAN_MAIN_DIAGNOSE
		can_main_diagnose.asynctx++;
#endif // #ifdef CAN_MAIN_DIAGNOSE
	}
#endif // #if defined(CAN_ASYNC_CTRL_TX_QUEUE)
	//	}

#if defined(OUTER) || defined(OUTER_APP)
#ifdef LI_CAN_SLV_ASYNC
#if defined(LI_CAN_SLV_ASYNC_TUNNEL)
	// process partly filled async tx data objects for a timed sending
	can_async_tunnel_process_tx_data();
#endif // #if defined(LI_CAN_SLV_ASYNC_TUNNEL)
#endif // #ifdef LI_CAN_SLV_ASYNC
#endif // #if defined(OUTER) || defined(OUTER_APP)
	return 0;
}
#endif // #if defined (LI_CAN_SLV_SYNC) || defined (LI_CAN_SLV_BOOT)

#if defined (LI_CAN_SLV_SYNC)
uint8_t can_sync_handler_tx(uint16_t msg_obj, uint8_t dlc, uint16_t canid)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint32_t rxi;
#ifndef LI_CAN_SLV_BOOT
	uint32_t txi;
#endif // #ifndef LI_CAN_SLV_BOOT
#if defined(OUTER) || defined(OUTER_APP)
	uint16_t table_pos;
	uint16_t obj;
#endif // #if defined(OUTER) || defined(OUTER_APP)

	(void)dlc;
	rxi = msg_obj;
#ifndef LI_CAN_SLV_BOOT
	txi = rxi;
#endif // #ifndef LI_CAN_SLV_BOOT

	//	if ((((1L << rxi) | (1L << txi)) & can_main_objs_mask) != 0)
	//	{
#if defined(OUTER) || defined(OUTER_APP)
	// sync tx objects
	if ((1L << txi) & can_main_isr_inp_mask_tx)
	{
		err = can_main_synchron_tx((1L << txi), &msg_obj, &table_pos);
#ifdef LI_CAN_SLV_DEBUG_SYNC_TX
		LI_CAN_SLV_DEBUG_PRINT("\nnode a sync tx: %lu", rxi);
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_TX

#ifdef LI_CAN_SLV_DEBUG_SYNC_TX_EXTENDED
		LI_CAN_SLV_DEBUG_PRINT(" msg_obj: %d, table_pos: %d,", msg_obj, table_pos);
		LI_CAN_SLV_DEBUG_PRINT(" err: %04x,", err);
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_TX_EXTENDED
		if (err == LI_CAN_SLV_ERR_OK)
		{
			// calculate obj number from transmitted CAN ID
			obj = canid & CAN_SYNC_OBJ_MASK;
#ifdef LI_CAN_SLV_DEBUG_SYNC_TX_EXTENDED
			LI_CAN_SLV_DEBUG_PRINT(" id: %x, obj: %d", canid, obj);
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_TX_EXTENDED
			can_sync_tx_data_main_ok(table_pos, obj);
#ifdef CAN_MAIN_DIAGNOSE
			can_main_diagnose.synctx++;
#endif // #ifdef CAN_MAIN_DIAGNOSE
		}
#ifdef LI_CAN_SLV_DEBUG_SYNC_TX_EXTENDED
		else
		{
			LI_CAN_SLV_DEBUG_PRINT(" error");
		}
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_TX_EXTENDED

#if defined(OUTER) || defined(OUTER_APP) || defined(CAN_MAIN_SYSTEM_MSG_TX_QUEUE) || defined(CAN_ASYNC_CTRL_TX_QUEUE)
		// request to the CAN tx task
		can_main_handler_tx(197);
#endif // #if defined(OUTER) || defined(OUTER_APP) || defined(CAN_MAIN_SYSTEM_MSG_TX_QUEUE) || defined(CAN_ASYNC_CTRL_TX_QUEUE)
	}
#endif // #if defined(OUTER) || defined(OUTER_APP)
	//	}
	return 0;
}
#endif // #if defined (LI_CAN_SLV_SYNC)

#if defined (LI_CAN_SLV_SYNC) && defined (LI_CAN_SLV_MON)
uint8_t can_sync_handler_rx_mon(uint16_t msg_obj, uint8_t dlc, uint16_t canid, uint8_t *data)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint32_t rxi;
#if defined(OUTER) || defined(OUTER_APP)
	uint16_t table_pos;
#endif // #if defined(OUTER) || defined(OUTER_APP)
#ifdef LI_CAN_SLV_DEBUG
	uint16_t i;
#endif // #ifdef LI_CAN_SLV_DEBUG
	rxi = msg_obj;

	//	if (((1L << rxi) & can_mon_objs_mask) != 0)
	//	{
#if defined(LI_CAN_SLV_RECONNECT) && defined(LI_CAN_SLV_RECONNECT_MON_NODE)
	// probably it was a valid message
	li_can_slv_reconnect_mon_node_msg_pending = CAN_INTID_MSG;
#endif // #if defined(LI_CAN_SLV_RECONNECT) && defined(LI_CAN_SLV_RECONNECT_MON_NODE)

#ifndef CAN_NODE_B_USED_FOR_RECONNECT_ONLY
	// synchronous data received from master
	if ((1L << rxi) & can_mon_isr_inp_mask_rx)
	{
		err = can_mon_synchron_rx((1L << rxi), &msg_obj, &table_pos);
#ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX
		LI_CAN_SLV_DEBUG_PRINT("\nnode b sync rx: %lu", rxi);
#endif // #ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX
#ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX_EXTENDED
		LI_CAN_SLV_DEBUG_PRINT(" msg_obj: %d, table_pos: %d,", msg_obj, table_pos);
		LI_CAN_SLV_DEBUG_PRINT(" err: %04x, id:%x", err, canid);
#endif // #ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX_EXTENDED

		if (err == LI_CAN_SLV_ERR_OK)
		{

#ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX_EXTENDED
			LI_CAN_SLV_DEBUG_PRINT(" id: %x", canid);
#endif // #ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX_EXTENDED

			// call synchronous receive routine of don CAN controller
			can_sync_rx_data_mon(table_pos, canid, dlc, data);

#ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX_DATA
			LI_CAN_SLV_DEBUG_PRINT("\ndata_mon: ");
			for (i = 0; i < 8; ++i)
			{
				LI_CAN_SLV_DEBUG_PRINT("%x ", data[i]);
			}
			LI_CAN_SLV_DEBUG_PRINT("\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX_DATA
		}

#ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX
		LI_CAN_SLV_DEBUG_PRINT("\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX
	}
#endif // #ifndef CAN_NODE_B_USED_FOR_RECONNECT_ONLY

#ifndef CAN_NODE_B_USED_FOR_RECONNECT_ONLY
	// synchronous data received from main CAN controller
	if ((1L << rxi) & can_mon_isr_inp_mask_rx_main)
	{
		err = can_mon_synchron_tx((1L << rxi), &msg_obj, &table_pos);
#ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX_FROM_MAIN
		LI_CAN_SLV_DEBUG_PRINT("\nnode b sync rx from main");
#endif // #ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX_FROM_MAIN
#ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX_FROM_MAIN_EXTENDED
		LI_CAN_SLV_DEBUG_PRINT(" msg_obj: %d, table_pos: %d,", msg_obj, table_pos);
		LI_CAN_SLV_DEBUG_PRINT(" err: %04x,", err);
#endif // #ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX_FROM_MAIN_EXTENDED

		if (err == LI_CAN_SLV_ERR_OK)
		{
#ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX_FROM_MAIN_EXTENDED
			LI_CAN_SLV_DEBUG_PRINT(" id: %x", canid);
#endif // #ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX_FROM_MAIN_EXTENDED

			// call synchrony receive routine of mon CAN controller
			can_sync_tx_data_mon(table_pos, canid, dlc, data);

#ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX_FROM_MAIN_DATA
			LI_CAN_SLV_DEBUG_PRINT("\ndata_mon: ");
			for (i = 0; i < 8; ++i)
			{
				LI_CAN_SLV_DEBUG_PRINT("%x ", data[i]);
			}
			LI_CAN_SLV_DEBUG_PRINT("\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_MON_SYNC_RX_FROM_MAIN_DATA
		}
	}
#endif // #ifndef CAN_NODE_B_USED_FOR_RECONNECT_ONLY

#if defined(LI_CAN_SLV_MON) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY_SYSTEM_REQ)
	/*------------------------------------------------------------------*/
	/* system message received                                          */
	/*------------------------------------------------------------------*/
	if (rxi == CAN_CONFIG_MSG_MON_OBJ_RX_SYS1)
	{
#ifdef LI_CAN_SLV_DEBUG_MON_SYS_RX_HANDLER
		LI_CAN_SLV_DEBUG_PRINT("\nnode b sys rx");
#endif // #ifdef LI_CAN_SLV_DEBUG_MON_SYS_RX_HANDLER
	}
#endif // #if defined(LI_CAN_SLV_MON) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY_SYSTEM_REQ)

#if !defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY_PROCESS_REQ)
	// process request received
	if (rxi == CAN_CONFIG_MSG_MON_OBJ_RX_PROCESS)
	{
#ifdef LI_CAN_SLV_DEBUG_MON_PROCESS_HANDLER
		LI_CAN_SLV_DEBUG_PRINT("\nnode b process rx start");
#endif // #ifdef LI_CAN_SLV_DEBUG_MON_PROCESS_HANDLER

#ifndef CAN_NODE_B_USED_FOR_RECONNECT_ONLY
		li_can_slv_sync_check_process_image();
		can_sync_rx_process_mon(dlc, data);
#endif // #ifndef CAN_NODE_B_USED_FOR_RECONNECT_ONLY

#ifdef LI_CAN_SLV_DEBUG_MON_PROCESS_HANDLER
		LI_CAN_SLV_DEBUG_PRINT("\nnode b process rx end");
#endif // #ifdef LI_CAN_SLV_DEBUG_MON_PROCESS_HANDLER

	}
#endif // #if !defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY_PROCESS_REQ)
	//	}
	return 0;
}
#endif // #if defined (LI_CAN_SLV_SYNC) && defined (LI_CAN_SLV_MON)

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/** @} */
