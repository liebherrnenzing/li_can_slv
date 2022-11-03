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

/*!	\remarks CAN bus reconnect routines are used for the main and the monitor CAN-controller.\n
 Namely the monitor CAN-controller never can be influence the CAN bus in bad condition\n
 (the tx pin is not connected to CAN-transceiver) but both CAN-controllers must be\n
 protected in reason of a bus off situation. There are two different types of reconnects.\n
 The reconnect startup and the reconnect online.\n

 - <b>reconnect startup</b>\n
 The reconnect startup routine exists of following steps:\n
 \arg	Initialization of the main CAN-controller at default baud rate (disabled, so no influence to CAN bus).\n
 \arg	Initialization of the monitor CAN-controller at default baud rate (disabled).\n
 \arg	Enabling the monitor CAN-controller and waiting for a CAN event.\n
 \arg	If no CAN event happens in a defined time (110% of the process security time) the main CAN-controller will be\n
 enabled. This happens in a normal startup sequence (no CAN traffic when module is switched on).\n
 \arg	In case of CAN events and detection of error frames switching to next CAN baud rate as long\n
 as error frames detected. The different "try baud rates" are defined by the baud rate table.\n
 Look therefore io_can_config.\n
 \arg	If a correct CAN message is received on the monitor CAN-controller it will be an indices that the\n
 right CAN baud rate is set.\n\n

 - <b>reconnect online</b>\n
 For the availability the time for the online reconnect should be under 100ms (less than the	process security time).\n
 So it is possible for the module to make an online reconnect without timeout situation to the CAN master unit.\n
 The reconnect online routine exists of following steps:\n
 \arg	detection of CAN error warning condition (main or monitor)
 \arg	deactivation of the main CAN-controller (no longer influence to the CAN bus)
 \arg	measurement of the current CAN baud rate with the monitor CAN-controller
 \arg	initialize the main CAN-controller with the new CAN baud rate
 \arg	activate the main CAN-controller\n\n

 - <b>reconnect recovery sequence</b>\n
 In case of CAN bus off situation of each CAN-controller (main and monitor) the can recovery\n
 routine (sequence) will be started. The recovery sequence exists of following steps:\n
 \arg	detection of CAN bus off condition (main or monitor)
 \arg	deactivation of the main and monitor CAN-controller (no longer influence to the CAN bus)
 \arg	new initialization of main and monitor CAN-controller
 \arg	new configuration of the main and monitor CAN-controller (see therefore can_io_config)
 \arg	measurement of the current CAN baud rate with the monitor CAN-controller
 \arg	initialize the main CAN-controller with the new CAN baud rate
 \arg	activate the main CAN-controller\n\n

 - <b>measure of CAN baud rate </b>\n
 The basic for a startup or online reconnect is the possibility to measure the CAN baud rate with no\n
 influence to the CAN-bus (no error frames generated). See therefore in the document <A HREF="..\ExternalDoc\CAN Reconnect.doc">CAN Reconnect.doc</A>.\n

 \test
 To activate/deactivate the main CAN-controller should not be influence the CAN bus (Test with\n
 a autonomous measurement, e. g.: CANScope).\n
 The reconnect routine have to tested for different CAN bus topologies:
 \arg	different CAN baud rates
 \arg 	different values of bus traffic (large/low number of modules; great/small process time)\n
 \arg 	testing of each variation:\n
 - master on 100, 125, 250, 500 and 1000kBaud\n
 - module on 100, 125, 250, 500 and 1000kBaud\n
 \arg 	testing of single communication (one master one reconnect module)\n
 \sa external representation corresponds to the document in <A HREF="..\ExternalDoc\CAN Reconnect.doc">CAN Reconnect.doc</A>\n
 */

/**
 * @file io_can_reconnect.c
 * @addtogroup can_reconnect
 * @{
 */
/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/core/io_can_reconnect.h>

#ifdef IO_SMP
#include "io_smp.h"
#endif // #ifdef IO_SMP

#include <li_can_slv/core/io_can_main.h>
#include <li_can_slv/core/io_can_mon.h>

#include <li_can_slv/error/io_can_error.h>
#include <li_can_slv/error/io_can_errno.h>

#ifdef SHOW_CAN_RECONNECT
#include "io_var.h"
#ifdef TESTSYS_SYSTEM
#include "io_testsys.h"
#include "io_smp_id.h"
#endif // #ifdef TESTSYS_SYSTEM
#endif // #ifdef SHOW_CAN_RECONNECT

#include "io_can_hw.h"
#include "io_can_main_hw.h"

#include "io_can_port.h"
#include <li_can_slv/core/io_can.h>
#include <li_can_slv/core/li_can_slv_core_defines.h>
#include <li_can_slv/error/li_can_slv_error_types.h>

#include <li_can_slv/core/li_can_slv_core_api.h>

#include <li_can_slv/config/io_can_config.h>

#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_debug.h"
#endif // #ifdef LI_CAN_SLV_DEBUG

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#define CAN_RECONNECT_BDR_DETECT_NR_LEC_BIT0	10 /**< number of Bit0 errors in series for baud rate detection */
#define CAN_RECONNECT_BDR_DETECT_NR_LEC			3 /**< number of errors before switch next baud rate */
#define CAN_RECONNECT_MAX_BDR_INDEX				4 /**< */

