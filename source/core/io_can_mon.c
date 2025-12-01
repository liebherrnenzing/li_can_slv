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
 * \remarks	Includes basic driver functions like initialization, enable and disable the monitor CAN-controller.\n
			The monitor CAN-controller is generally used to verifying the correct working of the main CAN-controller.\n
			Anathor abandonment auf the monitor CAN-controller is to measure the current CAN baudrate.

			- <b>initialization</b>\n
			Only the receive direction are connected to the CAN-transceiver. If the monitor CAN-controller\n
			is enabled, it will not be influenced the CAN bus.

			- <b>interrupt service routine</b>\n
			Includes the interrupt service routine of the monitor CAN-controller. Identification of the reason of the interrupt\n
			\arg	valid can message detected
			\arg	error frame
			\arg	error warning
			\arg	bus off

			- <b>CAN-ISR and the system state</b>\n
			The behaviour of the monitor interrupt service routine depends on the system state.\n
			\arg	Booting - Waiting for first CAN event (deactivation process by arbitration)\n
					No tansmission of any data are allowed.\n
			\arg	System Critical\n
					No tansmission of synchron data are allowed.\n
			\arg	System Alert (disable CAN controller)\n
					No tansmission of any data are allowed.\n
			\arg	System FATAL (disable CAN controller)\n
					No tansmission of any data are allowed.\n

			Also some conditions on the monitor interrupt service routine changes the system state.\n
			\arg	Process running\n
					No timeout conditions. Set system state to Process running.\n
			\arg	Timeout\n
					Timeout conditions. Set system state to Timeout.\n
			\arg	Change CAN baudrate\n
					Set system state to Change CAN baudrate.\n
			\arg	Reconnect Startup\n
					Set system state to Reconnect Startup.\n
			\arg	Reconnect Online\n
					Set system state to Reconnect Online.\n
			\arg	Bus off\n
					Set system state to Bus off.\n
			\arg	Silent mode\n
					Set system state to Silent mode.\n

			Also the distribution of the CAN messages (definition in io_can_config) is implemented in the\n
			interrupt service routine of the monitor CAN-controller.\n
			\arg	software interrupt (50ms)\n
					In case of no CAN event at least every 50ms a software interrupt request is send to the\n
					interrupt service routine of the monitor CAN-controller. This is used to check the synchron\n
					process an set the timeout condition of ech logical module.\n The same software interrupt request (50ms)\n
					is used for the reconnect routine.\n
			\arg	prozess request\n
					The monitor CAN-controller helps to prove the synchrone process of the module via CAN.\n
					The complete process image must be correct:\n
						- only one process request in the right format detected\n
						- all rx data only one time and in the right format detected\n
			\arg	error frame\n
					An error frame is directly processed in the interrupt service routine. The monitor CAN-controller\n
					is also used to measure the actual CAN baudrate. Therefore the monitor CAN-controller is initialized\n
					with different CAN baudrates an testet by enabling the CAN-controller. If a valid message received\n
					the current baudrate on the CAN bus is detected. If an error frame is detected change to the next baudrate.\n
			\arg	error warning\n
					If error warning is active by call the interrupt service routine, the reconnect routines are started.\n
					See	therefore io_can_reconnect.
			\arg	bus off\n
					If bus off is detected a recovery sequence is started to activate the monitor CAN-controller once again.\n
					See therefore io_can_reconnect.\n\n

			- <b>can status</b>\n
			The CAN status register of the monitor CAN-controller is analyzed by receiving a can message (RxOK flag).\n

			- <b>CAN baudrate</b>\n
			There are up to 5 different CAN baudrates possible. The monitor CAN-controller must be have at any time the\n
			same bit timing register set as the main CAN-controller. This condition is proved in process safty time.\n

	\pre
	\test	To set a not valid CAN baudrate must be detected.\n

 */

/**
* @file io_can_mon.c
* @brief
* @addtogroup can_driver_mon
* @{
*/

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/core/io_can_mon.h>
#include "io_can_mon_hw.h"
#include "io_can_hw.h"

