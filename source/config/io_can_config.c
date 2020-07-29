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

/*!
 * \brief All data records and configuration functions are defined in can_config
 * to support the Liebherr CAN Protocol
 * \remarks
 * - <b>logical CAN module</b><br>
 * A logical CAN module includes following attributes:
 * 	- module type<br>
 * 	Every logical CAN module is defined by a module type.
 * 	- module number<br>
 * 	The range of valid module numbers starts at the number 2 and ends at the number 124. Module number 1
 * 	is reserved for the CAN master and the module numbers 125, 126, 127 and 128 are illegal module numbers
 * 	where the system message communication is possible, for debugging and service.<br>
 *
 * 	- RX system message object<br>
 * 	For general communication (e. g. status request) between CAN master and CAN module a received system
 * 	message object has to be defined.
 * 	 - 1 object for all logical modules used for broadcast system messages
 * 	 - 4 objects (1 CAN message object with mask arbitration) for each logical module<br>
 *
 * 	- TX system message object<br>
 * 	For general communication (e. g. status answer) between CAN master and CAN module a transmit system
 * 	message object has to be defined.<br>
 * 	For the 4 TX data objects of one logical module one CAN message object with online changing arbitration
 * 	is used.<br>
 * 	- RX process request
 * 	1 CAN message object
 * 	- RX data objects and RX-conversion-code<br>
 * 	A logical module owns up to 4 RX data objects with defined DLC [0...8]. For the 4 RX data objects of
 * 	one logical module one CAN message object with mask arbitration is used. If a correct process image is
 * 	received the RX-conversion-code is called.
 * 	- TX data objects and its TX-conversion-code<br>
 * 	A logical module owns up to 4 TX data objects with defined DLC [0...8]. For the 4 TX data objects of
 * 	one logical module one CAN message object with online changing arbitration is used. If a correct process
 * 	image is received the TX-conversion-code is called.
 * 	- RX asynchronous data objects<br>
 * 	- TX asynchronous data objects<br>
 * 	The asynchronous TX data are send directly (inner core) or via asynchronous transmit queue (outer core).
 * 	Therefore one CAN driver object with online changing arbitration is used.<br>
 * 	- repeat rate<br>
 * 	The repeat rate of a logical module defines the commonness of the module number objects	(4 receive and 4 transmit).<br>
 * 	- CAN baud rate<br>
 * 	See therefore #can_config_set_baudrate
 */

/**
 * @file io_can_config.c
 * @addtogroup can_config
 * @{
 */
/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/io_can_config.h>
#include "io_can_port.h"

#include <li_can_slv/config/li_can_slv_config_defines.h>

#ifdef LI_CAN_SLV_TUPLE
#include "io_tuple.h"
#include "io_tuple_id.h"
#endif // #ifdef LI_CAN_SLV_TUPLE

#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
#include <li_can_slv/xload/li_can_slv_xload_defines.h>
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG

#include "io_can_hw.h"
#include <li_can_slv/core/io_can_main.h>
#include "io_can_main_hw.h"
#include <li_can_slv/core/io_can.h>

#include <li_can_slv/error/io_can_error.h>
#include <li_can_slv/error/io_can_errno.h>

#if defined(LI_CAN_SLV_MON) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY)
#include <li_can_slv/core/io_can_mon.h>
#include "io_can_mon_hw.h"
#endif // #if defined(LI_CAN_SLV_MON) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY)

#ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR
#include <li_can_slv/config/io_can_config_types.h>
#endif // #ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR

#if (defined(OUTER) || defined(OUTER_APP))
#ifdef SHOW_CAN_CONFIG
#include "io_var.h"
#ifdef TESTSYS_SYSTEM
#include "io_testsys.h"
#endif // #ifdef TESTSYS_SYSTEM
#endif // #ifdef SHOW_CAN_CONFIG
#ifdef BGNDCMD
#include "io_bgndcmd.h"
#endif // #ifdef BGNDCMD
#endif // #if defined(OUTER) || defined(OUTER_APP)

#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_debug.h"
#endif // #ifdef LI_CAN_SLV_DEBUG

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/
static li_can_slv_config_bdr_t can_config_bdr_startup;

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/
#ifndef LI_CAN_SLV_BOOT
uint16_t can_config_nr_of_modules = 0; /**< number of modules*/
#endif // #ifndef LI_CAN_SLV_BOOT