#ifndef LI_CAN_SLV_MON
#define CAN_MON_CTRL_INIT						0x0001 /**< reseting bit INIT */
#endif // #ifndef LI_CAN_SLV_MON

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/
volatile int16_t li_can_slv_reconnect_main_node_msg_pending = CAN_RECONNECT_INTID_NO_INTERRUPT_PENDING;
volatile int16_t li_can_slv_reconnect_mon_node_msg_pending = CAN_RECONNECT_INTID_NO_INTERRUPT_PENDING;

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
static li_can_slv_reconnect_online_change_baudrate_start_cbk_funcp_t reconnect_online_change_baudrate_start_cbk_funcp = NULL;
static li_can_slv_reconnect_online_change_baudrate_stop_cbk_funcp_t reconnect_online_change_baudrate_stop_cbk_funcp = NULL;
#endif // #ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK

#ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK
static li_can_slv_reconnect_startup_change_baudrate_start_cbk_funcp_t reconnect_startup_change_baudrate_start_cbk_funcp = NULL;
static li_can_slv_reconnect_startup_change_baudrate_stop_cbk_funcp_t reconnect_startup_change_baudrate_stop_cbk_funcp = NULL;
#endif // #ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK

static volatile li_can_slv_reconnect_t can_reconnect =  /**< */
{
	CAN_RECONNECT_STATE_OFF,
	CAN_RECONNECT_CAUSE_STARTUP_NO_TRAFFIC,
	CAN_RECONNECT_BACK_UNKNOWN,
	0,
	0,
	0,
	0,
	0,
	0L,
	0,
	0,
	0L
};

static volatile uint8_t process_on_off = FALSE;

static const can_config_bdr_t can_reconnect_bdr_table[CAN_RECONNECT_MAX_BDR_INDEX] = /**< */
{
	125,
	1000,
	500,
	250
};

#ifdef SHOW_CAN_RECONNECT
static const char_t *can_reconnect_state_info[] = /**< */
{
	"DEACTIVATED",
	"OFF        ",
	"STARTUP    ",
	"ONLINE     ",
	"IDLE       "
};
#endif // #ifdef SHOW_CAN_RECONNECT

#ifdef SHOW_CAN_RECONNECT
static const char_t *can_reconnect_cause_info[] = /**< */
{
	"STARTUP_NO_TRAFFIC",
	"STARTUP           ",
	"MAIN_EWRN         ",
	"MAIN_BOFF         ",
	"MON_EWRN          ",
	"MON_BOFF          ",
	"DOWNLOAD ACK      "
};
#endif // #ifdef SHOW_CAN_RECONNECT

#ifdef SHOW_CAN_RECONNECT
static const char_t *can_reconnect_back_info[] = /**< */
{
	"UNKNOWN            ",
	"STARTUP_TIME       ",
	"STARTUP_VALID_EVENT",
	"VALID_EVENT        ",
	"NO_LEC             ",
	"BACK_BIT0          ",
	"NO_IRQ_PENDING     ",
	"DOWNLOAD ACK       "
};
#endif // #ifdef SHOW_CAN_RECONNECT

#ifdef SHOW_CAN_RECONNECT
/*--------------------------------------------------------------------------*/
/* system variable CAN reconnect                                            */
/*--------------------------------------------------------------------------*/
static var_change_t can_reconnect_var_change[] = /**< */
{
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""}
};


#ifdef IO_SMP
static const var_const_t can_reconnect_var_const[] = /**< */
{
	{"canrc.state",	&can_reconnect.state,	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	HNDLPOS_CANRC_STATE,	&can_reconnect_var_change[0]},
	{"canrc.cause",	&can_reconnect.cause,	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	HNDLPOS_CANRC_CAUSE,	&can_reconnect_var_change[1]},
	{"canrc.back",	&can_reconnect.back,	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	HNDLPOS_CANRC_BACK,		&can_reconnect_var_change[2]},
	{"canrc.nr",	&can_reconnect.nr,		VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	HNDLPOS_CANRC_NR,		&can_reconnect_var_change[3]},
	{VAR_END_OF_TABLE}
};
#else // #ifdef IO_SMP
static const var_const_t can_reconnect_var_const[] = /**< */
{
	{"canrc.state",		&can_reconnect.state,		VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_reconnect_var_change[0]},
	{"canrc.cause",		&can_reconnect.cause,		VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_reconnect_var_change[1]},
	{"canrc.back",		&can_reconnect.back,			VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_reconnect_var_change[2]},
	{"canrc.nr",		&can_reconnect.nr,			VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_reconnect_var_change[3]},
	{"canrc.bdr_index",	&can_reconnect.bdr_index,	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_reconnect_var_change[4]},
	{"canrc.id_on",		&can_reconnect.id_on,		VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_reconnect_var_change[5]},
	{"canrc.id_off",	&can_reconnect.id_off,		VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_reconnect_var_change[6]},
	{"canrc.back_bdr",	&can_reconnect.back_bdr,		VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_reconnect_var_change[7]},
	{VAR_END_OF_TABLE}
};
#endif // #ifdef IO_SMP