#ifdef LI_CAN_SLV_RECONNECT
#include <li_can_slv/core/io_can_reconnect.h>
#endif // #ifdef LI_CAN_SLV_RECONNECT

#include <li_can_slv/sync/io_can_sync.h>

#include <li_can_slv/core/io_can.h>
#include <li_can_slv/error/io_can_errno.h>
#ifdef SHOW_CAN_MON
#include "io_var.h"
#ifdef TESTSYS_SYSTEM
#include "io_testsys.h"
#endif // #ifdef TESTSYS_SYSTEM
#endif // #ifdef SHOW_CAN_MON

#ifdef IO_SMP
#include "io_smp.h"
#endif // #ifdef IO_SMP

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
#ifdef CAN_MON_DIAGNOSE
can_mon_diagnose_t can_mon_diagnose = {0};
#endif // #ifdef CAN_MON_DIAGNOSE

/* interrupt pending masks */
volatile dword_t can_mon_isr_inp_mask_rx = 0x00000000L; /*!< interrupt pending mask receive for can_mon_isr */
volatile dword_t can_mon_isr_inp_mask_rx_main = 0x00000000L; /*!< interrupt pending mask transmit for can_mon_isr */
volatile dword_t can_mon_objs_mask = 0x00000000L; /*!< mask that identifies all slots the CAN controller is configured for */

can_mon_rx_msg_obj_t can_mon_rx_msg_obj[CAN_CONFIG_SYNC_MON_MAX_NR_OF_RX_OBJ] = {0};
uint16_t can_mon_rx_msg_obj_used = 0;

can_mon_tx_msg_obj_t can_mon_tx_msg_obj[CAN_CONFIG_SYNC_MON_MAX_NR_OF_TX_OBJ] = {0};
uint16_t can_mon_tx_msg_obj_used = 0;

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/
#ifdef SHOW_CAN_MON
/*--------------------------------------------------------------------------*/
/* system variable CAN monitor                                              */
/*--------------------------------------------------------------------------*/
static var_change_t can_mon_var_change[] =
{
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},

	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},

	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
};

#ifdef CAN_MON_DIAGNOSE
static const var_const_t can_mon_var_const[] =
{
	{"mon.rxok",	&can_mon_diagnose.rxok,		VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,					&can_mon_var_change[0]},
	{"mon.txok",	&can_mon_diagnose.txok,		VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,					&can_mon_var_change[1]},
	{"mon.err",		&can_mon_diagnose.err,		VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,					&can_mon_var_change[2]},
#ifdef IO_SMP
	{"mon.ewrn",	&can_mon_diagnose.ewrn,		VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	HNDLPOS_CAN_MON_EWRN,	&can_mon_var_change[3]},

	{"mon.boff",	&can_mon_diagnose.boff,		VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	HNDLPOS_CAN_MON_BOFF,	&can_mon_var_change[4]},
#else // #ifdef IO_SMP
	{"mon.ewrn",	&can_mon_diagnose.ewrn,		VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,					&can_mon_var_change[3]},

	{"mon.boff",	&can_mon_diagnose.boff,		VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,					&can_mon_var_change[4]},
#endif // #ifdef IO_SMP
	{"mon.stuff",	&can_mon_diagnose.lec[0],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,					&can_mon_var_change[5]},
	{"mon.form",	&can_mon_diagnose.lec[1],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,					&can_mon_var_change[6]},
	{"mon.ackn",	&can_mon_diagnose.lec[2],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,					&can_mon_var_change[7]},

	{"mon.bit1",	&can_mon_diagnose.lec[3],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,					&can_mon_var_change[8]},
	{"mon.bit0",	&can_mon_diagnose.lec[4],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,					&can_mon_var_change[9]},
	{"mon.crc",		&can_mon_diagnose.lec[5],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,					&can_mon_var_change[10]},
	{"mon.pr",		&can_mon_diagnose.pr,		VAR_UINT16, 0, 	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,					&can_mon_var_change[11]},
	{VAR_END_OF_TABLE}
};
#endif // #ifdef CAN_MON_DIAGNOSE