#ifdef LI_CAN_SLV_BOOT
li_can_slv_config_module_t can_config_module_tab =
{
	CAN_CONFIG_MODULE_TYPE_NOT_USED,
	LI_CAN_SLV_CONFIG_DEF_MOD_NR1
};
#else // #ifdef LI_CAN_SLV_BOOT
li_can_slv_config_module_t can_config_module_tab[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES] = /**< can configuration module table*/
{
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 1
	/* initialization of logical module 1 */
	{
		CAN_CONFIG_MODULE_TYPE_NOT_USED,
		LI_CAN_SLV_CONFIG_DEF_MOD_NR1,
#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
		{TUPLE_ID_INVALID, DLOAD_CRC_DEFAULT, DLOAD_DATE_DEFAULT, NULL},
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#ifdef LI_CAN_SLV_MAIN_MON
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
		{0, 0, 0, 0},
#ifdef LI_CAN_SLV_MAIN_MON
		{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{0, 0, 0, 0},
		{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MON
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#ifdef LI_CAN_SLV_MAIN_MON
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
	},
#endif //LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 1
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 2
	/* initialization of logical module 2 */
	{
		CAN_CONFIG_MODULE_TYPE_NOT_USED,
		LI_CAN_SLV_CONFIG_DEF_MOD_NR2,
#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
		{TUPLE_ID_INVALID, DLOAD_CRC_DEFAULT, DLOAD_DATE_DEFAULT, NULL},
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#ifdef LI_CAN_SLV_MAIN_MON
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
		{0, 0, 0, 0},
#ifdef LI_CAN_SLV_MAIN_MON
		{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{0, 0, 0, 0},
		{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MON
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#ifdef LI_CAN_SLV_MAIN_MON
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
	},
#endif //LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 2
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 3
	/* initialization of logical module 3 */
	{
		CAN_CONFIG_MODULE_TYPE_NOT_USED,
		LI_CAN_SLV_CONFIG_DEF_MOD_NR3,
#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
		{TUPLE_ID_INVALID, DLOAD_CRC_DEFAULT, DLOAD_DATE_DEFAULT, NULL},
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#ifdef LI_CAN_SLV_MAIN_MON
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
		{0, 0, 0, 0},
#ifdef LI_CAN_SLV_MAIN_MON
		{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{0, 0, 0, 0},
		{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MON
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#ifdef LI_CAN_SLV_MAIN_MON
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
	},
#endif //LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 3
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 4
	/* initialization of logical module 4 */
	{
		CAN_CONFIG_MODULE_TYPE_NOT_USED,
		LI_CAN_SLV_CONFIG_DEF_MOD_NR4,
#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
		{TUPLE_ID_INVALID, DLOAD_CRC_DEFAULT, DLOAD_DATE_DEFAULT, NULL},
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#ifdef LI_CAN_SLV_MAIN_MON
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
		{0, 0, 0, 0},
#ifdef LI_CAN_SLV_MAIN_MON
		{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{0, 0, 0, 0},
		{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MON
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#ifdef LI_CAN_SLV_MAIN_MON
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
	},
#endif //LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 4
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 5
	/* initialization of logical module 5 */
	{
		CAN_CONFIG_MODULE_TYPE_NOT_USED,
		LI_CAN_SLV_CONFIG_DEF_MOD_NR5,
#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
		{TUPLE_ID_INVALID, DLOAD_CRC_DEFAULT, DLOAD_DATE_DEFAULT, NULL},
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#ifdef LI_CAN_SLV_MAIN_MON
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
		{0, 0, 0, 0},
#ifdef LI_CAN_SLV_MAIN_MON
		{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{0, 0, 0, 0},
		{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MON
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#ifdef LI_CAN_SLV_MAIN_MON
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
	},
#endif //LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 5
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 6
	/* initialization of logical module 6 */
	{
		CAN_CONFIG_MODULE_TYPE_NOT_USED,
		LI_CAN_SLV_CONFIG_DEF_MOD_NR6,
#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
		{TUPLE_ID_INVALID, DLOAD_CRC_DEFAULT, DLOAD_DATE_DEFAULT, NULL},
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#ifdef LI_CAN_SLV_MAIN_MON
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
		{0, 0, 0, 0},
#ifdef LI_CAN_SLV_MAIN_MON
		{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{0, 0, 0, 0},
		{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MON
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#ifdef LI_CAN_SLV_MAIN_MON
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
	},
#endif //LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 6
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 7
	/* initialization of logical module 7 */
	{
		CAN_CONFIG_MODULE_TYPE_NOT_USED,
		LI_CAN_SLV_CONFIG_DEF_MOD_NR7,
#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
		{TUPLE_ID_INVALID, DLOAD_CRC_DEFAULT, DLOAD_DATE_DEFAULT, NULL},
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#ifdef LI_CAN_SLV_MAIN_MON
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
		{0, 0, 0, 0},
#ifdef LI_CAN_SLV_MAIN_MON
		{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{0, 0, 0, 0},
		{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MON
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#ifdef LI_CAN_SLV_MAIN_MON
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
	},
#endif //LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 7
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 8
	/* initialization of logical module 8 */
	{
		CAN_CONFIG_MODULE_TYPE_NOT_USED,
		LI_CAN_SLV_CONFIG_DEF_MOD_NR8,
#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
		{TUPLE_ID_INVALID, DLOAD_CRC_DEFAULT, DLOAD_DATE_DEFAULT, NULL},
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#ifdef LI_CAN_SLV_MAIN_MON
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
		{0, 0, 0, 0},
#ifdef LI_CAN_SLV_MAIN_MON
		{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{0, 0, 0, 0},
		{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MON
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0},
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#ifdef LI_CAN_SLV_MAIN_MON
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		{NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
	},
#endif //LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 8
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 9
#warning LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES out of range!
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES
};
#endif // #ifdef LI_CAN_SLV_BOOT

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_BOOT
static can_config_module_silent_t can_config_module_silent = LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE; /**< module silent or awake for boot loader */
#else // #ifdef LI_CAN_SLV_BOOT
static can_config_module_silent_t can_config_module_silent[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES] =
{
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 1
	/* initialization of logical module 1 */
	LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE,
#endif
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 2
	/* initialization of logical module 2 */
	LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE,
#endif
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 3
	/* initialization of logical module 3 */
	LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE,
#endif
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 4
	/* initialization of logical module 4 */
	LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE,
#endif
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 5
	/* initialization of logical module 5 */
	LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE,
#endif
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 6
	/* initialization of logical module 6 */
	LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE,
#endif
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 7
	/* initialization of logical module 7 */
	LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE,
#endif
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 8
	/* initialization of logical module 8 */
	LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE
#endif
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 9
#warning LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES out of range!
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES
}; /**< module silent or awake */
#endif // #ifdef LI_CAN_SLV_BOOT

static can_config_bdr_tab_t can_config_bdr_current = {0, 0, 0, 0, 0, 0, 0}; /**< current baud rate table */
static can_config_bdr_tab_t can_config_bdr_tab[CAN_CONFIG_SIZE_OF_BDR_TAB] = /**< baud rate table */
{
	{ CAN_CONFIG_DEF_BDR, 0, 0, 0, 0, 0, 0 },
	{ 125, 0, 0, 0, 0, 0, 0 },
	{ 500, 0, 0, 0, 0, 0, 0 },
	{ 1000, 0, 0, 0, 0, 0, 0 }
};

#ifdef LI_CAN_SLV_ASYNC
#if defined(OUTER) || defined(OUTER_APP)
static const uint16_t can_config_async_ctrl_rx_objs[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES] =
{
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 1
	CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL1_RX,
#endif
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 2
	CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL2_RX,
#endif
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 3
	CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL3_RX,
#endif
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 4
	CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL4_RX,
#endif
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 5
	CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL5_RX,
#endif
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 6
	CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL6_RX,
#endif
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 7
	CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL7_RX,
#endif
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 8
	CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL8_RX,
#endif
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 9
#warning LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES out of range!
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES
}; /**< */
#endif // #if defined(OUTER) || defined(OUTER_APP)
#endif // #ifdef LI_CAN_SLV_ASYNC

/**
 * @brief function pointer to set module silent or awake
 * @param table_pos used table position in the configuration
 * @param type module type
 * @param module_nr used module number
 * @param module_silent_awake silent or awake
 */
void (*can_config_set_module_silent_awake_call)(uint16_t table_pos, char_t *type, li_can_slv_module_nr_t module_nr, can_config_module_silent_t module_silent_awake) = NULL;

#ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR
lcsa_module_change_voter_state_t (*module_nr_change_validator)(char_t *module_type, li_can_slv_module_nr_t new_module_nr, li_can_slv_module_nr_t actual_module_nr) = NULL;
#endif // #ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR

#ifdef SHOW_CAN_CONFIG
/*--------------------------------------------------------------------------*/
/* system variable CAN main                                                 */
/*--------------------------------------------------------------------------*/
static var_change_t can_config_var_change[] = /**< */
{
	{VAR_STATE_INIT,	"kBaud",	""},
	{VAR_STATE_INIT,	"kBaud",	""},
	{VAR_STATE_INIT,	"reg",		""},
	{VAR_STATE_INIT,	"reg",		""},
	{VAR_STATE_INIT,	"reg",		""},
	{VAR_STATE_INIT,	"reg",		""},
	{VAR_STATE_INIT,	"reg",		""},
	{VAR_STATE_INIT,	"reg",		""}
};

static const var_const_t can_config_var_const[] = /**< */
{
	{"can.bdr",			&can_config_bdr_current.baudrate,				VAR_UINT16,	0,	0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,		&can_config_var_change[0]},
	{"can.bdrdef",		&can_config_bdr_tab[0].baudrate,				VAR_UINT16,	0,	0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,		&can_config_var_change[1]},
	{"can.prescaler",	&can_config_bdr_current.baudrate_prescaler,		VAR_UINT8,	0,	0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,		&can_config_var_change[2]},
	{"can.resyncjw",	&can_config_bdr_current.resync_jump_width,		VAR_UINT8,	0,	0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,		&can_config_var_change[3]},
	{"can.prop_seg",	&can_config_bdr_current.propagation_segment,	VAR_UINT8,	0,	0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,		&can_config_var_change[4]},
	{"can.phasetseg_1",	&can_config_bdr_current.phase_time_segment_one,	VAR_UINT8,	0,	0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,		&can_config_var_change[5]},
	{"can.phasetseg_2",	&can_config_bdr_current.phase_time_segment_two,	VAR_UINT8,	0,	0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,		&can_config_var_change[6]},
	{"can.specific", 	&can_config_bdr_current.plattform_specific,		VAR_UINT8,	0,	0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,		&can_config_var_change[7]},
	{VAR_END_OF_TABLE}
};

#ifdef TESTSYS_SYSTEM
/*--------------------------------------------------------------------------*/
/* IO-TESTSYS-PAGE system CAN main                                          */
/*--------------------------------------------------------------------------*/
static const testsys_var_t can_config_testsys[] = /**< */
{
	{&can_config_var_const[0],	1,	1},
	{&can_config_var_const[1],	1,	2},

	{&can_config_var_const[2],	1,	4},
	{&can_config_var_const[3],	1,	5},
	{&can_config_var_const[4],	1,	6},
	{&can_config_var_const[5],	1,	7},
	{&can_config_var_const[6],	1,	8},
	{&can_config_var_const[7],	1,	9},
	{TESTSYS_VAR_END_OF_TABLE,	0,	0}
};
#endif // #ifdef SHOW_CAN_CONFIG
#endif // #ifdef TESTSYS_SYSTEM

#ifdef LI_CAN_SLV_UNKNOWN_MODULE
static li_can_slv_config_module_t can_config_ukwn_module =
{
	CAN_CONFIG_MODULE_TYPE_UNKNOWN,
#ifdef LI_CAN_SLV
	LI_CAN_SLV_CONFIG_DEF_MOD_NR1,
#else
	TUPLE_ID_INVALID, CAN_CONFIG_DEF_MODULE_NR1, CAN_CONFIG_DEF_MODULE_NR1, CAN_CONFIG_DEF_MODULE_NR1,
#endif // #ifdef LI_CAN_SLV
#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
	{ TUPLE_ID_INVALID, DLOAD_CRC_DEFAULT, DLOAD_DATE_DEFAULT, NULL},
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG
	LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
	LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
	{ LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0 },
	{ LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0 },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
#ifdef LI_CAN_SLV_MAIN_MON
	{ NULL, NULL, NULL, NULL },
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
	{ NULL, NULL, NULL, NULL},
	{ NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
	{ 0, 0, 0, 0 },
#ifdef LI_CAN_SLV_MAIN_MON
	{ 0, 0, 0, 0 },
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
	{ 0, 0, 0, 0},
	{ 0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MON
	LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
	LI_CAN_SLV_CONFIG_OBJ_USED_NOT,
	{ LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0 },
	{ LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0, LI_CAN_SLV_CONFIG_DLC_USED_0 },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
#ifdef LI_CAN_SLV_MAIN_MON
	{ NULL, NULL, NULL, NULL },
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
	{ NULL, NULL, NULL, NULL},
	{ NULL, NULL, NULL, NULL},
#endif // #ifdef LI_CAN_SLV_MON
};
#endif // #ifdef LI_CAN_SLV_UNKNOWN_MODULE

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#if defined(LI_CAN_SLV_BOOT) && defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR)
static li_can_slv_errorcode_t can_config_change_module(li_can_slv_module_nr_t module_nr);
#endif // #if defined(LI_CAN_SLV_BOOT) && defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR)

#if defined(OUTER) || defined(OUTER_APP)
static li_can_slv_errorcode_t can_config_set_module(uint16_t table_pos, const li_can_slv_config_module_t *module, li_can_slv_module_nr_t module_nr, void *rx0, void *rx1, void *rx2, void *rx3, void *tx0, void *tx1, void *tx2, void *tx3);

static li_can_slv_errorcode_t rx_main_dummy(byte_t *can, void *app, uint16_t dlc);	/**< dummy conversion routine main RX */
static li_can_slv_errorcode_t tx_main_dummy(byte_t *can, void *app, uint16_t dlc);	/**< dummy conversion routine main TX */

#if defined(LI_CAN_SLV_MON) || defined(LI_CAN_SLV_MAIN_MON)
static li_can_slv_errorcode_t tx_mon_dummy(byte_t *can, void *app, uint16_t dlc); /**< dummy conversion routine mon TX */
#endif // #if defined(LI_CAN_SLV_MON) || defined(LI_CAN_SLV_MAIN_MON)

#ifdef LI_CAN_SLV_MON
static li_can_slv_errorcode_t rx_mon_dummy(byte_t *can, void *app, uint16_t dlc); /**< dummy conversion routine mon TX */
#endif // #ifdef LI_CAN_SLV_MON

#ifdef LI_CAN_SLV_ASYNC
static li_can_slv_errorcode_t can_config_get_async_ctrl_rx_obj(uint16_t table_pos, uint16_t *msg_obj_nr);
#endif // #ifdef LI_CAN_SLV_ASYNC
#endif // #if defined(OUTER) || defined(OUTER_APP)

#ifndef LI_CAN_SLV_BOOT
static uint16_t get_table_pos_from_type_and_actual_module_number(char_t *module_type, li_can_slv_module_nr_t actual_module_nr);
#endif // #ifndef LI_CAN_SLV_BOOT

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief initialize can configuration
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_init(void)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t i;

	can_config_nr_of_modules = 0;
	for (i = 0; i < LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES; i++)
	{
		can_config_module_tab[i].type[0] = '\0';
	}

#ifdef SHOW_CAN_CONFIG
	/*----------------------------------------------------------------------*/
	/* add can config IO's                                                  */
	/*----------------------------------------------------------------------*/
	err = var_add(&can_config_var_const[0]);

	/*----------------------------------------------------------------------*/
	/* add can config IO's to test system                                   */
	/*----------------------------------------------------------------------*/
#ifdef TESTSYS_SYSTEM
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = testsys_system_page_add(&can_config_testsys[0], "CAN Config system");
	}
#endif // #ifdef TESTSYS_SYSTEM
#endif // #ifdef SHOW_CAN_CONFIG

	return (err);
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

/**
 * @brief Set CAN baud rate table from the tuple system
 * @remarks If no baud rate table tuple is defined the default baud rate table is used.
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_set_baudrate_table(void)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
#if defined(LI_CAN_SLV_TUPLE) && defined(CAN_BAUDRATE_TABLE_CNFG)
	uint16_t num_bytes;
	can_config_bdr_tab_t tmp_tab[CAN_CONFIG_SIZE_OF_BDR_TAB];
#endif // #if defined(LI_CAN_SLV_TUPLE) && defined(CAN_BAUDRATE_TABLE_CNFG)

#if defined(LI_CAN_SLV_TUPLE) && defined(CAN_BAUDRATE_TABLE_CNFG)
	err = tuple_read(TUPLE_ID_CAN_BDR_TABLE, (byte_t *) tmp_tab, sizeof(tmp_tab), &num_bytes, NULL, NULL);
	switch (err)
	{
		case LI_CAN_SLV_ERR_OK:
			/* use baud rate table defined by tuple */
			can_port_memory_cpy(&can_config_bdr_tab, &tmp_tab, sizeof(tmp_tab));
			break;

		case ERR_IO_TPL_NOT_FOUND:
		default:
			/* use default baud rate table */
			can_port_memory_cpy(&can_config_bdr_tab, &can_main_hw_default_baudrate_table, sizeof(can_main_hw_default_baudrate_table));
			err = LI_CAN_SLV_ERR_OK; // reset err to LI_CAN_SLV_ERR_OK
			break;
	}
#else // #if defined(LI_CAN_SLV_TUPLE) && defined(CAN_BAUDRATE_TABLE_CNFG)
	/* use default baud rate table */
	can_port_memory_cpy(&can_config_bdr_tab, &can_main_hw_default_baudrate_table, sizeof(can_main_hw_default_baudrate_table));
#endif // #if defined(LI_CAN_SLV_TUPLE) && defined(CAN_BAUDRATE_TABLE_CNFG)
	return (err);
}

/**
 * @param baudrate is the  baud rate to set in kBaud
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_set_baudrate(can_config_bdr_t baudrate)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t i;
	for (i = 0; i < CAN_CONFIG_SIZE_OF_BDR_TAB; i++)
	{
		if (baudrate == can_config_bdr_tab[i].baudrate)
		{
			can_port_memory_cpy(&can_config_bdr_current, &can_config_bdr_tab[i], sizeof(can_config_bdr_current));
			err = can_main_hw_set_baudrate(&can_config_bdr_tab[i]);
#if defined(LI_CAN_SLV_MON) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY)
			if(err == LI_CAN_SLV_ERR_OK)
			{			
				err = can_mon_hw_set_baudrate(&can_config_bdr_tab[i]);
			}
#endif // #if defined(LI_CAN_SLV_MON) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY)
			if(err == LI_CAN_SLV_ERR_OK)
			{
#ifdef LI_CAN_SLV_NO_XLOAD_INFO
				can_config_set_baudrate_startup(baudrate);
#else // #ifdef LI_CAN_SLV_NO_XLOAD_INFO
				li_can_slv_xload_info_set_can_baudrate(baudrate);
#endif // #ifdef LI_CAN_SLV_NO_XLOAD_INFO
			}
		}
	}
	return err; //(ERR_MSG_CAN_CONFIG_SET_INVALID_BAUDRATE);
}

/**
 * @brief set default CAN baud rate defined by baud rate-table
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_set_baudrate_default(void)
{
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT
	LI_CAN_SLV_DEBUG_PRINT("bdr def\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT
	return (lcsa_set_baudrate(can_config_bdr_tab[0].baudrate));
}

/**
 * @brief get current configured CAN baud rate
 * @param baudrate defines the configured baud rate in kBaud
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_get_baudrate(can_config_bdr_t *baudrate)
{
	*baudrate = can_config_bdr_current.baudrate;
	return (LI_CAN_SLV_ERR_OK);
}

/**
 *
 */
uint8_t can_config_is_baudrate_valid(can_config_bdr_t baudrate)
{
	uint16_t i;

	for (i = 0; i < CAN_CONFIG_SIZE_OF_BDR_TAB; i++)
	{
		if (baudrate == can_config_bdr_tab[i].baudrate)
		{
			return TRUE;
		}
	}
	return FALSE;
}

/**
 *
 */
void can_config_save_baudrate_startup(li_can_slv_config_bdr_t baud_rate)
{
	can_config_bdr_startup = baud_rate;
}

/**
 *
 */
li_can_slv_config_bdr_t can_config_get_baudrate_startup(void)
{
	return can_config_bdr_startup;
}

#ifdef LI_CAN_SLV_BOOT
/**
 * @brief add module to can configuration
 * @remarks Add a logical module. In case of multiple usage of a module number
 * the default module number is set and error is returned.
 * @param module_type module type
 * @param module_nr is the module number of the configured module
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_add_module(char_t *module_type, li_can_slv_module_nr_t module_nr)
{
	li_can_slv_errorcode_t err;
	uint16_t can_id_or, can_id_and, mask;

	can_port_memory_cpy(&can_config_module_tab.type, module_type, CAN_CONFIG_TYPE_STRING_LENGTH);

	can_config_module_tab.module_nr = module_nr;

	can_id_or = 0x01FC & ((CAN_CONFIG_MODULE_NR_BROADCAST - 1) << 2);
	can_id_and = 0x01FC & ((CAN_CONFIG_MODULE_NR_BROADCAST - 1) << 2);

	can_id_or |= (0x01FC & ((can_config_module_tab.module_nr - 1) << 2));
	can_id_and &= (0x01FC & ((can_config_module_tab.module_nr - 1) << 2));

	mask = 0x01FC & (can_id_and | ~can_id_or);

	err = can_main_define_msg_obj(CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL1_RX, CAN_CONFIG_ASYNC_CTRL_RX_SLAVE_ID, CAN_CONFIG_ACCEPTANCE_MASK | mask, CAN_CONFIG_ASYNC_CTRL_RX_DLC, CAN_CONFIG_DIR_RX, CAN_MAIN_SERVICE_ID_RX, CAN_OBJECT_NOT_SYNC);

	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_main_define_msg_obj(CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS1, CAN_CONFIG_SYS_MSG_ID, CAN_CONFIG_ACCEPTANCE_MASK | mask, CAN_CONFIG_SYS_MSG_DLC, CAN_CONFIG_DIR_RX, CAN_MAIN_SERVICE_ID_RX, CAN_OBJECT_NOT_SYNC);
	}

	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_config_set_module_silent_awake(can_config_module_silent);
	}

	return (err);
}
#else // #ifdef LI_CAN_SLV_BOOT
/**
 * @brief add module to can configuration
 * @remark Add a logical module with the can_config_add_module function. Multiple configuration of a module
 * type as a logical module is not allowed. In case of multiple usage of a module number the default module
 * number is set and a according error is returned.
 * @param module is the pointer to application module type configuration
 * @param module_nr is the module number of the configured module
 * @param rx0 data pointer to application data of CAN receive object 0
 * @param rx1 data pointer to application data of CAN receive object 1
 * @param rx2 data pointer to application data of CAN receive object 2
 * @param rx3 data pointer to application data of CAN receive object 3
 * @param tx0 data pointer to application data of CAN transmit object 0
 * @param tx1 data pointer to application data of CAN transmit object 1
 * @param tx2 data pointer to application data of CAN transmit object 2
 * @param tx3 data pointer to application data of CAN transmit object 3
 * @return #LI_CAN_SLV_ERR_OK, #ERR_MSG_CAN_CONFIG_REDECLARED_MODULE_NR, #ERR_MSG_CAN_CONFIG_MAXIMUM_NR_OF_MODULE_TYPE
 */
li_can_slv_errorcode_t can_config_add_module(const li_can_slv_config_module_t *module, li_can_slv_module_nr_t module_nr,
        void *rx0, void *rx1, void *rx2, void *rx3, void *tx0, void *tx1, void *tx2, void *tx3)
{
	/**@todo fix error handling */
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t i, j;

#ifndef LI_CAN_SLV_SYS_MODULE_ERROR
	err = err;
#endif // #ifndef LI_CAN_SLV_SYS_MODULE_ERROR

#ifdef LI_CAN_SLV_DEBUG_CAN_INIT_MOD
	LI_CAN_SLV_DEBUG_PRINT("add mod, type:%4s\n", module->type);
#endif // #ifdef #ifdef LI_CAN_SLV_DEBUG_CAN_INIT_MOD

	i = 0;
	while (i < LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES)
	{
		if (can_config_module_tab[i].type[0] == '\0')
		{
			/* check if module number not already used */
			j = 0;
			while (j < i)
			{
				if (can_config_module_tab[j].module_nr == module_nr)
				{
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT
					LI_CAN_SLV_DEBUG_PRINT("mdnr:%d\n", module_nr);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT

					/* set module with default module number */
					err = can_config_set_module(i, module, LI_CAN_SLV_CONFIG_DEF_MOD_NR1 + i, rx0, rx1, rx2, rx3, tx0, tx1, tx2, tx3);

#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
					if (err != LI_CAN_SLV_ERR_OK)
					{
						error_syserr_send(err, ERR_LVL_INFO, can_config_get_module_nr_main(), ERR_LVL_INFO);
					}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
					return (ERR_MSG_CAN_CONFIG_REDECLARED_MODULE_NR);
				}
				j++;
			}

#ifdef LI_CAN_SLV_DEBUG_CAN_INIT
			LI_CAN_SLV_DEBUG_PRINT("used modnr: %d\n", module_nr);
			LI_CAN_SLV_DEBUG_PRINT("mdnr:%d, %08x\n", module_nr, err);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT

			/* set module with module number */
			err = can_config_set_module(i, module, module_nr, rx0, rx1, rx2, rx3, tx0, tx1, tx2, tx3);

#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
			if (err != LI_CAN_SLV_ERR_OK)
			{
				error_syserr_send(err, ERR_LVL_INFO, can_config_get_module_nr_main(), ERR_LVL_INFO);
			}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
			return (LI_CAN_SLV_ERR_OK);
		}
		i++;
	}
	return (ERR_MSG_CAN_CONFIG_MAXIMUM_NR_OF_MODULE_TYPE);
}
#endif // #ifdef LI_CAN_SLV_BOOT

#ifdef LI_CAN_SLV_UNKNOWN_MODULE
/**
 * @param module_type type of the module
 * @param module_nr module number
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_add_ukwn_module(li_can_slv_module_type_t *module_type, li_can_slv_module_nr_t module_nr)
{
	if ((module_nr > CAN_CONFIG_MAX_MODULE_NR) || (module_nr < CAN_CONFIG_MIN_MODULE_NR))
	{
		module_nr = LI_CAN_SLV_CONFIG_DEF_MOD_NR1;
	}

	can_config_ukwn_module.module_nr = module_nr;

	if (NULL != module_type)
	{
		li_can_slv_port_string_n_cpy(can_config_ukwn_module.type, module_type, CAN_CONFIG_TYPE_STRING_LENGTH);
	}

	return can_config_add_module(&can_config_ukwn_module, module_nr, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}
#endif // #ifdef LI_CAN_SLV_UNKNOWN_MODULE

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief get the can module configuration table by type
 * @param module module configuration table
 * @param type module type
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_get_module(li_can_slv_config_module_t **module, char_t *type)
{
	li_can_slv_errorcode_t err;
	uint16_t table_pos;

	err = can_config_module_type_valid(type, &table_pos);

	if (err == LI_CAN_SLV_ERR_OK)
	{
		*module = &can_config_module_tab[table_pos];
	}
	else
	{
		err = ERR_MSG_CAN_CONFIG_MODULE_TYPE_NOT_FOUND;
	}

	return err;
}

/**
 *
 */
li_can_slv_errorcode_t can_config_set_critical(char_t *type)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t table_pos;
#ifdef CLEAR_CRITICAL_OBJ
	uint16_t msg_obj, i;
#endif // #ifdef CLEAR_CRITICAL_OBJ
	err = can_config_module_type_valid(type, &table_pos);
	if (err != LI_CAN_SLV_ERR_OK)
	{
		return (err);
	}

	/**
	 * @todo ciritical muss hier nicht ein sync (main mon) statfinden
	 */
	/*----------------------------------------------------------------------*/
	/* switch of synchronous process                                        */
	/*----------------------------------------------------------------------*/
	can_config_sync_off(table_pos);

#ifdef CLEAR_CRITICAL_OBJ
	/*----------------------------------------------------------------------*/
	/* clear CAN message object for receiving synchronous data from         */
	/* main CAN-controller on monitor CAN-controller                        */
	/*----------------------------------------------------------------------*/
	for (i = 0; i < can_config_module_tab[table_pos].tx_obj; i++)
	{
		msg_obj = can_config_module_tab[table_pos].tx_msg_obj_mon[i];
		err = can_msg_obj_init(msg_obj);
		if (err != LI_CAN_SLV_ERR_OK)
		{
			return (err);
		}
	}

	/*----------------------------------------------------------------------*/
	/* clear CAN message object for receiving synchronous data from master  */
	/* on main & monitor CAN-controller                                     */
	/*----------------------------------------------------------------------*/
	for (i = 0; i < can_config_module_tab[table_pos].rx_obj; i++)
	{
		msg_obj = can_config_module_tab[table_pos].rx_msg_obj_main[i];
		err = can_msg_obj_init(msg_obj);
		if (err != LI_CAN_SLV_ERR_OK)
		{
			return (err);
		}

		msg_obj = can_config_module_tab[table_pos].rx_msg_obj_mon[i];
		err = can_msg_obj_init(msg_obj);
		if (err != LI_CAN_SLV_ERR_OK)
		{
			return (err);
		}
	}
#endif // #ifdef CLEAR_CRITICAL_OBJ

	return (err);
}
#endif //#if defined(OUTER) || defined(OUTER_APP)

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief switch off synchronous process of defined module
 * @param table_pos position in configuration table
 */
void can_config_sync_off(uint16_t table_pos)
{
	uint16_t i;
	byte_t clear_data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	/*----------------------------------------------------------------------*/
	/* synchronous transmit                                                 */
	/*----------------------------------------------------------------------*/
	can_config_module_tab[table_pos].tx_obj_sync = LI_CAN_SLV_CONFIG_OBJ_USED_NOT;
	for (i = 0; i < can_config_module_tab[table_pos].tx_obj; i++)
	{
		can_main_synchron_tx_data_off(table_pos, i);
		can_config_module_tab[table_pos].tx_dlc_sync[i] = LI_CAN_SLV_CONFIG_DLC_USED_0;
		can_config_module_tab[table_pos].tx_main_sync[i] = &tx_main_dummy;
#ifdef LI_CAN_SLV_MAIN_MON
		can_config_module_tab[table_pos].tx_main_rx_mon_sync[i] = &tx_mon_dummy;
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		can_config_module_tab[table_pos].tx_mon_sync[i] = &tx_mon_dummy;
#endif // #ifdef LI_CAN_SLV_MON
	}

	/*----------------------------------------------------------------------*/
	/* synchronous receive                                                  */
	/*----------------------------------------------------------------------*/
	can_config_module_tab[table_pos].rx_obj_sync = LI_CAN_SLV_CONFIG_OBJ_USED_NOT;
	for (i = 0; i < can_config_module_tab[table_pos].rx_obj; i++)
	{
		can_config_module_tab[table_pos].rx_dlc_sync[i] = LI_CAN_SLV_CONFIG_DLC_USED_0;
		can_config_module_tab[table_pos].rx_main_sync[i] = &rx_main_dummy;
#ifdef LI_CAN_SLV_MON
		can_config_module_tab[table_pos].rx_mon_sync[i] = &rx_mon_dummy;
#endif // #ifdef LI_CAN_SLV_MON
	}

	/*----------------------------------------------------------------------*/
	/* clear synchronous data by conversion code                            */
	/*----------------------------------------------------------------------*/
	for (i = 0; i < can_config_module_tab[table_pos].rx_obj; i++)
	{
		can_config_module_tab[table_pos].rx_main[i](&clear_data[0], can_config_module_tab[table_pos].rx[i], can_config_module_tab[table_pos].rx_dlc[i]);
	}
}

/**
 * @brief switch on synchronous process of defined module
 * @param table_pos position in configuration table
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_sync_on(uint16_t table_pos)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t i;

	/*----------------------------------------------------------------------*/
	/* synchronous transmit                                                    */
	/*----------------------------------------------------------------------*/
	can_config_module_tab[table_pos].tx_obj_sync = can_config_module_tab[table_pos].tx_obj;
	for (i = 0; i < can_config_module_tab[table_pos].tx_obj; i++)
	{
		can_main_synchron_tx_data_on(table_pos, i);
		can_config_module_tab[table_pos].tx_dlc_sync[i] = can_config_module_tab[table_pos].tx_dlc[i];
		can_config_module_tab[table_pos].tx_main_sync[i] = can_config_module_tab[table_pos].tx_main[i];
#ifdef LI_CAN_SLV_MAIN_MON
		can_config_module_tab[table_pos].tx_main_rx_mon_sync[i] = can_config_module_tab[table_pos].tx_main_rx_mon[i];
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
		can_config_module_tab[table_pos].tx_mon_sync[i] = can_config_module_tab[table_pos].tx_mon[i];
#endif // #ifdef LI_CAN_SLV_MON
	}

	/*----------------------------------------------------------------------*/
	/* synchronous receive                                                     */
	/*----------------------------------------------------------------------*/
	can_config_module_tab[table_pos].rx_obj_sync = can_config_module_tab[table_pos].rx_obj;
	for (i = 0; i < can_config_module_tab[table_pos].rx_obj; i++)
	{
		can_config_module_tab[table_pos].rx_dlc_sync[i] = can_config_module_tab[table_pos].rx_dlc[i];
		can_config_module_tab[table_pos].rx_main_sync[i] = can_config_module_tab[table_pos].rx_main[i];
#ifdef LI_CAN_SLV_MON
		can_config_module_tab[table_pos].rx_mon_sync[i] = can_config_module_tab[table_pos].rx_mon[i];
#endif // #ifdef LI_CAN_SLV_MON
	}

	return (err);
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief set defined module as critical (no synchronous process)
 * @param type is the module type
 * @param crc is the crc-code
 * @param date is the current date
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_set_ioconfig(char_t *type, crc16_t crc, uint32_t date)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t table_pos;

	err = can_config_module_type_valid(type, &table_pos);
	if (err != LI_CAN_SLV_ERR_OK)
	{
		return (err);
	}

	can_config_module_tab[table_pos].ioconfig.crc = crc;
	can_config_module_tab[table_pos].ioconfig.date = date;

	return (err);
}

/**
 * @brief get the can module configuration table by type
 * @param module module configuration table
 * @param type module type
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_call_ioconfig_set(char_t *type)
{
	li_can_slv_errorcode_t err;
	uint16_t table_pos;

	err = can_config_module_type_valid(type, &table_pos);

	if (err == LI_CAN_SLV_ERR_OK)
	{
		if (can_config_module_tab[table_pos].ioconfig.set != NULL)
		{
			err = can_config_module_tab[table_pos].ioconfig.set();
		}
	}
	else
	{
		err = ERR_MSG_CAN_CONFIG_MODULE_TYPE_NOT_FOUND;
	}

	return err;
}
#endif // #if defined(OUTER) || defined(OUTER_APP)
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG

#if defined(LI_CAN_SLV_BOOT) && defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR)
/**
 * @brief change module number of configured module
 * @remarks Changes the module number of a configured logical module with defined module type.
 * Any module type appears only one time. Multiple usage of a module type is not allowed. The
 * arbitration of all can message objects which are	used by the defined module will be also changed.\n
 * @param module_nr
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_change_module_nr(li_can_slv_module_nr_t module_nr)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
#ifdef LI_CAN_SLV_TUPLE
	uint32_t timestamp = 0L;
#endif // #ifdef LI_CAN_SLV_TUPLE

	/* change module arbitration to new module number */
	err = can_config_change_module(module_nr);

#ifdef LI_CAN_SLV_TUPLE
	if (err == LI_CAN_SLV_ERR_OK)
	{
#ifdef LI_CAN_SLV_BYTE_ORDER_BIG_ENDIAN
		module_nr = can_port_swap_uint16(module_nr);
#endif // #ifdef LI_CAN_SLV_BYTE_ORDER_BIG_ENDIAN
		/* module number of 1. defined module in the CAN configuration module table */
		err = tuple_write(TUPLE_ID_MODULE_NR1, (byte_t *)&module_nr, sizeof(li_can_slv_module_nr_t), timestamp);
	}
#endif // #ifdef LI_CAN_SLV_TUPLE

#ifdef LI_CAN_SLV_TUPLE
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = tuple_switch_chain();
	}
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = tuple_sync_chains();
	}
#endif // #ifdef LI_CAN_SLV_TUPLE
	return (err);
}
#endif	// #if defined(LI_CAN_SLV_BOOT) && defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR)

#if (defined(OUTER) || defined(OUTER_APP))
#ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR
/**
 *
 */
li_can_slv_errorcode_t can_config_change_module_nr(uint16_t table_pos, li_can_slv_module_nr_t new_module_nr)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	lcsa_module_change_voter_state_t state = LCSA_MODULE_CHANGE_ERROR;

	if (NULL != module_nr_change_validator)
	{
		state = module_nr_change_validator(can_config_module_tab[table_pos].type, new_module_nr, can_config_module_tab[table_pos].module_nr);

		switch (state)
		{
			case LCSA_MODULE_CHANGE_VALID:
			{
				err = li_can_slv_config_change_module_nr_and_identifiers(can_config_module_tab[table_pos].type, new_module_nr, can_config_module_tab[table_pos].module_nr);

				if (LI_CAN_SLV_ERR_OK == err)
				{
					err = can_config_set_module_silent_awake(table_pos, LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE);
				}
				break;
			}
			case LCSA_MODULE_CHANGE_VALID_BUT_SET_SILENT:
			{
				err = li_can_slv_config_change_module_nr_and_identifiers(can_config_module_tab[table_pos].type, new_module_nr, can_config_module_tab[table_pos].module_nr);

				if (LI_CAN_SLV_ERR_OK == err)
				{
					err = can_config_set_module_silent_awake(table_pos, LI_CAN_SLV_CONFIG_MODULE_STATE_SILENT);
				}
				break;
			}
			case LCSA_MODULE_CHANGE_FORBIDDEN:
			case LCSA_MODULE_CHANGE_ERROR:
				err = ERR_MSG_CAN_CHANGING_TO_INVALID_MODULE_NR;
				break;

			default:
				err = ERR_MSG_CAN_CHANGING_TO_INVALID_MODULE_NR;
		}
	}
	else
	{
		err = li_can_slv_config_change_module_nr_and_identifiers(can_config_module_tab[table_pos].type, new_module_nr, can_config_module_tab[table_pos].module_nr);
	}

	return (err);
}
#endif // #ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR
#endif // #if (defined(OUTER) || defined(OUTER_APP)) && defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR)

#ifdef LI_CAN_SLV_BOOT
/**
 * @brief find the given module number in the can configuration table
 * @param module_nr module number
 * @param table_pos position of module number in the configuration table
 * @param module_silent state of module (SILENT, AWAKE)
 * @param module_found 0 if module is found in the table, otherwise 1
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_module_nr_valid(li_can_slv_module_nr_t module_nr, can_config_module_silent_t *module_silent, uint16_t *module_found)
{
	if (can_config_module_tab.module_nr == module_nr)
	{
		*module_found = 1;
	}
	else
	{
		*module_found = 0;
	}
	*module_silent = can_config_module_silent;
	return LI_CAN_SLV_ERR_OK;
}
#else // #ifdef LI_CAN_SLV_BOOT
/**
 * @brief find the given module number in the can configuration table
 * @param module_nr: module number
 * @param table_pos position of module number in the configuration table
 * @param module_silent state of module (SILENT, AWAKE)
 * @param module_found 0 if module is found in the table, otherwise 1
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_module_nr_valid(li_can_slv_module_nr_t module_nr, uint16_t *table_pos, can_config_module_silent_t *module_silent, uint16_t *module_found)
{
	*module_found = 0;
	for (*table_pos = 0; (*table_pos < LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES) && !(*module_found); (*table_pos)++)
	{
		if (can_config_module_tab[*table_pos].module_nr == module_nr)
		{
			*module_found = 1;
		}
	}
	(*table_pos)--;
	*module_silent = can_config_module_silent[*table_pos];
	return LI_CAN_SLV_ERR_OK;
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief get the type of the module at given position
 * @param table_pos: position in configuration table
 * @param module_type:  type of module at the given position
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_get_module_type(uint16_t table_pos, char_t *module_type)
{
	if (table_pos < LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES)
	{
		can_port_memory_cpy(module_type, can_config_module_tab[table_pos].type, CAN_CONFIG_TYPE_STRING_LENGTH);
	}
	else
	{
		return ERR_MSG_CAN_CONFIG_MODULE_WRONG_TABLE_POS;
	}
	return LI_CAN_SLV_ERR_OK;
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

#ifdef LI_CAN_SLV_BOOT
/**
 * @brief get the number of the module at given position
 * @param module_nr number of module at the given position
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_module_nr_t can_config_get_module_nr(void)
{
	return can_config_module_tab.module_nr;
}
#endif // #ifdef LI_CAN_SLV_BOOT

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief get the number of the module at given position
 * @param table_pos position in configuration table
 * @param module_nr number of module at the given table position
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_get_module_nr(uint16_t table_pos, li_can_slv_module_nr_t *module_nr)
{
	if (table_pos < LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES)
	{
		*module_nr = can_config_module_tab[table_pos].module_nr;
	}
	else
	{
		return ERR_MSG_CAN_CONFIG_MODULE_WRONG_TABLE_POS;
	}
	return LI_CAN_SLV_ERR_OK;
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

#if defined(OUTER) || defined(OUTER_APP)
li_can_slv_module_nr_t can_config_get_module_nr_main(void)
{
	return (can_config_module_tab[LI_CAN_SLV_CONFIG_TABLE_POS_MAIN_MODULE].module_nr);
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief prepare the number of the module's main logical module if available
 * @param module_nr
 */
void can_config_init_module_nr_main(li_can_slv_module_nr_t module_nr)
{

	if ((module_nr <= CAN_CONFIG_MAX_MODULE_NR) && (module_nr >= CAN_CONFIG_MIN_MODULE_NR))
	{
		can_config_module_tab[LI_CAN_SLV_CONFIG_TABLE_POS_MAIN_MODULE].module_nr = module_nr;
	}
	else
	{
		can_config_module_tab[LI_CAN_SLV_CONFIG_TABLE_POS_MAIN_MODULE].module_nr = LI_CAN_SLV_CONFIG_DEF_MOD_NR1;
	}
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT
	LI_CAN_SLV_DEBUG_PRINT("mdnr: %d\n", module_nr);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief get the io_config_id of the module at given position
 * @param table_pos position in configuration table
 * @param ioconfig io_config_id of module at the given position
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_get_ioconfig(uint16_t table_pos, can_config_ioconfig_t **ioconfig)
{
	if (table_pos < LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES)
	{
		*ioconfig = &can_config_module_tab[table_pos].ioconfig;
	}
	else
	{
		*ioconfig = NULL;
		return ERR_MSG_CAN_CONFIG_MODULE_WRONG_TABLE_POS;
	}
	return LI_CAN_SLV_ERR_OK;
}
#endif // #if defined(OUTER) || defined(OUTER_APP)
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG

#ifdef LI_CAN_SLV_BOOT
/**
 * @brief get the (silent) state of the module at given position
 * @param module_silent_awake  state of module (SILENT, AWAKE) at the given position
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_get_module_silent_awake(can_config_module_silent_t *module_silent_awake)
{
	*module_silent_awake = can_config_module_silent;
	return LI_CAN_SLV_ERR_OK;
}
#endif // #ifdef LI_CAN_SLV_BOOT

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief get the (silent) state of the module at given position
 * @param table_pos position in configuration table
 * @param module_silent_awake state of module (SILENT, AWAKE) at the given position
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_get_module_silent_awake(uint16_t table_pos, can_config_module_silent_t *module_silent_awake)
{
	if (table_pos < LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES)
	{
		*module_silent_awake = can_config_module_silent[table_pos];
	}
	else
	{
		return ERR_MSG_CAN_CONFIG_MODULE_WRONG_TABLE_POS;
	}
	return LI_CAN_SLV_ERR_OK;
}
#endif	// #if defined(OUTER) || defined(OUTER_APP)

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief can_config_get_nr_of_modules
 * @param nr_of_modules number of defined modules
 */
void can_config_get_nr_of_modules(uint16_t *nr_of_modules)
{
	*nr_of_modules = can_config_nr_of_modules;
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

#if defined(OUTER) || defined(OUTER_APP)
li_can_slv_errorcode_t can_config_module_type_valid(char_t *type, uint16_t *table_pos)
{
	uint16_t i;

	for (i = 0; i < LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES; i++)
	{
		/* check if module type is found */
		if (!can_port_string_cmp(&can_config_module_tab[i].type[0], type))
		{
			*table_pos = i;
			return (LI_CAN_SLV_ERR_OK);
		}
	}
	*table_pos = 0;
	return (ERR_MSG_CAN_CONFIG_MODULE_TYPE_NOT_FOUND);
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief can_config_module_valid
 * @param[in] type module type of module
 * @param[in] module_nr module number of module
 * @param[out] table_pos table position of the module
 * @return #LI_CAN_SLV_ERR_OK or #ERR_MSG_CAN_CONFIG_MODULE_NOT_FOUND
 */
li_can_slv_errorcode_t can_config_module_valid(char_t *type, li_can_slv_module_nr_t module_nr, uint16_t *table_pos)
{
	uint16_t tmp;

	tmp = get_table_pos_from_type_and_actual_module_number(type, module_nr);

	if (tmp < LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES)
	{
		*table_pos = tmp;
		return (LI_CAN_SLV_ERR_OK);
	}
	else
	{
		*table_pos = 0;
		return (ERR_MSG_CAN_CONFIG_MODULE_NOT_FOUND);
	}
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

#ifdef LI_CAN_SLV_BOOT
/**
 * @brief set the module at given table position to silent or awake mode (state)
 * @param module_silent_awake silent or awake state
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_set_module_silent_awake(can_config_module_silent_t module_silent_awake)
{
	can_config_module_silent = module_silent_awake;
	return LI_CAN_SLV_ERR_OK;
}
#else // #ifndef LI_CAN_SLV_BOOT
/**
 * @brief set the module at given table position to silent or awake mode (state)
 * @param table_pos position in configuration table
 * @param module_silent_awake silent or awake state
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_set_module_silent_awake(uint16_t table_pos, can_config_module_silent_t module_silent_awake)
{
	if (table_pos < LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES)
	{
		can_config_module_silent[table_pos] = module_silent_awake;
		if (can_config_set_module_silent_awake_call != NULL)
		{
			can_config_set_module_silent_awake_call(table_pos, can_config_module_tab[table_pos].type, can_config_module_tab[table_pos].module_nr, can_config_module_silent[table_pos]);
		}

		switch (can_config_module_silent[table_pos])
		{
			case LI_CAN_SLV_CONFIG_MODULE_STATE_SILENT:
				can_config_sync_off(table_pos);
				break;

			case LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE:
				can_config_sync_on(table_pos);
				break;

			default:
				return ERR_MSG_CAN_CONFIG_INVALID_SILENT_AWAKE_STATE;
				break;
		}
	}
	else
	{
		return ERR_MSG_CAN_CONFIG_MODULE_WRONG_TABLE_POS;
	}
	return LI_CAN_SLV_ERR_OK;
}
#endif // #ifndef LI_CAN_SLV_BOOT

#ifndef LI_CAN_SLV_BOOT
/**
 *
 * @param module_type
 * @param module_nr
 * @param desired_state
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_config_set_module_silent_awake_from_type_and_nr(char_t *module_type, li_can_slv_module_nr_t module_nr, can_config_module_silent_t desired_state)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t table_pos = 0;

	table_pos = get_table_pos_from_type_and_actual_module_number(module_type, module_nr);

	if (table_pos < LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES)
	{
		err = can_config_set_module_silent_awake(table_pos, desired_state);
	}
	else
	{
		err = ERR_MSG_CAN_CONFIG_MODULE_NOT_FOUND;
	}

	return (err);
}
#endif // #ifndef LI_CAN_SLV_BOOT

#if defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR) && !defined(LI_CAN_SLV_BOOT)
/**
 * @brief returns the actual module number of the given logical module type
 * @param modnr module number of the given logical module type
 * @param type logical module type
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_get_module_nr_by_type(li_can_slv_module_nr_t *modnr, char_t *type)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t table_pos;

	err = can_config_module_type_valid(type, &table_pos);
	if (err != LI_CAN_SLV_ERR_OK)
	{
		*modnr = LI_CAN_SLV_CONFIG_DEF_MOD_NR1; //return an invalid module number as result
		return (err);
	}

	*modnr = can_config_module_tab[table_pos].module_nr;

	return (LI_CAN_SLV_ERR_OK);
}
#endif // #if defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR) && !defined(LI_CAN_SLV_BOOT)

#if defined(OUTER) || defined(OUTER_APP)
#ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR
/**
 *
 * @param module_type
 * @param new_module_nr
 * @param actual_module_nr
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_config_change_module_nr_and_identifiers(char_t *module_type, li_can_slv_module_nr_t new_module_nr, li_can_slv_module_nr_t actual_module_nr)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t i;
	uint16_t can_id, msg_obj;
	uint16_t can_id_or, can_id_and, mask;
	uint16_t table_pos = 0;

#ifdef LI_CAN_SLV_DEBUG_CAN_INIT
	LI_CAN_SLV_DEBUG_PRINT("table_pos: %d, modnr: %d\n", table_pos, new_module_nr);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT

	table_pos = get_table_pos_from_type_and_actual_module_number(module_type, actual_module_nr);

	can_config_module_tab[table_pos].module_nr = new_module_nr;

	/* define CAN message object for transmitting synchronous data from main CAN Node to master               */
	/* define CAN message object for receiving synchronous data from main CAN Node on monitor CAN Node        */
	/* define CAN message object for receiving synchronous data from main CPU on monitor CPU on main CAN Node */
	/* the can_id is always the same for this three use cases                                                 */
	can_id = CAN_CONFIG_DATA_TX_MASK + ((new_module_nr - 1) << 2);

#ifdef LI_CAN_SLV_MAIN_MON
	if (can_mainmon_type == CAN_MAINMON_TYPE_MAIN)
	{
#endif // #ifdef LI_CAN_SLV_MAIN_MON

		/* define CAN message object for transmitting synchronous data from main CAN Node to master */
		for (i = 0; i < can_config_module_tab[table_pos].tx_obj; i++)
		{
			/* calculation of the new transmit CAN identifier */
			err = can_main_sync_process_tx_data_cnfg(table_pos, i, new_module_nr);
			if (err != LI_CAN_SLV_ERR_OK)
			{
				return (err);
			}

#if defined(LI_CAN_SLV_MON)
			/* define CAN message object for receiving synchronous data from main CAN Node on monitor CAN Node on the Main CPU */
			msg_obj = can_config_module_tab[table_pos].tx_msg_obj_mon[i];
			err = can_mon_define_msg_obj(msg_obj, can_id + i, CAN_CONFIG_ACCEPTANCE_ONE_ID, can_config_module_tab[table_pos].tx_dlc[i], CAN_CONFIG_DIR_RX, CAN_MON_ISR_ID_RX_MAIN, CAN_OBJECT_IS_SYNC);
			if (err != LI_CAN_SLV_ERR_OK)
			{
				return (err);
			}
#endif // #if defined(LI_CAN_SLV_MON)
		}
#ifdef LI_CAN_SLV_MAIN_MON
	}
#endif // #ifdef LI_CAN_SLV_MAIN_MON

#ifdef LI_CAN_SLV_MAIN_MON
	if (can_mainmon_type == CAN_MAINMON_TYPE_MON)
	{
		/* define CAN message object for receiving synchronous data from main CPU on monitor CPU on main CAN Node */
		for (i = 0; i < can_config_module_tab[table_pos].tx_obj; i++)
		{

			/* set used message object */
			msg_obj = can_config_module_tab[table_pos].tx_main_rx_msg_obj_mon[i];
			err = can_main_define_msg_obj(msg_obj, can_id + i, CAN_CONFIG_ACCEPTANCE_ONE_ID, can_config_module_tab[table_pos].tx_dlc[i], CAN_CONFIG_DIR_RX, CAN_MAIN_SERVICE_ID_RX, CAN_OBJECT_IS_SYNC);
			if (err != LI_CAN_SLV_ERR_OK)
			{
				return (err);
			}
		}
	}
#endif // #ifdef LI_CAN_SLV_MAIN_MON

	/* define CAN message object for receiving synchronous data from master on main & monitor CAN node */
	can_id = CAN_CONFIG_DATA_RX_MASK + ((new_module_nr - 1u) << 2u);

	for (i = 0; i < can_config_module_tab[table_pos].rx_obj; i++)
	{
		msg_obj = can_config_module_tab[table_pos].rx_msg_obj_main[i];
		err = can_main_define_msg_obj(msg_obj, can_id + i, CAN_CONFIG_ACCEPTANCE_ONE_ID, can_config_module_tab[table_pos].rx_dlc[i], CAN_CONFIG_DIR_RX, CAN_MAIN_SERVICE_ID_RX, CAN_OBJECT_IS_SYNC);
		if (err != LI_CAN_SLV_ERR_OK)
		{
			return (err);
		}

#ifdef LI_CAN_SLV_MON
		/* monitor CAN node */
		msg_obj = can_config_module_tab[table_pos].rx_msg_obj_mon[i];
		err = can_mon_define_msg_obj(msg_obj, can_id + i, CAN_CONFIG_ACCEPTANCE_ONE_ID, can_config_module_tab[table_pos].rx_dlc[i], CAN_CONFIG_DIR_RX, CAN_MON_ISR_ID_RX, CAN_OBJECT_IS_SYNC);
		if (err != LI_CAN_SLV_ERR_OK)
		{
			return (err);
		}
#endif // #ifdef LI_CAN_SLV_MON
	}

	can_id_or = 0x01FCu & ((CAN_CONFIG_MODULE_NR_BROADCAST - 1u) << 2u);
	can_id_and = 0x01FCu & ((CAN_CONFIG_MODULE_NR_BROADCAST - 1u) << 2u);

	for (i = 0; i < can_config_nr_of_modules; i++)
	{
		can_id_or |= (0x01FCu & ((can_config_module_tab[i].module_nr - 1u) << 2u));
		can_id_and &= (0x01FCu & ((can_config_module_tab[i].module_nr - 1u) << 2u));
	}

	mask = 0x01FCu & (can_id_and | ~can_id_or);

#ifdef LI_CAN_SLV_ASYNC
	/* change one asynchronous control receive object per logical module */
	can_id = CAN_CONFIG_ASYNC_CTRL_RX_SLAVE_ID + ((new_module_nr - 1u) << 2u);
	err = can_config_get_async_ctrl_rx_obj(table_pos, &msg_obj);
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_main_define_msg_obj(msg_obj, can_id, CAN_CONFIG_ACCEPTANCE_ONE_ID, CAN_CONFIG_ASYNC_CTRL_RX_DLC, CAN_CONFIG_DIR_RX, CAN_MAIN_ASYNC_CTRL_SERVICE_ID_RX, CAN_OBJECT_NOT_SYNC);
	}
#endif // #ifdef LI_CAN_SLV_ASYNC

#ifdef LI_CAN_SLV_ASYNC
	/* define message object for receiving asynchronous data */
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_main_define_msg_obj(CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_DATA_RX, CAN_CONFIG_ASYNC_DATA_RX_SLAVE_ID, CAN_CONFIG_ACCEPTANCE_MASK | mask, CAN_CONFIG_ASYNC_DATA_RX_DLC, CAN_CONFIG_DIR_RX, CAN_MAIN_ASYNC_SERVICE_ID_RX, CAN_OBJECT_NOT_SYNC);
	}
#endif // #ifdef LI_CAN_SLV_ASYNC

	/* define message object for receiving system messages */
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_main_define_msg_obj(CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS1, CAN_CONFIG_SYS_MSG_ID, CAN_CONFIG_ACCEPTANCE_MASK | mask, CAN_CONFIG_SYS_MSG_DLC, CAN_CONFIG_DIR_RX, CAN_MAIN_SERVICE_ID_RX, CAN_OBJECT_NOT_SYNC);
	}

#if 1
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_main_define_msg_obj(CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS2, CAN_CONFIG_SYS_MSG_ID, CAN_CONFIG_ACCEPTANCE_MASK | mask, CAN_CONFIG_SYS_MSG_DLC, CAN_CONFIG_DIR_RX, CAN_MAIN_SERVICE_ID_RX, CAN_OBJECT_NOT_SYNC);
	}

	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_hw_combine_msg_obj_to_two_stage_fifo(CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS1, CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS2);
	}
#endif

	/* in case of silent state call silent awake routine */
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_config_set_module_silent_awake(table_pos, can_config_module_silent[table_pos]);
	}
	return (err);
}
#endif // #ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR

/**
 * @brief calls the function for setting the module silent awake
 * @param pfnc function pointer for setting the module silent awake
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_set_module_silent_awake_call_fnc(void(*pfnc)(uint16_t table_pos, char_t *type, li_can_slv_module_nr_t module_nr, can_config_module_silent_t module_silent_awake))
{
	can_config_set_module_silent_awake_call = pfnc;
	return (LI_CAN_SLV_ERR_OK);
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

/**
 *
 */
#ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR
void li_can_slv_config_set_module_nr_change_voter(lcsa_module_change_voter_state_t (*pfnc)(char_t *module_type, li_can_slv_module_nr_t new_module_nr, li_can_slv_module_nr_t actual_module_nr))
{
	module_nr_change_validator = pfnc;
}
#endif // #ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#if defined(LI_CAN_SLV_BOOT) && defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR)
/**
 * @brief change module in can configuration
 * @param module_nr new module number
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t can_config_change_module(li_can_slv_module_nr_t module_nr)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t i;
	uint16_t can_id_or, can_id_and, mask;
	const char_t *module_type = CAN_CONFIG_MODULE_TYPE_UNKNOWN;

	/* set module number */
	can_config_module_tab.module_nr = module_nr;

	can_id_or = 0x01FC & ((CAN_CONFIG_MODULE_NR_BROADCAST - 1) << 2);
	can_id_and = 0x01FC & ((CAN_CONFIG_MODULE_NR_BROADCAST - 1) << 2);

	can_id_or |= (0x01FC & ((can_config_module_tab.module_nr - 1) << 2));
	can_id_and &= (0x01FC & ((can_config_module_tab.module_nr - 1) << 2));

	mask = 0x01FC & (can_id_and | ~can_id_or);

	/* define message object for receiving asynchron control data */
	err = can_main_define_msg_obj(CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL1_RX, CAN_CONFIG_ASYNC_CTRL_RX_SLAVE_ID, CAN_CONFIG_ACCEPTANCE_MASK | mask, CAN_CONFIG_ASYNC_CTRL_RX_DLC, CAN_CONFIG_DIR_RX, CAN_MAIN_SERVICE_ID_RX, CAN_OBJECT_NOT_SYNC);

	/* define message object for receiving system messages */
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_main_define_msg_obj(CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS1, CAN_CONFIG_SYS_MSG_ID, CAN_CONFIG_ACCEPTANCE_MASK | mask, CAN_CONFIG_SYS_MSG_DLC, CAN_CONFIG_DIR_RX, CAN_MAIN_SERVICE_ID_RX, CAN_OBJECT_NOT_SYNC);
	}

	/* in case of silent state call silent awake routine */
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_config_set_module_silent_awake(can_config_module_silent);
	}

	return (err);
}
#endif // #if defined(LI_CAN_SLV_BOOT) && defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR)

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief set module in can configuration
 * @param table_pos position in configuration table
 * @param[in] module is the pointer to application module type configuration
 * @param module_nr is the module number of the configured module
 * @param rx0 data pointer to application data of CAN receive object 0
 * @param rx1 data pointer to application data of CAN receive object 1
 * @param rx2 data pointer to application data of CAN receive object 2
 * @param rx3 data pointer to application data of CAN receive object 3
 * @param tx0 data pointer to application data of CAN transmit object 0
 * @param tx1 data pointer to application data of CAN transmit object 1
 * @param tx2 data pointer to application data of CAN transmit object 2
 * @param tx3 data pointer to application data of CAN transmit object 3
 * @return #LI_CAN_SLV_ERR_OK, #LI_CAN_SLV_ERR_OK, #ERR_MSG_CAN_MAIN_OVER_TABLE_POS, #ERR_MSG_CAN_MAIN_OVER_OBJECT_NR, #LI_CAN_SLV_ERR_OK or #ERR_MSG_CAN_NO_MSG_OBJ_FREE
 */
static li_can_slv_errorcode_t can_config_set_module(uint16_t table_pos, const li_can_slv_config_module_t *module, li_can_slv_module_nr_t module_nr,
        void *rx0, void *rx1, void *rx2, void *rx3,
        void *tx0, void *tx1, void *tx2, void *tx3)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t i;
	uint16_t can_id, msg_obj;
	uint16_t can_id_or, can_id_and, mask;

	can_port_memory_cpy(&can_config_module_tab[table_pos], module, sizeof(li_can_slv_config_module_t));

	// limit module number
	if ((module_nr > CAN_CONFIG_MAX_MODULE_NR) || (module_nr < CAN_CONFIG_MIN_MODULE_NR))
	{
		module_nr = LI_CAN_SLV_CONFIG_DEF_MOD_NR1 + table_pos;
	}

#ifdef LI_CAN_SLV_DEBUG_CAN_INIT
	LI_CAN_SLV_DEBUG_PRINT("set module, table_pos: %d, modnr: %d\n", table_pos, module_nr);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT

	can_config_module_tab[table_pos].module_nr = module_nr;

	// set receive application data pointers
	can_config_module_tab[table_pos].rx[0] = rx0;
	can_config_module_tab[table_pos].rx[1] = rx1;
	can_config_module_tab[table_pos].rx[2] = rx2;
	can_config_module_tab[table_pos].rx[3] = rx3;

	// set transmit application data pointers
	can_config_module_tab[table_pos].tx[0] = tx0;
	can_config_module_tab[table_pos].tx[1] = tx1;
	can_config_module_tab[table_pos].tx[2] = tx2;
	can_config_module_tab[table_pos].tx[3] = tx3;

	/* define CAN message object for transmitting synchronous data from main CAN Node to master               */
	/* define CAN message object for receiving synchronous data from main CAN Node on monitor CAN Node        */
	/* define CAN message object for receiving synchronous data from main CPU on monitor CPU on main CAN Node */
	/* the can_id is always the same for this three use cases                                                 */
	can_id = CAN_CONFIG_DATA_TX_MASK + ((module_nr - 1) << 2);
	can_config_module_tab[table_pos].tx_obj_sync = can_config_module_tab[table_pos].tx_obj;

#ifdef LI_CAN_SLV_MAIN_MON
	if (can_mainmon_type == CAN_MAINMON_TYPE_MAIN)
	{
#endif // #ifdef LI_CAN_SLV_MAIN_MON

		/* define CAN message object for transmitting synchronous data from main CAN Node to master */
		for (i = 0; i < can_config_module_tab[table_pos].tx_obj; i++)
		{
			can_config_module_tab[table_pos].tx_dlc_sync[i] = can_config_module_tab[table_pos].tx_dlc[i];

			/* calculation of transmit CAN identifier */
			err = can_main_sync_process_tx_data_cnfg(table_pos, i, module_nr);
			if (err != LI_CAN_SLV_ERR_OK)
			{
				return (err);
			}

#if defined(LI_CAN_SLV_MON)
			/* define CAN message object for receiving synchronous data from main CAN Node on monitor CAN Node on the Main CPU */
			err = can_mon_get_next_free_msg_obj(&msg_obj);
			if (err != LI_CAN_SLV_ERR_OK)
			{
				return (err);
			}

			// set used message object
			can_config_module_tab[table_pos].tx_msg_obj_mon[i] = msg_obj;
			err = can_mon_define_msg_obj(msg_obj, can_id + i, CAN_CONFIG_ACCEPTANCE_ONE_ID, can_config_module_tab[table_pos].tx_dlc[i], CAN_CONFIG_DIR_RX, CAN_MON_ISR_ID_RX_MAIN, CAN_OBJECT_IS_SYNC);
			if (err != LI_CAN_SLV_ERR_OK)
			{
				return (err);
			}

			err = can_mon_msg_obj_tx_data_cnfg(msg_obj, table_pos);
			if (err != LI_CAN_SLV_ERR_OK)
			{
				return (err);
			}
#endif // #if defined(LI_CAN_SLV_MON)
		}
#ifdef LI_CAN_SLV_MAIN_MON
	}
#endif // #ifdef LI_CAN_SLV_MAIN_MON

#ifdef LI_CAN_SLV_MAIN_MON
	if (can_mainmon_type == CAN_MAINMON_TYPE_MON)
	{
		/* define CAN message object for receiving synchronous data from main CPU on monitor CPU on main CAN Node */
		for (i = 0; i < can_config_module_tab[table_pos].tx_obj; i++)
		{
			can_config_module_tab[table_pos].tx_dlc_sync[i] = can_config_module_tab[table_pos].tx_dlc[i];

			err = can_main_get_next_free_msg_obj(&msg_obj);
			if (err != LI_CAN_SLV_ERR_OK)
			{
				return (err);
			}

			/* set used message object */
			can_config_module_tab[table_pos].tx_main_rx_msg_obj_mon[i] = msg_obj;

			err = can_main_define_msg_obj(msg_obj, can_id + i, CAN_CONFIG_ACCEPTANCE_ONE_ID, can_config_module_tab[table_pos].tx_dlc[i], CAN_CONFIG_DIR_RX, CAN_MAIN_SERVICE_ID_RX, CAN_OBJECT_IS_SYNC);
			if (err != LI_CAN_SLV_ERR_OK)
			{
				return (err);
			}

			err = can_main_msg_obj_rx_data_cnfg(msg_obj, table_pos);
			if (err != LI_CAN_SLV_ERR_OK)
			{
				return (err);
			}
		}
	}
#endif // #ifdef LI_CAN_SLV_MAIN_MON

	/* define CAN message object for receiving synchronous data from master on main & monitor CAN node */
	can_id = CAN_CONFIG_DATA_RX_MASK + ((module_nr - 1) << 2);
	can_config_module_tab[table_pos].rx_obj_sync = can_config_module_tab[table_pos].rx_obj;

	for (i = 0; i < can_config_module_tab[table_pos].rx_obj_sync; i++)
	{
		can_config_module_tab[table_pos].rx_dlc_sync[i] = can_config_module_tab[table_pos].rx_dlc[i];

		/* main CAN node */
		err = can_main_get_next_free_msg_obj(&msg_obj);
		if (err != LI_CAN_SLV_ERR_OK)
		{
			return (err);
		}

		can_config_module_tab[table_pos].rx_msg_obj_main[i] = msg_obj;
		err = can_main_define_msg_obj(msg_obj, can_id + i, CAN_CONFIG_ACCEPTANCE_ONE_ID, can_config_module_tab[table_pos].rx_dlc[i], CAN_CONFIG_DIR_RX, CAN_MAIN_SERVICE_ID_RX, CAN_OBJECT_IS_SYNC);
		if (err != LI_CAN_SLV_ERR_OK)
		{
			return (err);
		}

		err = can_main_msg_obj_rx_data_cnfg(msg_obj, table_pos);
		if (err != LI_CAN_SLV_ERR_OK)
		{
			return (err);
		}

#ifdef LI_CAN_SLV_MON
		/* monitor CAN node */
		err = can_mon_get_next_free_msg_obj(&msg_obj);
		if (err != LI_CAN_SLV_ERR_OK)
		{
			return (err);
		}

		can_config_module_tab[table_pos].rx_msg_obj_mon[i] = msg_obj;
		err = can_mon_define_msg_obj(msg_obj, can_id + i, CAN_CONFIG_ACCEPTANCE_ONE_ID, can_config_module_tab[table_pos].rx_dlc[i], CAN_CONFIG_DIR_RX, CAN_MON_ISR_ID_RX, CAN_OBJECT_IS_SYNC);
		if (err != LI_CAN_SLV_ERR_OK)
		{
			return (err);
		}

		err = can_mon_msg_obj_rx_data_cnfg(msg_obj, table_pos);
		if (err != LI_CAN_SLV_ERR_OK)
		{
			return (err);
		}
#endif // #ifdef LI_CAN_SLV_MON
	}

	can_id_or = 0x01FC & ((CAN_CONFIG_MODULE_NR_BROADCAST - 1) << 2);
	can_id_and = 0x01FC & ((CAN_CONFIG_MODULE_NR_BROADCAST - 1) << 2);

	for (i = 0; i < (table_pos + 1); i++)
	{
		can_id_or |= (0x01FC & ((can_config_module_tab[i].module_nr - 1) << 2));
		can_id_and &= (0x01FC & ((can_config_module_tab[i].module_nr - 1) << 2));
	}

	mask = 0x01FC & (can_id_and | ~can_id_or);

#ifdef LI_CAN_SLV_ASYNC
	/* define one message object to receive asynchronous control data per logical module */
	can_id = CAN_CONFIG_ASYNC_CTRL_RX_SLAVE_ID + ((module_nr - 1) << 2);
	err = can_config_get_async_ctrl_rx_obj(can_config_nr_of_modules, &msg_obj);
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_main_define_msg_obj(msg_obj, can_id, CAN_CONFIG_ACCEPTANCE_ONE_ID, CAN_CONFIG_ASYNC_CTRL_RX_DLC, CAN_CONFIG_DIR_RX, CAN_MAIN_ASYNC_CTRL_SERVICE_ID_RX, CAN_OBJECT_NOT_SYNC);
	}
#endif // #ifdef LI_CAN_SLV_ASYNC

#ifdef LI_CAN_SLV_ASYNC
	/* define message object for receive asynchronous data */
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_main_define_msg_obj(CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_DATA_RX, CAN_CONFIG_ASYNC_DATA_RX_SLAVE_ID, CAN_CONFIG_ACCEPTANCE_MASK | mask, CAN_CONFIG_ASYNC_DATA_RX_DLC, CAN_CONFIG_DIR_RX, CAN_MAIN_ASYNC_SERVICE_ID_RX, CAN_OBJECT_NOT_SYNC);
	}
#endif // #ifdef LI_CAN_SLV_ASYNC

	/* define message object for receiving system messages */
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_main_define_msg_obj(CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS1, CAN_CONFIG_SYS_MSG_ID, CAN_CONFIG_ACCEPTANCE_MASK | mask, CAN_CONFIG_SYS_MSG_DLC, CAN_CONFIG_DIR_RX, CAN_MAIN_SERVICE_ID_RX, CAN_OBJECT_NOT_SYNC);
	}