/*--------------------------------------------------------------------------*/
/* IO-TESTSYS-PAGE system CAN reconnect                                     */
/*--------------------------------------------------------------------------*/
static const testsys_var_t can_reconnect_testsys[] = /**< */
{
	{&can_reconnect_var_const[0],	1,	1},
	{&can_reconnect_var_const[1],	1,	2},
	{&can_reconnect_var_const[2],	1,	3},
	{&can_reconnect_var_const[3],	1,	4},
	{&can_reconnect_var_const[4],	1,	5},
	{&can_reconnect_var_const[5],	1,	6},
	{&can_reconnect_var_const[6],	1,	7},
	{&can_reconnect_var_const[7],	1,	8},
	{TESTSYS_VAR_END_OF_TABLE,		0,	0}
};
#endif // #ifdef SHOW_CAN_RECONNECT

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/
static li_can_slv_errorcode_t can_reconnect_on(uint16_t id);
static li_can_slv_errorcode_t can_reconnect_off(uint16_t id);
static li_can_slv_errorcode_t can_reconnect_next_baudrate(void);

#ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
static void can_reconnect_call_online_change_baudrate_start_cbk(void);
#endif // #ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK

#ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK
static void can_reconnect_call_startup_change_baudrate_start_cbk(void);
#endif // #ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
/**
 * @brief reconnect initialization
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_reconnect_init(void)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

#ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT
	LI_CAN_SLV_DEBUG_PRINT("cr init\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT

#ifdef SHOW_CAN_RECONNECT
	can_reconnect_var_change[0].info = can_reconnect_state_info[can_reconnect.state];
	can_reconnect_var_change[1].info = can_reconnect_cause_info[can_reconnect.cause];
	can_reconnect_var_change[2].info = can_reconnect_back_info[can_reconnect.back];
	/*----------------------------------------------------------------------*/
	/* add can main IO's                                                    */
	/*----------------------------------------------------------------------*/
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = var_add(&can_reconnect_var_const[0]);
	}

	/*----------------------------------------------------------------------*/
	/* add can main IO's to testsys                                         */
	/*----------------------------------------------------------------------*/
#ifdef TESTSYS_SYSTEM
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = testsys_system_page_add(&can_reconnect_testsys[0], "CAN reconnect system");
	}
#endif // #ifdef TESTSYS_SYSTEM
#endif // #ifdef SHOW_CAN_RECONNECT
	return (err);
}

/*!
 * \brief startup reconnect routine
 * \remarks the li_can_slv_reconnect_startup is called at every startup of the module. First the current baud rate \n
 * of the CAN bus is measured. While the measurement there is no influence to the CAN bus possible.\n
 * After detection of the current CAN baud rate all initializations are started and the module will\n
 * be connected to CAN bus. If no CAN event is detected (e. g.: error frame) and the time of baud rate \n
 * measurement exceeds the process safety time by 110% the module will be connect to CAN bus. This is\n
 * the normal case of a module startup. When a module is connected to CAN there will be no CAN traffic\n
 * (event), because the master is in his initialization condition.\n
 * \return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_reconnect_startup(void)
{
	if (can_reconnect.state == CAN_RECONNECT_STATE_STARTUP)
	{
#ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT_STARTUP
		LI_CAN_SLV_DEBUG_PRINT("cr on startup: %ld\n", can_port_get_system_ticks());
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT_STARTUP

#ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK
		can_reconnect_call_startup_change_baudrate_start_cbk();
#endif // #ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK

		can_reconnect.time = can_port_get_system_ticks() + can_port_msec_2_ticks(CAN_RECONNECT_STARTUP_NO_TRAFFIC_TIME);
		can_reconnect.cause = CAN_RECONNECT_CAUSE_STARTUP_NO_TRAFFIC;

		li_can_slv_reconnect_main_node_msg_pending = CAN_RECONNECT_INTID_NO_INTERRUPT_PENDING;
		li_can_slv_reconnect_mon_node_msg_pending = CAN_RECONNECT_INTID_NO_INTERRUPT_PENDING;

		can_reconnect_on(1);
	}
	return (LI_CAN_SLV_ERR_OK);
}

/*!
 * \brief can reconnect download
 * \remarks the li_can_slv_reconnect_startup is called at every startup of the module in download mode.
 * \return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_reconnect_download(void)
{
#ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT
	LI_CAN_SLV_DEBUG_PRINT("dl\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT

	can_reconnect.cause = CAN_RECONNECT_CAUSE_DOWNLOAD_ACKN;
	can_reconnect.back = CAN_RECONNECT_BACK_DOWNLOAD_ACKN;
	return (LI_CAN_SLV_ERR_OK);
}

/*!
 * \brief CAN reconnect get state
 * \return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_reconnect_state_t li_can_slv_reconnect_get_state(void)
{
	return (can_reconnect.state);
}

/*!
 * \brief CAN reconnect set state
 * \return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_reconnect_set_state(li_can_slv_reconnect_state_t state)
{
	can_reconnect.state = state;
	return (LI_CAN_SLV_ERR_OK);
}

/*!	\brief online reconnect routine for main CAN-Controller
 \remarks The can_reconnect_online_main is called in case of CAN error warning condition. The CAN-controller which\n
 influences the CAN-bus will be immediately deactivated and the measurement of the current CAN baud rate \n
 will be started. The reconnect online will be stay in the condition of measuring the CAN baud rate until\n
 a valid value of CAN baud rate is detected.\n
 \param ewrn
 \return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful */
li_can_slv_errorcode_t li_can_slv_reconnect_on_main_node_online(uint16_t ewrn)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if ((can_reconnect.state == CAN_RECONNECT_STATE_OFF) && (ewrn != 0x0000) && (lcsa_get_state() != LI_CAN_SLV_STATE_STOPPED))
	{
#ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT
		LI_CAN_SLV_DEBUG_PRINT("mo\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT

#ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
		can_reconnect_call_online_change_baudrate_start_cbk();
#endif // #ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK

#ifdef LI_CAN_SLV_RECONNECT_MAIN_NODE
		// change to listen only
		(void)li_can_slv_set_node_mode(LI_CAN_SLV_MODE_STOPPED);
		err = li_can_slv_set_node_mode(LI_CAN_SLV_MODE_LISTEN_ONLY);
#endif // #ifdef LI_CAN_SLV_RECONNECT_MAIN_NODE

		can_reconnect.state = CAN_RECONNECT_STATE_ON;
		can_reconnect.cause = CAN_RECONNECT_CAUSE_ONLINE_MAIN_EWRN;

		li_can_slv_reconnect_main_node_msg_pending = CAN_RECONNECT_INTID_LEC_IE_TXOK_RXOK;
		li_can_slv_reconnect_mon_node_msg_pending = CAN_RECONNECT_INTID_LEC_IE_TXOK_RXOK;

		err = can_reconnect_on(10);
	}
	return (err);
}

/*!	\brief reconnect recovery sequence
 \remarks in case of CAN bus off situation of main CAN-controller the can_reconnect_recovery\n
 routine (sequence) will be started.\n
 \param boff
 \return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_reconnect_on_main_node_recovery(uint16_t boff)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if ((can_reconnect.state == CAN_RECONNECT_STATE_OFF) && (boff != 0x0000) && (lcsa_get_state() != LI_CAN_SLV_STATE_STOPPED))
	{
#ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT
		LI_CAN_SLV_DEBUG_PRINT("cr m rec\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT

#ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
		can_reconnect_call_online_change_baudrate_start_cbk();
#endif // #ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK

#ifdef LI_CAN_SLV_RECONNECT_MAIN_NODE
		// change to listen only
		(void)li_can_slv_set_node_mode(LI_CAN_SLV_MODE_STOPPED);
		err = li_can_slv_set_node_mode(LI_CAN_SLV_MODE_LISTEN_ONLY);
#endif // #ifdef LI_CAN_SLV_RECONNECT_MAIN_NODE

		can_reconnect.state = CAN_RECONNECT_STATE_ON;
		can_reconnect.cause = CAN_RECONNECT_CAUSE_ONLINE_MAIN_BOFF;

		li_can_slv_reconnect_main_node_msg_pending = CAN_RECONNECT_INTID_LEC_IE_TXOK_RXOK;
		li_can_slv_reconnect_mon_node_msg_pending = CAN_RECONNECT_INTID_LEC_IE_TXOK_RXOK;

		err = can_reconnect_on(20);
	}
	return (err);
}

#ifdef LI_CAN_SLV_RECONNECT_MON_NODE
/*!	\brief online reconnect routine for monitor CAN-Controller
 \remarks the can_reconnect_online_mon is called in case of CAN error warning condition. The CAN-controller which\n
 influences the CAN-bus will be immediately deactivated and the measurement of the current CAN baud rate \n
 will be started. The reconnect online will be stay in the condition of measuring the CAN baud rate until\n
 a valid value of CAN baud rate is detected.\n
 \param ewrn
 \return	#li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful */
li_can_slv_errorcode_t li_can_slv_reconnect_on_mon_node_online(uint16_t ewrn)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if ((can_reconnect.state == CAN_RECONNECT_STATE_OFF) && (ewrn != 0x0000))
	{
		can_reconnect.state = CAN_RECONNECT_STATE_ON;
		can_reconnect.cause = CAN_RECONNECT_CAUSE_ONLINE_MON_EWRN;
		err = can_reconnect_on(30);
	}
	return (err);
}
#endif // #ifdef LI_CAN_SLV_RECONNECT_MON_NODE

#ifdef LI_CAN_SLV_RECONNECT_MON_NODE
/*!	\brief reconnect recovery sequence
 \remarks in case of CAN bus off situation of mon CAN-controller the can_reconnect_recovery\n
 routine (sequence) will be started.\n
 \param boff
 \return	#li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_reconnect_on_mon_node_recovery(uint16_t boff)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if ((can_reconnect.state == CAN_RECONNECT_STATE_OFF) && (boff != 0x0000))
	{
		can_reconnect.state = CAN_RECONNECT_STATE_ON;
		can_reconnect.cause = CAN_RECONNECT_CAUSE_ONLINE_MON_BOFF;
		err = can_reconnect_on(40);
	}
	return (err);
}
#endif // #ifdef LI_CAN_SLV_RECONNECT_MON_NODE

/*!
 * \brief li_can_slv_reconnect_process_on
 */