#ifdef CAN_MON_DIAGNOSE
/*--------------------------------------------------------------------------*/
/* IO-TESTSYS-PAGE system CAN monitor                                       */
/*--------------------------------------------------------------------------*/
static const testsys_var_t can_mon_testsys[] =
{
	{&can_mon_var_const[0],		1,	1},
	{&can_mon_var_const[1],		1,	2},
	{&can_mon_var_const[2],		1,	3},
	{&can_mon_var_const[3],		1,	4},

	{&can_mon_var_const[4],		1,	5},
	{&can_mon_var_const[5],		1,	6},
	{&can_mon_var_const[6],		1,	7},
	{&can_mon_var_const[7],		1,	8},

	{&can_mon_var_const[8],		1,	9},
	{&can_mon_var_const[9],		1,	10},
	{&can_mon_var_const[10],	1,	11},
	{&can_mon_var_const[11],	1,	12},
	{TESTSYS_VAR_END_OF_TABLE,	0,	0}
};
#endif // #ifdef SHOW_CAN_MON
#endif // #ifdef CAN_MON_DIAGNOSE

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t can_mon_init(void)
{
	li_can_slv_errorcode_t err;

	err = can_mon_hw_init();
#ifdef CAN_MON_DIAGNOSE
#ifdef SHOW_CAN_MON
	/*----------------------------------------------------------------------*/
	/* add can monitor IO's                                                 */
	/*----------------------------------------------------------------------*/
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = var_add(&can_mon_var_const[0]);
	}

	/*----------------------------------------------------------------------*/
	/* add can monitors IO's to testsys                                     */
	/*----------------------------------------------------------------------*/
#ifdef TESTSYS_SYSTEM
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = testsys_system_page_add(&can_mon_testsys[0], "CAN Node b controller system");
	}
#endif // #ifdef TESTSYS_SYSTEM
#endif // #ifdef SHOW_CAN_MON
#endif // #ifdef CAN_MON_DIAGNOSE
	return err;
}

li_can_slv_errorcode_t can_mon_deinit(void)
{
	can_mon_isr_inp_mask_rx = 0x00000000L;
	can_mon_isr_inp_mask_rx_main = 0x00000000L;
	can_mon_objs_mask = 0x00000000L;

	li_can_slv_port_memory_set(can_mon_rx_msg_obj, 0x00, sizeof(can_mon_rx_msg_obj));
	can_mon_rx_msg_obj_used = 0;

	li_can_slv_port_memory_set(can_mon_tx_msg_obj, 0x00, sizeof(can_mon_tx_msg_obj));
	can_mon_tx_msg_obj_used = 0;

	return can_mon_hw_deinit();
}

/**
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_mon_enable(void)
{
	return can_mon_hw_enable();
}

/**
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_mon_disable(void)
{
	return can_mon_hw_disable();
}

/**
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_mon_process_stop(void)
{
	return (ERR_MSG_CAN_NOT_IMPLEMENTED);
}

/**
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_mon_process_start(void)
{
	return (ERR_MSG_CAN_NOT_IMPLEMENTED);
}

/**
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_mon_msg_obj_rx_data_cnfg(uint16_t msg_obj, uint16_t table_pos)
{
	if (can_mon_rx_msg_obj_used < CAN_CONFIG_SYNC_MON_MAX_NR_OF_RX_OBJ)
	{
		can_mon_rx_msg_obj[can_mon_rx_msg_obj_used].msg_obj_mask = (1L << msg_obj);
		can_mon_rx_msg_obj[can_mon_rx_msg_obj_used].msg_obj = msg_obj;
		can_mon_rx_msg_obj[can_mon_rx_msg_obj_used].table_pos = table_pos;
		can_mon_rx_msg_obj_used++;
		return (LI_CAN_SLV_ERR_OK);
	}
	return (ERR_MSG_CAN_MON_MSG_OBJ_RX_DATA_CNFG);
}

/**
 * @return #LI_CAN_SLV_ERR_OK or #ERR_MSG_CAN_MON_MSG_OBJ_TX_DATA_CNFG
 */