#if 1
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_main_define_msg_obj(CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS2, CAN_CONFIG_SYS_MSG_ID, CAN_CONFIG_ACCEPTANCE_MASK | mask, CAN_CONFIG_SYS_MSG_DLC, CAN_CONFIG_DIR_RX, CAN_MAIN_SERVICE_ID_RX, CAN_OBJECT_NOT_SYNC);
	}

	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_hw_combine_msg_obj_to_two_stage_fifo(CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS1, CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS2);
	}
#endif

	/* in case of silent state call silent awake routine */
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_config_set_module_silent_awake(table_pos, can_config_module_silent[table_pos]);
	}

	can_config_nr_of_modules++;

	return (err);
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief makes dummy assignment of main rx data (can, app, dlc)
 * @param can is the pointer to the data register of the used CAN message object
 * @param app is the pointer to the application data
 * @param dlc is the data length code of transmitted message
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t rx_main_dummy(byte_t *can, void *app, uint16_t dlc)
{
	can = can; //dummy assignment
	dlc = dlc; //dummy assignment
	app = app; //dummy assignment
	return (LI_CAN_SLV_ERR_OK);
}

/**
 * @brief makes dummy assignment of main tx data (can, app, dlc)
 * @param can is the pointer to the data register of the used CAN message object
 * @param app is the pointer to the application data
 * @param dlc is the data length code of transmitted message
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t tx_main_dummy(byte_t *can, void *app, uint16_t dlc)
{
	can = can; //dummy assignment
	dlc = dlc; //dummy assignment
	app = app; //dummy assignment
	return (LI_CAN_SLV_ERR_OK);
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

#if defined(OUTER) || defined(OUTER_APP)
#ifdef LI_CAN_SLV_MON
/**
 * @brief makes dummy assignment of mon rx data (can, app, dlc)
 * @param can is the pointer to the data register of the used CAN message object
 * @param app is the pointer to the application data
 * @param dlc is the data length code of transmitted message
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t rx_mon_dummy(byte_t *can, void *app, uint16_t dlc)
{
	can = can; //dummy assignment
	dlc = dlc; //dummy assignment
	app = app; //dummy assignment
	return (LI_CAN_SLV_ERR_OK);
}
#endif // #ifdef LI_CAN_SLV_MON

#if defined(LI_CAN_SLV_MON) || defined(LI_CAN_SLV_MAIN_MON)
/**
 * @brief makes dummy assignment of mon tx data (can, app, dlc)
 * @param can is the pointer to the data register of the used CAN message object
 * @param app is the pointer to the application data
 * @param dlc is the data length code of transmitted message
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t tx_mon_dummy(byte_t *can, void *app, uint16_t dlc)
{
	can = can; //dummy assignment
	dlc = dlc; //dummy assignment
	app = app; //dummy assignment
	return (LI_CAN_SLV_ERR_OK);
}
#endif // #if defined(LI_CAN_SLV_MON) || defined(LI_CAN_SLV_MAIN_MON)
#endif	// #if defined(OUTER) || defined(OUTER_APP)

#ifdef LI_CAN_SLV_ASYNC
#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief get the corresponding asynchronous control data receive message object for desired logical module
 * @param table_pos position in configuration table
 * @param msg_obj_nr number of the message object
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t can_config_get_async_ctrl_rx_obj(uint16_t table_pos, uint16_t *msg_obj_nr)
{
	if (table_pos > (LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES - 1u))
	{
		return (ERR_MSG_CAN_CONFIG_MODULE_WRONG_TABLE_POS);
	}

	/**
	 * @todo es kann hier zu einem Problem kommen bei get message object
	 */
	*msg_obj_nr = can_config_async_ctrl_rx_objs[table_pos];

	return (LI_CAN_SLV_ERR_OK);
}
#endif // #if defined(OUTER) || defined(OUTER_APP)
#endif // #ifdef LI_CAN_SLV_ASYNC

#ifndef LI_CAN_SLV_BOOT
/**
 *
 */
static uint16_t get_table_pos_from_type_and_actual_module_number(char_t *module_type, li_can_slv_module_nr_t actual_module_nr)
{
	uint16_t i = 0;

	for (i = 0; i < LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES; ++i)
	{
		if (li_can_slv_port_string_n_cmp(module_type, can_config_module_tab[i].type, CAN_CONFIG_TYPE_STRING_LENGTH) == 0)
		{
			if (can_config_module_tab[i].module_nr == actual_module_nr)
			{
				return (i);
			}
		}
	}

	return (LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES);
}
#endif // #ifndef LI_CAN_SLV_BOOT

/** @} */