void li_can_slv_reconnect_process_on(void)
{
	process_on_off = TRUE;
}

/*!
 * \brief li_can_slv_reconnect_process_off
 */
void li_can_slv_reconnect_process_off(void)
{
	process_on_off = FALSE;
}

/*!
 * \brief reconnect process
 * \param intid
 * \param lec
 * \return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_reconnect_process(int16_t intid, int16_t lec)
{
#ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT_PROCESS
	LI_CAN_SLV_DEBUG_PRINT("intid:%d, lec:%d, state:%d\n", intid, lec, can_reconnect.state);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT_PROCESS

	if (FALSE == process_on_off)
	{
		return (LI_CAN_SLV_ERR_OK);
	}

	switch (can_reconnect.state)
	{
		case CAN_RECONNECT_STATE_OFF:
			/*--------------------------------------------------------------*/
			/* reconnect off                                                */
			/*--------------------------------------------------------------*/
			break;

		case CAN_RECONNECT_STATE_STARTUP:
			/*--------------------------------------------------------------*/
			/* reconnect startup                                            */
			/*--------------------------------------------------------------*/
			if (intid == CAN_RECONNECT_INTID_MSG)
			{
				/*----------------------------------------------------------*/
				/* any valid event on CAN                                   */
				/*----------------------------------------------------------*/
				can_reconnect.cause = CAN_RECONNECT_CAUSE_STARTUP;
				can_reconnect.back = CAN_RECONNECT_BACK_STARTUP_VALID_EVENT;
				can_reconnect_off(0);
				return (LI_CAN_SLV_ERR_OK);
			}

			if (can_port_get_system_ticks() > can_reconnect.time)
			{
				/*----------------------------------------------------------*/
				/* no event on CAN since startup                            */
				/*----------------------------------------------------------*/
				can_reconnect.cause = CAN_RECONNECT_CAUSE_STARTUP_NO_TRAFFIC;
				can_reconnect.back = CAN_RECONNECT_BACK_STARTUP_TIME;
				can_reconnect.back_bdr = can_config_get_baudrate_startup();

#ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT
				LI_CAN_SLV_DEBUG_PRINT("no event since startup\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT

				can_reconnect_off(1);
				return (LI_CAN_SLV_ERR_OK);
			}

#ifdef LI_CAN_SLV_RECONNECT_PROCESS_STARTUP_CHECK_LEC
			/*
			 * For some implementations you need to check the lec!
			 * If the startup has no other can node which ACKs the frame you will stay in the reconnect phase
			 * until the can_reconnect.time is reached
			 */
			if (intid == CAN_RECONNECT_INTID_LEC_IE_TXOK_RXOK || lec != CAN_LEC_NO_ERROR)
#else
			if (intid == CAN_RECONNECT_INTID_LEC_IE_TXOK_RXOK)
#endif // #ifdef LI_CAN_SLV_RECONNECT_PROCESS_STARTUP_CHECK_LEC
			{
				/*----------------------------------------------------------*/
				/* no valid event on CAN                                    */
				/*----------------------------------------------------------*/
				can_reconnect.cause = CAN_RECONNECT_CAUSE_STARTUP;
				can_reconnect.state = CAN_RECONNECT_STATE_ON;
				can_reconnect_on(100);
				return (LI_CAN_SLV_ERR_OK);
			}

			// switch next baud rate (if time has passed)
			if (can_port_get_system_ticks() > can_reconnect.time_next_bdr_change)
			{
				can_reconnect_next_baudrate();
			}
			break;

		case CAN_RECONNECT_STATE_ON:
			/*--------------------------------------------------------------*/
			/* reconnect on                                                 */
			/*--------------------------------------------------------------*/
			if (intid == CAN_RECONNECT_INTID_MSG)
			{
				/*----------------------------------------------------------*/
				/* any valid event on CAN                                   */
				/*----------------------------------------------------------*/
				can_reconnect.back = CAN_RECONNECT_BACK_VALID_EVENT;
				can_reconnect_off(10);
			}
			else if ((intid == CAN_RECONNECT_INTID_LEC_IE_TXOK_RXOK) && (lec == CAN_LEC_NO_ERROR))
			{
				// switch next baud rate (if time has passed)
				if (can_port_get_system_ticks() > can_reconnect.time_next_bdr_change)
				{
					/* reset lec_cnt */
					can_reconnect.lec_cnt_bit0 = 0;
					can_reconnect_next_baudrate();
					can_reconnect.idle_cnt = 0;
					can_reconnect.state = CAN_RECONNECT_STATE_IDLE;
				}
			}
			else
			{
				switch (lec)
				{
					case CAN_LEC_NO_ERROR:
						/*------------------------------------------------------*/
						/* message ok --> valid baud rate                       */
						/*------------------------------------------------------*/
						can_reconnect.back = CAN_RECONNECT_BACK_NO_LEC;
						can_reconnect_off(11);
						break;

					case CAN_LEC_BIT0:
						can_reconnect.lec_cnt_bit0++;
						if (can_reconnect.lec_cnt_bit0 >= CAN_RECONNECT_BDR_DETECT_NR_LEC_BIT0)
						{
							/*--------------------------------------------------*/
							/* Bit0-errors in serie -> valid baud rate          */
							/*--------------------------------------------------*/
							can_reconnect.back = CAN_RECONNECT_BACK_BIT0;
							can_reconnect_off(12);
						}
						break;

					case CAN_LEC_STUFF:
					case CAN_LEC_FORM:
					case CAN_LEC_ACK:
					case CAN_LEC_BIT1:
					case CAN_LEC_CRC:
						/*------------------------------------------------------*/
						/* any other lec --> not valid baud rate                */
						/*------------------------------------------------------*/
						/* reset lec_cnt */

						can_reconnect.lec_cnt_bit0 = 0;
						// switch next baud rate (if time has passed)
						if (can_port_get_system_ticks() > can_reconnect.time_next_bdr_change)
						{

							/* switch next baud rate */
							can_reconnect_next_baudrate();
							can_reconnect.idle_cnt = 0;
							can_reconnect.state = CAN_RECONNECT_STATE_IDLE;
						}

						break;

					case CAN_LEC_NOT_DEFINED:
						if (intid == CAN_RECONNECT_INTID_NO_INTERRUPT_PENDING)
						{
							/*--------------------------------------------------*/
							/* undefined lec and no interrupt pending           */
							/*--------------------------------------------------*/
							/**
							 * Verify for CANStress Test (PME1IOCAN-13)
							 */
#if 1
							can_reconnect.back = CAN_RECONNECT_BACK_NO_IRQ_PENDING;
							can_reconnect_off(13);
#else
							if (intid == CAN_RECONNECT_INTID_NO_INTERRUPT_PENDING)
							{
								can_reconnect_next_baudrate();
							}
#endif
						}
						break;

					default:
						break;
				}
			}
			break;

		case CAN_RECONNECT_STATE_IDLE:
			/*--------------------------------------------------------------*/
			/* reconnect on                                                 */
			/*--------------------------------------------------------------*/
			can_reconnect.idle_cnt++;
			if (can_reconnect.idle_cnt > CAN_RECONNECT_IDLE_CNT)
			{
				can_reconnect.state = CAN_RECONNECT_STATE_ON;
			}
			break;

		default:
			break;
	}