li_can_slv_errorcode_t can_mon_msg_obj_tx_data_cnfg(uint16_t msg_obj, uint16_t table_pos)
{
	if (can_mon_tx_msg_obj_used < CAN_CONFIG_SYNC_MON_MAX_NR_OF_TX_OBJ)
	{
		can_mon_tx_msg_obj[can_mon_tx_msg_obj_used].msg_obj_mask = (1L << msg_obj);
		can_mon_tx_msg_obj[can_mon_tx_msg_obj_used].msg_obj = msg_obj;
		can_mon_tx_msg_obj[can_mon_tx_msg_obj_used].table_pos = table_pos;
		can_mon_tx_msg_obj_used++;
		return (LI_CAN_SLV_ERR_OK);
	}
	return (ERR_MSG_CAN_MON_MSG_OBJ_TX_DATA_CNFG);
}

/**
 * @param msg_obj used message object to define
 * @param can_id identifier used
 * @param acceptance_mask acceptance mask
 * @param dlc data length code of object
 * @param dir data direction used
 * @param service_id id of the CAN SERVICE used
 * @param is_sync_obj is msg_obj used as a synchronous object
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_mon_define_msg_obj(uint16_t msg_obj, uint16_t can_id, uint16_t acceptance_mask, byte_t dlc, byte_t dir, can_mon_service_id_t service_id, bit_t is_sync_obj)
{
	li_can_slv_errorcode_t err;

#ifdef CAN_MON_CHECK_DEFINE_OBJECT
	if (!(msg_obj < LI_CAN_SLV_MAIN_NODE_MAX_NOF_MSG_OBJ))
	{
		return (ERR_MSG_CAN_DEFINE_OBJ_INVALID_NR);
	}

	if ((can_id < CAN_IDENTIFIER_MIN) || (can_id > CAN_IDENTIFIER_MAX))
	{
		return (ERR_MSG_CAN_DEFINE_OBJ_INVALID_ID);
	}

	if ((dlc < CAN_DLC_MIN) || (dlc > CAN_DLC_MAX))
	{
		return (ERR_MSG_CAN_DEFINE_OBJ_INVALID_DLC);
	}

	if (!((dir == CAN_CONFIG_DIR_TX) || (dir == CAN_CONFIG_DIR_RX)))
	{
		return (ERR_MSG_CAN_DEFINE_OBJ_INVALID_DIR);
	}
#endif // #ifdef CAN_MON_CHECK_DEFINE_OBJECT

#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_MON
	LI_CAN_SLV_DEBUG_PRINT("mon def msg obj:%d, id:%d\n", msg_obj, can_id);
	LI_CAN_SLV_DEBUG_PRINT("mask: %d dlc: %d, dir: %d\n", acceptance_mask, dlc, dir);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_MON

	err = can_mon_hw_define_msg_obj(msg_obj, can_id, acceptance_mask, dlc, dir, service_id);

	if (LI_CAN_SLV_ERR_OK == err)
	{
		switch (service_id)
		{
			case CAN_MON_ISR_ID_RX_MAIN:
				if (is_sync_obj != 0)
				{
					can_mon_isr_inp_mask_rx_main |= (1L << msg_obj);
				}
				can_mon_objs_mask |= (1L << msg_obj);
				break;

			case CAN_MON_ISR_ID_RX:
				if (is_sync_obj != 0)
				{
					can_mon_isr_inp_mask_rx |= (1L << msg_obj);
				}
				can_mon_objs_mask |= (1L << msg_obj);
				break;

			default:
				err = ERR_MSG_CAN_MON_UNDEFINED_ISR_ID;
				break;
		}
	}
	return (err);
}

li_can_slv_errorcode_t can_mon_get_next_free_msg_obj(uint16_t *msg_obj)
{
	li_can_slv_errorcode_t err;

	err = can_mon_hw_get_next_free_msg_obj(msg_obj);

	return (err);
}

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/** @} */
