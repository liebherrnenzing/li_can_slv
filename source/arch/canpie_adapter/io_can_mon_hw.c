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
 * @file canpie_adapter/io_can_mon_hw.c
 * @addtogroup lcs_arch_canpie_adapter
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>

#include "io_can_mon_hw.h"

#ifdef LI_CAN_SLV_MON

#ifdef LI_CAN_SLV_SYNC
#include <li_can_slv/sync/io_can_sync.h>
#endif // #ifdef LI_CAN_SLV_SYNC

#include <li_can_slv/core/io_can_mon.h>
#include <li_can_slv/core/io_can.h>

#include <li_can_slv/error/io_can_error.h>
#include <li_can_slv/error/io_can_errno.h>

#include <li_can_slv/config/io_can_config_types.h>
#include "io_can_mon_hw_handler.h"

#include "canpie.h"
#include "cp_core.h"
#include "cp_msg.h"

#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_debug.h"
#endif // #ifdef LI_CAN_SLV_DEBUG

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#define CANPIE_MODE_START	(eCP_MODE_START)
#define CANPIE_MODE_STOP	(eCP_MODE_STOP)
#define CANPIE_MODE_LISTEN_ONLY	(eCP_MODE_LISTEN_ONLY)
#define CANPIE_BUFFER_DIR_TX	(eCP_BUFFER_DIR_TRM)
#define CANPIE_BUFFER_DIR_RX	(eCP_BUFFER_DIR_RCV)

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/
CpPort_ts can_port_mon; // CAN interface

const can_config_bdr_tab_t can_mon_hw_default_baudrate_table[CAN_CONFIG_SIZE_OF_BDR_TAB] = /**< baud rate table */
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
static uint8_t mon_msg_obj_used[LI_CAN_SLV_MON_NODE_MAX_NOF_MSG_OBJ];

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t can_mon_hw_init(void)
{
	CpCoreDriverInit(LI_CAN_SLV_MON_ARCH, &can_port_mon, 0);
	CpCoreIntFunctions(&can_port_mon, can_mon_hw_handler_rx, 0L, 0L);

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_mon_hw_deinit(void)
{
	CpCoreDriverRelease(&can_port_mon);

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_mon_hw_msg_obj_init(uint16_t msg_obj)
{
	mon_msg_obj_used[msg_obj] = FALSE;
	CpCoreBufferRelease(&can_port_mon, msg_obj);

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_mon_hw_get_next_free_msg_obj(uint16_t *msg_obj)
{
	uint16_t i;

#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
	LI_CAN_SLV_DEBUG_PRINT("can_hw_get_nex_free_msg_obj\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW

	for (i = 0; i < LI_CAN_SLV_MON_NODE_MAX_NOF_MSG_OBJ; i++)
	{
		if (mon_msg_obj_used[i] == FALSE)
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

li_can_slv_errorcode_t can_mon_hw_enable(void)
{
	li_can_slv_node_mode_t mode;

	mode = li_can_slv_get_node_mode();
	if (LI_CAN_SLV_MODE_LISTEN_ONLY == mode)
	{
		CpCoreCanMode(&can_port_mon, CANPIE_MODE_LISTEN_ONLY);
	}
	else
	{
		CpCoreCanMode(&can_port_mon, CANPIE_MODE_START);
	}
	return (LI_CAN_SLV_ERR_OK);
}

#ifdef LI_CAN_SLV_RECONNECT
li_can_slv_errorcode_t can_mon_hw_enable_listen_only(void)
{
	CpCoreCanMode(&can_port_mon, CANPIE_MODE_LISTEN_ONLY);
	return (LI_CAN_SLV_ERR_OK);
}
#endif // #ifdef LI_CAN_SLV_RECONNECT

li_can_slv_errorcode_t can_mon_hw_disable(void)
{
	CpCoreCanMode(&can_port_mon, CANPIE_MODE_STOP);

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_mon_hw_define_msg_obj(uint16_t msg_obj, uint16_t can_id, uint16_t acceptance_mask, byte_t dlc, byte_t dir, can_mon_service_id_t service_id)
{
	uint8_t ubBufferIdxV;
	CpStatus_tv status;
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	enum CpBufferDir_e msg_dir;

	ubBufferIdxV = (uint8_t) msg_obj;

#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
	LI_CAN_SLV_DEBUG_PRINT("can hw def msgobj:%d, id:%d\n", msg_obj, can_id);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW

	// set message object to used state
	mon_msg_obj_used[msg_obj] = TRUE;

	if (dir == CAN_CONFIG_DIR_TX)
	{
		msg_dir = CANPIE_BUFFER_DIR_TX;
	}
	else
	{
		msg_dir = CANPIE_BUFFER_DIR_RX;
	}

	status = CpCoreBufferConfig(&can_port_mon, ubBufferIdxV, can_id, acceptance_mask, CP_MSG_FORMAT_CBFF, msg_dir);

	if (eCP_ERR_NONE == status)
	{
		switch (service_id)
		{
			case CAN_MON_ISR_ID_RX:
			case CAN_MON_ISR_ID_RX_MAIN:
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
				LI_CAN_SLV_DEBUG_PRINT(" with CAN_MON_SERVICE_ID_TX\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
				break;
			default:
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
				LI_CAN_SLV_DEBUG_PRINT(" with ERR_MSG_CAN_MON_UNDEFINED_ISR_ID\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_HW
				err = ERR_MSG_CAN_MON_UNDEFINED_ISR_ID;
				break;
		}
	}
	else
	{
		err = ERR_MSG_CAN_INIT_FAILED;
	}
	return err;
}

li_can_slv_errorcode_t can_mon_hw_set_baudrate(can_config_bdr_tab_t *bdr_tab_entry)
{
	CpCoreCanMode(&can_port_mon, CANPIE_MODE_STOP);

	switch (bdr_tab_entry->baudrate)
	{
		case 125:
			CpCoreBitrate(&can_port_mon, eCP_BITRATE_125K, eCP_BITRATE_NONE);
			break;

		case 250:
			CpCoreBitrate(&can_port_mon, eCP_BITRATE_250K, eCP_BITRATE_NONE);
			break;

		case 500:
			CpCoreBitrate(&can_port_mon, eCP_BITRATE_500K, eCP_BITRATE_NONE);
			break;

		case 1000:
			CpCoreBitrate(&can_port_mon, eCP_BITRATE_1M, eCP_BITRATE_NONE);
			break;

		default:
			CpCoreCanMode(&can_port_mon, CANPIE_MODE_START);
			return (ERR_MSG_CAN_CONFIG_SET_INVALID_BAUDRATE);
			break;
	}

	if (li_can_slv_get_node_mode() == LI_CAN_SLV_MODE_OPERATIONAL)
	{
		CpCoreCanMode(&can_port_mon, CANPIE_MODE_START);
	}
	else
	{
		CpCoreCanMode(&can_port_mon, CANPIE_MODE_LISTEN_ONLY);
	}

	return (LI_CAN_SLV_ERR_OK);
}
#endif // #ifdef LI_CAN_SLV_MON
/** @} */
