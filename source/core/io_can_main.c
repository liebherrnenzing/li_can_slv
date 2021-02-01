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
 * \brief This module handles the basic driver functionality like initialization, enable
 * and disable for the main CAN-controller of the used CPU.
 * \remarks The main CAN-controller is used for the general communication
 * of the module with the master. Therefore the Liebherr CAN bus protocol is defined.
 *
 * - <b>initialization</b>\n
 * Both directions (receive/transmit) are connected to the main CAN-transceiver. If the controller
 * is enabled, it will be influenced the CAN bus. So after initialization the main CAN-controller
 * If CAN Reconnect is enabled first the CAN baud rate has to be measured by the monitor CAN-controller.
 * Then the main CAN-controller is enabled with the right CAN baud rate. See therefore the utility reconnect.
 *
 * - <b>interrupt service routine</b><br>
 * Includes the interrupt service routine of the main CAN-controller and the identification of the reason of the interrupt.\n
 *  - valid can message detected
 *  - error frame
 *  - error warning
 *  - bus off<br>
 *
 * - <b>CAN-ISR and the system state</b><br>
 * The behavior of the main interrupt service routine depends on the system state.
 * 	- Booting: Waiting for first CAN event. No transmission is allowed.
 * 	- System Critical: Transmission of synchronous data is not allowed. Stop synchronous process by disable process request
 * 	arbitration.
 * 	- System Alert: Disable CAN controller do not transmit any data.
 * 	- System FATAL: Disable CAN controller do not transmit any data.<br>
 *
 * 	<h3>Also some conditions on the main interrupt service routine changes the system state.</h3>
 * 	- Process running<br>
 * 	No timeout conditions. Set system state to Process running.
 * 	- Timeout<br>
 * 	Timeout conditions. Set system state to Timeout.<br>
 * 	- Change CAN baud rate<br>
 * 	Set system state to Change CAN baud rate.<br>
 * 	- Reconnect Startup<br>
 * 	Set system state to Reconnect Startup.<br>
 * 	- Reconnect Online<br>
 * 	Set system state to Reconnect Online.<br>
 * 	- Bus off<br>
 * 	Set system state to Bus off.
 * 	- Silent mode<br>
 * 	Set system state to Silent mode.<br>
 *
 * 	<h3>Also the distribution of the CAN messages (definition in io_can_config) is implemented in the interrupt
 * 	service routine of the main CAN-controller.</h3>
 * 	\arg software interrupt (50ms)\n
 * 	In case of no CAN event at least every 50ms a software interrupt request is send to the interrupt service
 * 	routine of the main CAN-controller. This is used to check the synchrony process an set the timeout
 * 	condition of each logical module. The same software interrupt request (50ms) is used for the reconnect
 * 	routine.\n
 * 	\arg system messages\n
 * 	If a system message is received	it will be processed by functions of io_can_sys.\n
 * 	\arg process request\n
 * 	A process request is directly processed in the interrupt service routine. The tx-conversion-code of the
 * 	application is started and all tx data of the module is sent to the CAN master.\n
 * 	\arg synchrony RX data\n
 * 	If the RX data are valid (correct DLC and every RX-object is received exactly only one time), the CAN
 * 	data are processed by the RX-conversion-code and the data is provided to the application.\n
 *
 *
 * 	\arg error frame\n
 * 	An error frame is directly processed in the interrupt service routine. For CAN bus diagnose every LEC
 * 	(last error code) type is stored by increasing a defined counter (see also io_can_quality).
 *
 * 	\arg error warning\n
 * 	If error warning is active by call the interrupt service routine, the main CAN-controller will be immediately
 * 	disabled (no longer influence to CAN bus). After that the reconnect routines are started.
 *
 * 	\arg bus off\n
 * 	If bus off is detected a recovery sequence is started to activate the main CAN-controller once again. The
 * 	reaction depends on the system state (sysstate = SYSSTATE_BOOTING (booting and waiting for first CAN event)).
 * 	The first sending of a CAN message is only possible	if a "status request" or a "process request" is received.
 * 	The CAN message "change CAN baud rate" clears the admission. Only with a re-phasing request (status or process)
 * 	a CAN message could be sending.\n\n
 *
 * 	- <b>can status</b>\n
 * 	The CAN status register of the main CAN-controller is not only analyzed by receiving a can message (RxOK flag).
 * 	Also when sending a CAN message the TxOK flag is analyzed.\n
 *
 * 	- <b>queues</b>\n
 *
 * 	Includes following queues with its management:
 * 	 - CAN system tx queue

 * 	- <b>CAN bus quality</b> (see io_can_quality)\n
 * 	A CAN bus measurement is implemented for the main CAN-controller which represented a statistical value of the
 * 	CAN-quality (0 to 1000 per mill).\n
 *
 * 	- <b>CAN analyzer</b> (see io_can_analyzer)\n
 * 	A CAN analyzer is implemented for the main CAN-controller. It's possible to record all received and
 * 	transmitted data with a time stamp of 0.1ms. Therefore a hardware timer is used.
 */