#ifdef SHOW_CAN_RECONNECT
	can_reconnect_var_change[0].info = can_reconnect_state_info[can_reconnect.state];
	can_reconnect_var_change[1].info = can_reconnect_cause_info[can_reconnect.cause];
	can_reconnect_var_change[2].info = can_reconnect_back_info[can_reconnect.back];
#endif // #ifdef SHOW_CAN_RECONNECT

	return (LI_CAN_SLV_ERR_OK);
}

#ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
/**
 * @brief This function sets the reconnect online change baud rate start callback
 * @param cbk function to handle the reconnect online change baud rate start callback
 */
void li_can_slv_reconnect_set_online_change_baudrate_start_cbk(li_can_slv_reconnect_online_change_baudrate_start_cbk_funcp_t cbk)
{
	if (NULL == reconnect_online_change_baudrate_start_cbk_funcp)
	{
		reconnect_online_change_baudrate_start_cbk_funcp = cbk;
	}
}

/**
 * @brief This function sets the reconnect online change baud rate stop callback
 * @param cbk function to handle the reconnect online change baud rate stop callback
 */
void li_can_slv_reconnect_set_online_change_baudrate_stop_cbk(li_can_slv_reconnect_online_change_baudrate_stop_cbk_funcp_t cbk)
{
	if (NULL == reconnect_online_change_baudrate_stop_cbk_funcp)
	{
		reconnect_online_change_baudrate_stop_cbk_funcp = cbk;
	}
}
#endif // #ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK

#ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK
/**
 * @brief This function sets the reconnect startup change baud rate start callback
 * @param cbk function to handle the reconnect startup change baud rate start callback
 */
void li_can_slv_reconnect_set_startup_change_baudrate_start_cbk(li_can_slv_reconnect_startup_change_baudrate_start_cbk_funcp_t cbk)
{
	if (NULL == reconnect_startup_change_baudrate_start_cbk_funcp)
	{
		reconnect_startup_change_baudrate_start_cbk_funcp = cbk;
	}
}

/**
 * @brief This function sets the reconnect startup change baud rate stop callback
 * @param cbk function to handle the reconnect startup change baud rate stop callback
 */