/*!
 * \file io_can_main.c
 * \brief CAN Main Driver
 * @addtogroup can_driver_main
 * @{
 */
/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/core/io_can_main.h>
#include "io_can_main_hw.h"

#include "io_can_port.h"

#include <li_can_slv/config/io_can_config.h>

#include <li_can_slv/core/io_can.h>
#include "io_can_hw.h"

#include <li_can_slv/error/io_can_error.h>
#include <li_can_slv/error/io_can_errno.h>

#if defined(OUTER) || defined(OUTER_APP)
#include <li_can_slv/sync/io_can_sync.h>
#endif /* #if defined(OUTER) || defined(OUTER_APP) */

#ifdef LI_CAN_SLV_RECONNECT
#include <li_can_slv/core/io_can_reconnect.h>
#endif /* #ifdef LI_CAN_SLV_RECONNECT */

#ifdef IO_SMP
#include "io_smp.h"
#endif /* #ifdef IO_SMP */

#ifdef SHOW_CAN_MAIN
#include "io_var.h"
#ifdef TESTSYS_SYSTEM
#include "io_testsys.h"
#endif /* #ifdef TESTSYS_SYSTEM */
#endif /* #ifdef SHOW_CAN_MAIN */

#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_debug.h"
#endif /* #ifdef LI_CAN_SLV_DEBUG */

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/
/* CAN main general definitions */
#define CAN_MAIN_PROCESS_DATA_TX_SEND_CLEAR		0x00000000 /**< clear send flags of synchronous process data transmission */
#define CAN_MAIN_PROCESS_DATA_TX_ACTIVE_CLEAR	0x00000000 /**< clear active flags of synchronous process data transmission */
#define CAN_MAIN_PROCESS_DATA_TX_SET_CLEAR		0x00000001 /**< clear set flags of synchronous process data transmission */

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/
/* interrupt pending masks */
volatile dword_t can_main_isr_inp_mask_rx = 0; /*!< interrupt pending mask receive for can_main_isr */
volatile dword_t can_main_isr_inp_mask_tx = 0; /*!< interrupt pending mask transmit for can_main_isr */

volatile dword_t can_main_objs_mask = 0; /*!< mask that identifies all slots the CAN controller is configured for */
volatile dword_t can_main_async_objs_mask = 0; /*!< mask that identifies all asynchronous data slots the CAN controller is configured for */
#if defined(OUTER) || defined(OUTER_APP)
volatile dword_t can_main_async_ctrl_objs_mask = 0; /*!< mask that identifies all asynchronous ctrl data slots the CAN controller is configured for */
#endif /* #if defined(OUTER) || defined(OUTER_APP) */

#if defined(OUTER) || defined(OUTER_APP)
volatile uint16_t li_can_slv_sync_main_rx_msg_obj_used = 0; /**< */
#endif /* #if defined(OUTER) || defined(OUTER_APP) */

#if defined(OUTER) || defined(OUTER_APP)
volatile can_main_rx_msg_obj_t li_can_slv_sync_main_rx_msg_obj[CAN_CONFIG_SYNC_MAIN_MAX_NR_OF_RX_OBJ] = {{0}}; /**< */
uint16_t li_can_slv_sync_main_tx_msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_TX_SYNC; /**< */
#endif /* #if defined(OUTER) || defined(OUTER_APP) */

#ifdef CAN_MAIN_DIAGNOSE
can_main_diagnose_t can_main_diagnose = {0};
#endif /* #ifdef CAN_MAIN_DIAGNOSE */

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/
#if defined(OUTER) || defined(OUTER_APP)
/* process data transmit control */
volatile can_main_sync_process_tx_data_ctrl_t can_main_sync_process_tx_data_ctrl =
{
	CAN_MAIN_PROCESS_SYNC_SEND_DATA_ACK, /* send_cmd */
	CAN_MAIN_PROCESS_DATA_TX_SEND_CLEAR, /* send_reg */
	CAN_MAIN_PROCESS_DATA_TX_SEND_CLEAR, /* send */
	0, /* send_end */
	0, /* send_current */
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES == 1
	{ {0, 0, 0, 0} }, /* id */
	{ {0, 0, 0, 0} } /* dlc */
#endif /* #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES == 1 */
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES == 2
	{ {0, 0, 0, 0}, {0, 0, 0, 0} }, /* id */
	{ {0, 0, 0, 0}, {0, 0, 0, 0} } /* dlc */
#endif /* #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES == 2 */
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES == 3
	{ {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} }, /* id */
	{ {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } /* dlc */
#endif /* #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES == 3 */
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES == 4
	{ {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} }, /* id */
	{ {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } /* dlc */
#endif /* #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES == 4 */
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES == 5
	{ {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} }, /* id */
	{ {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } /* dlc */
#endif /* #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES == 5 */
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES == 6
	{ {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} }, /* id */
	{ {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } /* dlc */
#endif /* #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES == 6 */
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES == 7
	{ {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} }, /* id */
	{ {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } /* dlc */
#endif /* #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES == 7 */
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES == 8
	{ {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} }, /* id */
	{ {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } /* dlc */
#endif /* #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES == 8 */
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 9
#warning LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES out of range!
#endif /* #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 9 */
};
#endif /* #if defined(OUTER) || defined(OUTER_APP) */

#ifdef SHOW_CAN_MAIN
/*--------------------------------------------------------------------------*/
/* system variable CAN main                                                 */
/*--------------------------------------------------------------------------*/
static var_change_t can_main_var_change[] =
{
	{ VAR_STATE_FRC_SET, "", ""},
	{ VAR_STATE_FRC_SET, "", ""},
	{ VAR_STATE_FRC_SET, "", ""},
	{ VAR_STATE_FRC_SET, "", ""},

	{ VAR_STATE_FRC_SET, "", ""},
	{ VAR_STATE_FRC_SET, "", ""},
	{ VAR_STATE_FRC_SET, "", ""},
	{ VAR_STATE_FRC_SET, "", ""},

	{ VAR_STATE_FRC_SET, "", ""},
	{ VAR_STATE_FRC_SET, "", ""},
	{ VAR_STATE_FRC_SET, "", ""},
	{ VAR_STATE_FRC_SET, "", ""},

	{ VAR_STATE_FRC_SET, "", ""},
	{ VAR_STATE_FRC_SET, "", ""},
	{ VAR_STATE_FRC_SET, "", ""},
	{ VAR_STATE_FRC_SET, "", ""},

	{ VAR_STATE_FRC_SET, "", ""},
	{ VAR_STATE_FRC_SET, "", ""},
	{ VAR_STATE_FRC_SET, "", ""},
#ifdef LI_CAN_SLV_DEBUG_MAIN_SYS_HANDLER_EXTENDED
	{ VAR_STATE_FRC_SET, "", ""},
#endif /* #ifdef LI_CAN_SLV_DEBUG_MAIN_SYS_HANDLER_EXTENDED */
#ifdef LI_CAN_SLV_DEBUG_CAN_ASYNC
	{ VAR_STATE_FRC_SET, "", ""},
#endif /* #ifdef LI_CAN_SLV_DEBUG_CAN_ASYNC */
};

#ifdef CAN_MAIN_DIAGNOSE
static const var_const_t can_main_var_const[] =
{
	{"main.rxok",		&can_main_diagnose.rxok,		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[0]},
	{"main.txok",		&can_main_diagnose.txok,		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[1]},
	{"main.err",		&can_main_diagnose.err,			VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[2]},
#ifdef IO_SMP
	{"main.ewrn",		&can_main_diagnose.ewrn,		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	HNDLPOS_CAN_MAIN_EWRN,	&can_main_var_change[3]},
	{"main.boff",		&can_main_diagnose.boff,		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	HNDLPOS_CAN_MAIN_BOFF,	&can_main_var_change[4]},
#else /* #ifdef IO_SMP */
	{"main.ewrn",		&can_main_diagnose.ewrn,		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[3]},
	{"main.boff",		&can_main_diagnose.boff,		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[4]},
#endif /* #ifdef IO_SMP */
	{"main.stuff",		&can_main_diagnose.lec[0],		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[5]},
	{"main.form",		&can_main_diagnose.lec[1],		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[6]},
	{"main.ackn",		&can_main_diagnose.lec[2],		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[7]},

	{"main.bit1",		&can_main_diagnose.lec[3],		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[8]},
	{"main.bit0",		&can_main_diagnose.lec[4],		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[9]},
	{"main.crc",		&can_main_diagnose.lec[5],		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[10]},
	{"main.pr",			&can_main_diagnose.pr,			VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[11]},

	{"main.syncrx",		&can_main_diagnose.syncrx,		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[12]},
	{"main.synctx",		&can_main_diagnose.synctx,		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[13]},
	{"main.sysrx",		&can_main_diagnose.sysrx,		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[14]},
	{"main.systx",		&can_main_diagnose.systx,		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[15]},

	{"main.actrlrx",	&can_main_diagnose.actrlrx,		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[16]},
	{"main.adatarx",	&can_main_diagnose.adatarx,		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[17]},
	{"main.asynctx",	&can_main_diagnose.asynctx,		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[18]},
#ifdef LI_CAN_SLV_DEBUG_MAIN_SYS_HANDLER_EXTENDED
	{"main.lost_sysrx",	&can_main_diagnose.sys_lost,	VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,					&can_main_var_change[20]},
#endif /* #ifdef LI_CAN_SLV_DEBUG_MAIN_SYS_HANDLER_EXTENDED */
#ifdef LI_CAN_SLV_DEBUG_CAN_ASYNC_SYS
	{"main.lost_actrlrx", &can_main_diagnose.actrl_lost,	VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,				&can_main_var_change[21]},
#endif /* #ifdef LI_CAN_SLV_DEBUG_CAN_ASYNC_SYS */
	{VAR_END_OF_TABLE}
};
#endif /* #ifdef CAN_MAIN_DIAGNOSE */

#ifdef CAN_MAIN_DIAGNOSE
/*--------------------------------------------------------------------------*/
/* IO-TESTSYS-PAGE system CAN main                                          */
/*--------------------------------------------------------------------------*/
static const testsys_var_t can_main_testsys[] =
{
	{&can_main_var_const[0],	1,	1},
	{&can_main_var_const[1],	1,	2},
	{&can_main_var_const[2],	1,	3},
	{&can_main_var_const[3],	1,	4},

	{&can_main_var_const[4],	1,	5},
	{&can_main_var_const[5],	1,	6},
	{&can_main_var_const[6],	1,	7},
	{&can_main_var_const[7],	1,	8},

	{&can_main_var_const[8],	1,	9},
	{&can_main_var_const[9],	1,	10},
	{&can_main_var_const[10],	1,	11},
	{&can_main_var_const[11],	1,	12},

	{&can_main_var_const[12],	1,	13},
	{&can_main_var_const[13],	1,	14},
	{&can_main_var_const[14],	1,	15},
	{&can_main_var_const[15],	1,	16},

	{&can_main_var_const[16],	1,	17},
	{&can_main_var_const[17],	1,	18},
	{&can_main_var_const[18],	1,	19},
#ifdef LI_CAN_SLV_DEBUG_MAIN_SYS_HANDLER_EXTENDED
	{&can_main_var_const[20],	40,	1},
#endif /* #ifdef LI_CAN_SLV_DEBUG_MAIN_SYS_HANDLER_EXTENDED */
#ifdef LI_CAN_SLV_DEBUG_CAN_ASYNC
	{&can_main_var_const[21],	40,	2},
#endif /* #ifdef LI_CAN_SLV_DEBUG_CAN_ASYNC */
	{TESTSYS_VAR_END_OF_TABLE,	0,	0}
};
#endif /* #ifdef SHOW_CAN_MAIN */
#endif /* #ifdef CAN_MAIN_DIAGNOSE */

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
/**
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_main_init(void)
{
	li_can_slv_errorcode_t err;

	err = can_main_hw_init();
#ifdef CAN_MAIN_DIAGNOSE
#ifdef SHOW_CAN_MAIN
	/*----------------------------------------------------------------------*/
	/* add can main IO's                                                    */
	/*----------------------------------------------------------------------*/
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = var_add(&can_main_var_const[0]);
	}

	/*----------------------------------------------------------------------*/
	/* add can main IO's to testsys                                         */
	/*----------------------------------------------------------------------*/
#ifdef TESTSYS_SYSTEM
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = testsys_system_page_add(&can_main_testsys[0], "CAN Node a controller system");
	}
#endif /* #ifdef TESTSYS_SYSTEM */
#endif /* #ifdef SHOW_CAN_MAIN */
#endif /* #ifdef CAN_MAIN_DIAGNOSE */
	return err;
}

#if defined(OUTER) || defined(OUTER_APP)
/*!
 * \brief start can main process
 * \return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_main_process_start(void)
{
	return (ERR_MSG_CAN_NOT_IMPLEMENTED);
}
#endif /* #if defined(OUTER) || defined(OUTER_APP) */

#if defined(OUTER) || defined(OUTER_APP)
/*!
 * \brief stop can main process
 * \return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_main_process_stop(void)
{
	return (ERR_MSG_CAN_NOT_IMPLEMENTED);
}
#endif /* #if defined(OUTER) || defined(OUTER_APP) */

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @param table_pos number module used
 * @param obj number of module object
 * @param module_nr used module number
 * @return #LI_CAN_SLV_ERR_OK, #ERR_MSG_CAN_MAIN_OVER_TABLE_POS or #ERR_MSG_CAN_MAIN_OVER_OBJECT_NR
 */
li_can_slv_errorcode_t can_main_sync_process_tx_data_cnfg(uint16_t table_pos, uint16_t obj, li_can_slv_module_nr_t module_nr)
{
	uint16_t can_id;
	dword_t send;

	if (table_pos > (LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES - 1u))
	{
		return (ERR_MSG_CAN_MAIN_OVER_TABLE_POS);
	}

	if (obj > CAN_CONFIG_NR_OF_MODULE_OBJS - 1)
	{
		return (ERR_MSG_CAN_MAIN_OVER_OBJECT_NR);
	}

	/* calculation of the transmit identifier */
	can_id = CAN_CONFIG_DATA_TX_MASK + ((module_nr - 1) << 2) + obj;

	can_main_sync_process_tx_data_ctrl.id[table_pos][obj] = can_id;
	can_main_sync_process_tx_data_ctrl.dlc[table_pos][obj] = can_config_module_tab[table_pos].tx_dlc[obj];

	/* set corresponding flag in the send register */
	send = 0x00000001L << obj;
	can_main_sync_process_tx_data_ctrl.send_reg |= (send << (CAN_CONFIG_NR_OF_MODULE_OBJS * table_pos));
	can_main_sync_process_tx_data_ctrl.send_end = table_pos * CAN_CONFIG_NR_OF_MODULE_OBJS + obj;

	return (LI_CAN_SLV_ERR_OK);
}
#endif /* #if defined(OUTER) || defined(OUTER_APP) */

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @param table_pos number module used
 * @param obj number of module object
 */
void can_main_synchron_tx_data_off(uint16_t table_pos, uint16_t obj)
{
	dword_t send;

	/* set corresponding flag in the send register */
	send = 0x00000001L << obj;
	can_main_sync_process_tx_data_ctrl.send_reg &= ~(send << (CAN_CONFIG_NR_OF_MODULE_OBJS * table_pos));
}
#endif /* #if defined(OUTER) || defined(OUTER_APP) */

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @param table_pos number module used
 * @param obj number of module object
 */
void can_main_synchron_tx_data_on(uint16_t table_pos, uint16_t obj)
{
	dword_t send;

	/* set corresponding flag in the send register */
	send = 0x00000001L << obj;
	can_main_sync_process_tx_data_ctrl.send_reg |= (send << (CAN_CONFIG_NR_OF_MODULE_OBJS * table_pos));
}
#endif /* #if defined(OUTER) || defined(OUTER_APP) */

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @param msg_obj used message object
 * @param table_pos number module used
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_main_msg_obj_rx_data_cnfg(uint16_t msg_obj, uint16_t table_pos)
{

	if (li_can_slv_sync_main_rx_msg_obj_used < CAN_CONFIG_SYNC_MAIN_MAX_NR_OF_RX_OBJ)
	{
		li_can_slv_sync_main_rx_msg_obj[li_can_slv_sync_main_rx_msg_obj_used].msg_obj = msg_obj;
		li_can_slv_sync_main_rx_msg_obj[li_can_slv_sync_main_rx_msg_obj_used].msg_obj_mask = (1L << msg_obj);
		li_can_slv_sync_main_rx_msg_obj[li_can_slv_sync_main_rx_msg_obj_used].table_pos = table_pos;

		li_can_slv_sync_main_rx_msg_obj_used++;
		return (LI_CAN_SLV_ERR_OK);
	}
	return (ERR_MSG_CAN_MAIN_MSG_OBJ_RX_DATA_CNFG);
}
#endif /* #if defined(OUTER) || defined(OUTER_APP) */

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @param msg_obj used message object
 */
void can_main_msg_obj_tx_data_cnfg(uint16_t msg_obj)
{
	li_can_slv_sync_main_tx_msg_obj = msg_obj;
}
#endif /* #if defined(OUTER) || defined(OUTER_APP) */

/**
 * @param module_nr module number
 * @param[in] src source pointer of data
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_main_send_queue_system_tx(li_can_slv_module_nr_t module_nr, byte_t const *src)
{
	uint16_t can_id;

	/* calculation of CAN identifier */
	can_id = CAN_CONFIG_SYS_TX_MASK + ((module_nr - 1) << 2);
	can_main_hw_send_msg(can_id, CAN_CONFIG_SYS_MSG_DLC, src);
	return (LI_CAN_SLV_ERR_OK);
}

#if defined(OUTER) || defined(OUTER_APP)
/*!
 * \brief set the send flag of all synchrony process data of all defined logical modules
 * \return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_main_process_trigger_sync_tx(void)
{
#ifdef CAN_TEST_ONLY_SEND_MASTER
	return (LI_CAN_SLV_ERR_OK);
#endif /* #ifdef CAN_TEST_ONLY_SEND_MASTER */

	/* indicate that a new sync TX request was issued */
	can_main_sync_process_tx_data_ctrl.send_cmd = CAN_MAIN_PROCESS_SYNC_SEND_DATA;

	/* trigger queue handling */
	can_port_trigger_can_main_sync_process_data_tx_queue();

	return (LI_CAN_SLV_ERR_OK);
}
#endif /* #if defined(OUTER) || defined(OUTER_APP) */

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t can_main_enable(void)
{
	li_can_slv_node_mode_t mode;
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	mode = li_can_slv_get_node_mode();

	if (LI_CAN_SLV_MODE_OPERATIONAL == mode)
	{
		err = can_main_hw_enable();
	}
#ifdef LI_CAN_SLV_RECONNECT
	else if (LI_CAN_SLV_MODE_LISTEN_ONLY == mode)
	{
		err = can_main_hw_enable_listen_only();
	}
#endif // #ifdef LI_CAN_SLV_RECONNECT
	else
	{
		err = can_main_hw_disable();
	}

	return err;
}

li_can_slv_errorcode_t can_main_disable(void)
{
	li_can_slv_node_mode_t mode;

	mode = li_can_slv_get_node_mode();

	if (LI_CAN_SLV_MODE_STOPPED != mode)
	{
		li_can_slv_set_node_mode(LI_CAN_SLV_MODE_STOPPED);
	}

	return can_main_hw_disable();
}

#if defined(OUTER) || defined(OUTER_APP)
/*!
 * @brief can_main_sync_process_tx_data
 */
void can_main_sync_send_process_data(void)
{
	uint8_t data[8];
	uint16_t table_pos, obj;
	uint16_t dlc;
	li_can_slv_errorcode_t err;

#ifdef LI_CAN_SLV_DEBUG_MAIN_SYNC_PROCESS_TX_DATA
	LI_CAN_SLV_DEBUG_PRINT("can_main_sync tx data\n");
	LI_CAN_SLV_DEBUG_PRINT("ctrl.send: %08lx\n", can_main_sync_process_tx_data_ctrl.send);
#endif /* #ifdef LI_CAN_SLV_DEBUG_MAIN_SYNC_PROCESS_TX_DATA */

	/* send all tx objects */
	while (can_main_sync_process_tx_data_ctrl.send != CAN_MAIN_PROCESS_DATA_TX_SEND_CLEAR)
	{
#ifdef LI_CAN_SLV_DEBUG_MAIN_SYNC_PROCESS_TX_DATA
		LI_CAN_SLV_DEBUG_PRINT("tx_slot=%d, mo=%d\n", i, li_can_slv_sync_main_tx_msg_obj);
#endif /* #ifdef LI_CAN_SLV_DEBUG_MAIN_SYNC_PROCESS_TX_DATA */

		/*--------------------------------------------------------------------------*/
		/*  number of objects                                                       */
		/*                                                                          */
		/*  nr    send                                                              */
		/*  0     0000	                                                            */
		/*  1     0001	                                                            */
		/*  2     0011					                                            */
		/*  3     0111			                                                    */
		/*  4     1111		                                                        */
		/*                                                                          */
		/*  bit = 0x000F >> (4-nr)                                                  */
		/*--------------------------------------------------------------------------*/

		/* switch to next active send flag */
		while ((can_main_sync_process_tx_data_ctrl.send_current <= can_main_sync_process_tx_data_ctrl.send_end) && ((can_main_sync_process_tx_data_ctrl.send & 0x00000001L) == 0x00000000L))
		{
			can_main_sync_process_tx_data_ctrl.send_current++;
			can_main_sync_process_tx_data_ctrl.send >>= 1;
		}

		/* check if send flag of current object is valid */
		if ((can_main_sync_process_tx_data_ctrl.send & 0x00000001L) == 0x00000001L)
		{
			table_pos = can_main_sync_process_tx_data_ctrl.send_current / CAN_CONFIG_NR_OF_MODULE_OBJS;
			obj = can_main_sync_process_tx_data_ctrl.send_current % CAN_CONFIG_NR_OF_MODULE_OBJS;

#ifdef LI_CAN_SLV_DEBUG_MAIN_SYNC_PROCESS_TX_DATA
			LI_CAN_SLV_DEBUG_PRINT("table_pos=%d, obj=%d\n", table_pos, obj);
			LI_CAN_SLV_DEBUG_PRINT("ctrl.send %08lx\n", can_main_sync_process_tx_data_ctrl.send);
#endif /* #ifdef LI_CAN_SLV_DEBUG_MAIN_SYNC_PROCESS_TX_DATA */

			/* call synchrony transmit routine of main CAN controller to prepare data to send via can, also capture data for monitor controller */
			dlc = (uint16_t) can_config_module_tab[table_pos].tx_dlc[obj];
			err = can_sync_tx_data_main(table_pos, obj, dlc, data);

			if (err == LI_CAN_SLV_ERR_OK)
			{
				err = can_main_hw_send_msg(can_main_sync_process_tx_data_ctrl.id[table_pos][obj], can_main_sync_process_tx_data_ctrl.dlc[table_pos][obj], data);
			}
			else
			{
				/**
				 * @toto add error handling
				 */
			}

#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
			if (err != LI_CAN_SLV_ERR_OK)
			{
				error_syserr_send(err, ERR_LVL_INFO, can_config_get_module_nr_main(), ERR_LVL_INFO);
			}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR

			can_main_sync_process_tx_data_ctrl.send_current++;
			can_main_sync_process_tx_data_ctrl.send >>= 1;
		}
	}
}
#endif /* #if defined(OUTER) || defined(OUTER_APP) */

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
li_can_slv_errorcode_t can_main_define_msg_obj(uint16_t msg_obj, uint16_t can_id, uint16_t acceptance_mask, byte_t dlc, byte_t dir, can_main_service_id_t service_id, bit_t is_sync_obj)
{
	li_can_slv_errorcode_t err;

#ifdef CAN_MAIN_CHECK_DEFINE_OBJECT
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
#endif /* #ifdef CAN_MAIN_CHECK_DEFINE_OBJECT */

#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_MAIN
	LI_CAN_SLV_DEBUG_PRINT("def msg obj:%d, id:%d\n", msg_obj, can_id);
	LI_CAN_SLV_DEBUG_PRINT(" m: %d dlc: %d, dir: %d\n", acceptance_mask, dlc, dir);
#endif /* #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_MAIN */

	err = can_main_hw_define_msg_obj(msg_obj, can_id, acceptance_mask, dlc, dir, service_id);

	if (LI_CAN_SLV_ERR_OK == err)
	{
		switch (service_id)
		{
			case CAN_MAIN_SERVICE_ID_TX:
				if (is_sync_obj != 0u)
				{
					can_main_isr_inp_mask_tx |= (1UL << msg_obj);
				}
				can_main_objs_mask |= (1L << msg_obj);
				break;

			case CAN_MAIN_SERVICE_ID_RX:
				if (is_sync_obj != 0u)
				{
					can_main_isr_inp_mask_rx |= (1UL << msg_obj);
				}
				can_main_objs_mask |= (1L << msg_obj);
				break;

			case CAN_MAIN_ASYNC_SERVICE_ID_TX:
			case CAN_MAIN_ASYNC_SERVICE_ID_RX:
#ifdef LI_CAN_SLV_BOOT
			case CAN_MAIN_ASYNC_CTRL_SERVICE_ID_RX:
#endif /* #ifdef LI_CAN_SLV_BOOT */
				can_main_async_objs_mask |= (1L << msg_obj);
				break;
#if defined(OUTER) || defined(OUTER_APP)
			case CAN_MAIN_ASYNC_CTRL_SERVICE_ID_RX:
				can_main_async_ctrl_objs_mask |= (1L << msg_obj);
				break;
#endif /* #if defined(OUTER) || defined(OUTER_APP) */
			default:
				err = ERR_MSG_CAN_MAIN_UNDEFINED_ISR_ID;
				break;
		}
	}
	return (err);
}

li_can_slv_errorcode_t can_main_get_next_free_msg_obj(uint16_t *msg_obj)
{
	li_can_slv_errorcode_t err;

	err = can_main_hw_get_next_free_msg_obj(msg_obj);

	return (err);
}

/** @} */