void li_can_slv_reconnect_set_startup_change_baudrate_stop_cbk(li_can_slv_reconnect_startup_change_baudrate_stop_cbk_funcp_t cbk)
{
	if (NULL == reconnect_startup_change_baudrate_stop_cbk_funcp)
	{
		reconnect_startup_change_baudrate_stop_cbk_funcp = cbk;
	}
}
#endif // #ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/
/*!
 * \brief CAN reconnect on sequence
 * \return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t can_reconnect_on(uint16_t id)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	lcsa_set_state(LI_CAN_SLV_STATE_RECONNECT);

#ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT
	LI_CAN_SLV_DEBUG_PRINT("cr on %d, %d\n", id, lcsa_get_baudrate());
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT

	can_reconnect.lec_cnt_bit0 = 0;
	can_reconnect.id_on = id;
	switch (id)
	{
		case 1: /* li_can_slv_reconnect_startup */
			can_reconnect.bdr_index = 0;
			can_reconnect.time_next_bdr_change = can_port_get_system_ticks() + can_port_msec_2_ticks(LI_CAN_SLV_RECONNECT_BAUDRATE_SWITCH_TIME);
			break;
		default:
			can_reconnect_next_baudrate();
			break;
	}

	li_can_slv_reconnect_process_on();

	return (err);
}

/*!
 * \brief CAN reconnect off sequence
 * \return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t can_reconnect_off(uint16_t id)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	lcsa_bdr_t baud;

#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
#ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
	uint8_t call_online_cbk;
#endif // #ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK

#ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK
	uint8_t call_startup_cbk;
#endif // #ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK

	baud = can_reconnect.back_bdr;

	// disable monitor CAN-controller
#if defined(LI_CAN_SLV_MON) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY)
	can_mon_disable();
#endif // #if defined(LI_CAN_SLV_MON) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY)

	li_can_slv_reconnect_process_off();

#if defined(OUTER) || defined(OUTER_APP)
	// free all async/sys/sync CAN TX objects for sending
	can_main_hw_free_tx_objs();
#endif // #if defined(OUTER) || defined(OUTER_APP)

#ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT_STARTUP
	LI_CAN_SLV_DEBUG_PRINT("cr off %d, with baudrate %d\n", id, baud);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT_STARTUP

#ifdef LI_CAN_SLV_MAIN_MON
	// on main mon system also send the baudrate to the mon cpu
	li_can_slv_port_sync_baudrate(baud);
#endif // #ifdef LI_CAN_SLV_MAIN_MON

	// ignore error in stopped mode
	(void)li_can_slv_set_node_mode(LI_CAN_SLV_MODE_STOPPED);

	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_config_set_baudrate(baud);
	}
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = li_can_slv_set_node_mode(LI_CAN_SLV_MODE_OPERATIONAL);
#ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT
		LI_CAN_SLV_DEBUG_PRINT("cr set mode err: %08x\n", err);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT

		if (err == LI_CAN_SLV_ERR_OK)
		{
			lcsa_set_state(LI_CAN_SLV_STATE_RUNNING);
		}
	}

	// set reconnect off
	can_reconnect.state = CAN_RECONNECT_STATE_OFF;
	can_reconnect.id_off = id;

	// increment reconnect number
	can_reconnect.nr++;

	if (err == LI_CAN_SLV_ERR_OK)
	{
#ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
		call_online_cbk = FALSE;
#endif // #ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK

#ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK
		call_startup_cbk = FALSE;
#endif // #ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK

		switch (can_reconnect.cause)
		{
			case CAN_RECONNECT_CAUSE_STARTUP:
				// if the cause was startup then the next state must be init, so set the state to init
				lcsa_set_state(LI_CAN_SLV_STATE_INIT);
#ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK
				call_startup_cbk = TRUE;
#endif // #ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK
				switch (baud)
				{
					case 125:
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
						error_syserr_send(ERR_MSG_CAN_CONNECT_STARTUP_125kB, ERR_LVL_UNAWARE, can_config_get_module_nr_main(), ERR_LVL_UNAWARE);
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
						break;

					case 250:
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
						error_syserr_send(ERR_MSG_CAN_CONNECT_STARTUP_250kB, ERR_LVL_UNAWARE, can_config_get_module_nr_main(), ERR_LVL_UNAWARE);
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
						break;

					case 500:
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
						error_syserr_send(ERR_MSG_CAN_CONNECT_STARTUP_500kB, ERR_LVL_UNAWARE, can_config_get_module_nr_main(), ERR_LVL_UNAWARE);
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
						break;

					case 1000:
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
						error_syserr_send(ERR_MSG_CAN_CONNECT_STARTUP_1000kB, ERR_LVL_UNAWARE, can_config_get_module_nr_main(), ERR_LVL_UNAWARE);
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERRO
						break;

					default:
						break;
				}
#ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT
				LI_CAN_SLV_DEBUG_PRINT("CAN_RECONNECT_CAUSE_STARTUP %d Baud\n", baud);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT
				break;

			case CAN_RECONNECT_CAUSE_ONLINE_MAIN_EWRN:
			case CAN_RECONNECT_CAUSE_ONLINE_MAIN_BOFF:
			case CAN_RECONNECT_CAUSE_ONLINE_MON_EWRN:
			case CAN_RECONNECT_CAUSE_ONLINE_MON_BOFF:
#ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
				call_online_cbk = TRUE;
#endif // #ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
				lcsa_set_state(LI_CAN_SLV_STATE_RUNNING);
				switch (baud)
				{
					case 125:
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
						error_syserr_send(ERR_MSG_CAN_CONNECT_ONLINE_125kB, ERR_LVL_UNAWARE, can_config_get_module_nr_main(), ERR_LVL_UNAWARE);
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
						break;

					case 250:
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
						error_syserr_send(ERR_MSG_CAN_CONNECT_ONLINE_250kB, ERR_LVL_UNAWARE, can_config_get_module_nr_main(), ERR_LVL_UNAWARE);
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
						break;

					case 500:
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
						error_syserr_send(ERR_MSG_CAN_CONNECT_ONLINE_500kB, ERR_LVL_UNAWARE, can_config_get_module_nr_main(), ERR_LVL_UNAWARE);
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
						break;

					case 1000:
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
						error_syserr_send(ERR_MSG_CAN_CONNECT_ONLINE_1000kB, ERR_LVL_UNAWARE, can_config_get_module_nr_main(), ERR_LVL_UNAWARE);
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
						break;

					default:
						break;
				}
#ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT
				LI_CAN_SLV_DEBUG_PRINT("CR CAUSE OL %d\n", baud);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT
				break;

			case CAN_RECONNECT_CAUSE_STARTUP_NO_TRAFFIC:
#ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK
				call_startup_cbk = TRUE;
#endif // #ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK
				// if the cause was startup then the next state must be init, so set the state to init
				lcsa_set_state(LI_CAN_SLV_STATE_INIT);
#ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT
				LI_CAN_SLV_DEBUG_PRINT("CR CAUSE NO_TRAFFIC\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT
				break;
			case CAN_RECONNECT_CAUSE_DOWNLOAD_ACKN:
				// if the cause was startup then the next state must be init, so set the state to init
				lcsa_set_state(LI_CAN_SLV_STATE_INIT);
#ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT
				LI_CAN_SLV_DEBUG_PRINT("CR CAUSE DL_ACKN %d\n", baud);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT
				break;
			default:
				break;
		}

#ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK
		if ((call_online_cbk == TRUE) && (NULL != reconnect_online_change_baudrate_stop_cbk_funcp))
		{
			reconnect_online_change_baudrate_stop_cbk_funcp(baud);
		}
#endif // #ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK

#ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
		if ((call_startup_cbk == TRUE) && (NULL != reconnect_startup_change_baudrate_stop_cbk_funcp))
		{
			reconnect_startup_change_baudrate_stop_cbk_funcp(baud);
		}
#endif // #ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
	}

	can_reconnect.back_time = li_can_slv_port_ticks_2_msec(can_port_get_system_ticks()); /* remember when reconnect was finished */
	return (err);
}

/*!
 * \brief CAN reconnect next baudrate
 * \return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t can_reconnect_next_baudrate(void)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

#if defined(LI_CAN_SLV_MON) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY)
	can_mon_disable();
#endif // #if defined(LI_CAN_SLV_MON) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY)

	if (can_reconnect.bdr_index < CAN_RECONNECT_MAX_BDR_INDEX - 1)
	{
		can_reconnect.bdr_index++;
	}
	else
	{
		can_reconnect.bdr_index = 0;
	}

	// change to listen only
	(void)li_can_slv_set_node_mode(LI_CAN_SLV_MODE_STOPPED);
	err = can_config_set_baudrate(can_reconnect_bdr_table[can_reconnect.bdr_index]);
	can_reconnect.back_bdr = can_reconnect_bdr_table[can_reconnect.bdr_index];
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = li_can_slv_set_node_mode(LI_CAN_SLV_MODE_LISTEN_ONLY);
	}

#ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT
	LI_CAN_SLV_DEBUG_PRINT("next: %d\n", can_reconnect_bdr_table[can_reconnect.bdr_index]);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_RECONNECT

	//update the time stamp of next allowed baud rate change to be able to check idle time
	can_reconnect.time_next_bdr_change = can_port_get_system_ticks() + can_port_msec_2_ticks(LI_CAN_SLV_RECONNECT_BAUDRATE_SWITCH_TIME);

#if defined(LI_CAN_SLV_MON) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY)
	can_mon_enable();
#endif // #if defined(LI_CAN_SLV_MON) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY)

	return (err);
}

/*!
 * \brief Get the time stamp when the reconnect has been finished
 * \return time stamp of finished reconnect or 0.
 */
uint32_t li_can_slv_reconnect_get_back_time_ms(void)
{
	return can_reconnect.back_time;
}


#ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
/**
 *
 */
static void can_reconnect_call_online_change_baudrate_start_cbk(void)
{
	if (NULL != reconnect_online_change_baudrate_start_cbk_funcp)
	{
		reconnect_online_change_baudrate_start_cbk_funcp();
	}
}
#endif // #ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK

#ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK
/**
 *
 */
static void can_reconnect_call_startup_change_baudrate_start_cbk(void)
{
	if (NULL != reconnect_startup_change_baudrate_start_cbk_funcp)
	{
		reconnect_startup_change_baudrate_start_cbk_funcp();
	}
}
#endif // #ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK

/** @} */

