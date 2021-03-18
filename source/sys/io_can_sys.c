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
 * \brief Includes all routines to RX and TX the the Liebherr CAN bus system commands / system messages.
	Every system messages is processed directly in the callback / interrupt service routine of the main
	CAN-controller.<br>

	<h3>system messages from master to module</h3>
	System messages with a DLC != 8 will be not accepted (no reaction). If a correct system message is received
	it will be processed by functions of this software module.<br>

	<table>
	<tr> <td ><b>system message</b>               <td ><b>Byte1 (ID of the message)</b> </td> </tr>
	<tr> <td >status request                      <td >0x01 </td></tr>
	<tr> <td >change CAN baud rate                <td >0x02 </td></tr>
	<tr> <td >version request                     <td >0x05 </td></tr>
	<tr> <td >process configuration               <td >0x08 </td></tr>
	<tr> <td >hardware version request            <td >0x0A </td></tr>
	<tr> <td >RX rate	                          <td >0x0B </td></tr>
	<tr> <td >change module number                <td >0x20 </td></tr>
	<tr> <td >stay silent and stop transmission   <td >0x21 </td></tr>
	<tr> <td >awake and resume transmission       <td >0x22 </td></tr>
	<tr> <td >show module identification          <td >0x23 </td></tr>
	<tr> <td >Factory Reset                       <td >0x30 </td></tr>
	<tr> <td >Change module type                  <td >0x31 </td></tr>
	</table>

	<h3>system messages from module to master</h3>
	Every system message is sent by the CAN system TX queue.
	<table>
	<tr> <td ><b>system message</b>               <td ><b>Byte1 (ID of the message)</b> </td> </tr>
	<tr> <td >module error 		                  <td >0x03 </td></tr>
	<tr> <td >status acknowledge				  <td >0x04 </td></tr>
	<tr> <td >version acknowledge 1               <td >0x05 </td></tr>
	<tr> <td >version acknowledge 2               <td >0x06 </td></tr>
	</table>\n
 */

/**
 * @file io_can_sys.c
 * @addtogroup can_sys
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/sys/io_can_sys.h>
#include <li_can_slv/sync/io_can_sync.h>
#include <li_can_slv/config/io_can_config.h>
#include <li_can_slv/core/io_can.h>
#include <li_can_slv/core/io_can_main.h>

#include "io_can_port.h"

#include <li_can_slv/error/io_can_error.h>
#include <li_can_slv/error/io_can_errno.h>

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
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/
static uint16_t can_sys_first_status_detect = 0;
static void(*li_can_slv_sys_first_status_request_cbk)(void) = NULL;

#ifdef LI_CAN_SLV_SYS_FACTORY_RESET_CBK
static li_can_slv_factory_reset_cbk_funcp_t li_can_slv_factory_reset_cbk = NULL;
#endif // #ifdef LI_CAN_SLV_SYS_FACTORY_RESET_CBK

#ifdef LI_CAN_SLV_SYS_RANDOM_STATUS_ACKNOWLEDGE
static uint32_t li_can_slv_rand_value;
#endif // #ifdef LI_CAN_SLV_SYS_RANDOM_STATUS_ACKNOWLEDGE

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_BOOT
static li_can_slv_errorcode_t can_sys_status_ackn(void);
static li_can_slv_errorcode_t can_sys_version_ackn(void);
#else // #ifdef LI_CAN_SLV_BOOT
static li_can_slv_errorcode_t can_sys_status_ackn(uint16_t table_pos);
static li_can_slv_errorcode_t can_sys_version_ackn(uint16_t table_pos);
#endif // #ifdef LI_CAN_SLV_BOOT
CAN_PORT_STATIC_INLINE li_can_slv_errorcode_t can_sys_version_ackn_send_version1(li_can_slv_module_nr_t module_nr, uint8_t bootkernel_version_major, uint16_t bootkernel_version_minor, uint8_t modulsoftware_version_major, uint16_t modulsoftware_version_minor);
CAN_PORT_STATIC_INLINE li_can_slv_errorcode_t can_sys_version_ackn_send_version2(li_can_slv_module_nr_t module_nr);
CAN_PORT_STATIC_INLINE li_can_slv_errorcode_t can_sys_status_ackn_send(li_can_slv_module_nr_t module_nr, char_t *module_type, uint32_t serial_number);
static li_can_slv_errorcode_t can_sys_set_silent_awake(const byte_t *src, li_can_slv_module_nr_t module_nr, can_config_module_silent_t silent_awake);
#ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR
static uint8_t check_if_serial_number_is_valid_from_can_data(uint32_t serial_number_expected, const byte_t *mmsb, const byte_t *msb, const byte_t *lsb);
#endif // #ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR

#ifdef LI_CAN_SLV_SYS_RANDOM_STATUS_ACKNOWLEDGE
static uint32_t li_can_slv_rand(void);
static void li_can_slv_seeding(uint32_t seed);
#endif // #ifdef LI_CAN_SLV_SYS_RANDOM_STATUS_ACKNOWLEDGE

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
/**
 * @brief: io_can_sys_init initialize function
 * @return
 */
li_can_slv_errorcode_t can_sys_init(void)
{
	can_sys_first_status_detect = 0;
	li_can_slv_sys_first_status_request_cbk = NULL;
#ifdef LI_CAN_SLV_SYS_RANDOM_STATUS_ACKNOWLEDGE
	li_can_slv_seeding(li_can_slv_port_get_serialnumber());
#endif // #ifdef LI_CAN_SLV_SYS_RANDOM_STATUS_ACKNOWLEDGE
	return (LI_CAN_SLV_ERR_OK);
}

/****************************************************************************/
/**
 * \brief The reaction of the can_sys_msg_rx function depends on: <br>
 * - <b>module number</b><br>
 * Every module number which is not configured as a logical module (see therefore io_can_config)
 * will be not accepted (no reaction).
 * If module number is zero, the message is a broadcast message.
 * - <b>dlc</b><br>
 * System messages with a DLC != 8 will be not accepted (no reaction).
 * - <b>byte_t *src</b><br>
 * A system message includes 8 data bytes. The first data byte represented the identifier
 * of the system message. Following system messages are supported:
 *
 * <table>
 * <tr> <td ><b>system message</b>               <td ><b>Byte1 (ID of the message)</b> </td> </tr>
 * <tr> <td >status request                      <td >0x01 </td></tr>
 * <tr> <td >change CAN baud rate                <td >0x02 </td></tr>
 * <tr> <td >version request                     <td >0x05 </td></tr>
 * <tr> <td >process configuration               <td >0x08 </td></tr>
 * <tr> <td >hardware version request            <td >0x0A </td></tr>
 * <tr> <td >RX rate	                         <td >0x0B </td></tr>
 * <tr> <td >change module number                <td >0x20 </td></tr>
 * <tr> <td >stay silent and stop transmission   <td >0x21 </td></tr>
 * <tr> <td >awake and resume transmission       <td >0x22 </td></tr>
 * <tr> <td >show module identification          <td >0x23 </td></tr>
 * </table>
 *
 * \param module_nr represented the module number of the received can message
 * \param dlc data length code of the system message (dlc must be 8)
 * \param[in] src source pointer of the data registers
 * \return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_sys_msg_rx(li_can_slv_module_nr_t module_nr, uint16_t dlc, byte_t const *src)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	can_config_bdr_t baudrate;
	li_can_slv_node_mode_t mode;
#ifndef LI_CAN_SLV_BOOT
	uint16_t table_pos;
#endif // #ifndef LI_CAN_SLV_BOOT
	uint8_t message_is_broadcast = FALSE;
	uint16_t module_found = 0;
#ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR
	uint32_t serial_nr;
	li_can_slv_module_nr_t new_module_nr;
#endif // #ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR
	can_config_module_silent_t module_silent_awake;
#ifndef LI_CAN_SLV_BOOT
	uint16_t nr_of_modules = 0;
#endif // #ifndef LI_CAN_SLV_BOOT
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
	msg_code_t errnum;
	err_prio_t priority;
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
#ifdef LI_CAN_SLV_SYS_RANDOM_STATUS_ACKNOWLEDGE
	uint32_t wait_status_req;
#endif // #ifdef LI_CAN_SLV_SYS_RANDOM_STATUS_ACKNOWLEDGE
	byte_t add_info;
	byte_t group_subgroup;

#ifdef LI_CAN_SLV_CHANGE_CAN_BAUDRATE_DELAY
	uint32_t i;
#endif // #ifdef LI_CAN_SLV_CHANGE_CAN_BAUDRATE_DELAY

#ifdef LI_CAN_SLV_DEBUG_MAIN_SYS_HANDLER_EXTENDED
	LI_CAN_SLV_DEBUG_PRINT("\n\ncan_sys_msg_rx");
#endif // #ifdef LI_CAN_SLV_DEBUG_MAIN_SYS_HANDLER_EXTENDED

	if (dlc != CAN_CONFIG_SYS_MSG_DLC)
	{
		return (ERR_MSG_CAN_INVALID_SYS_MSG_DLC);
	}
#ifdef LI_CAN_SLV_BOOT
	if (module_nr != CAN_CONFIG_MODULE_NR_BROADCAST)
	{
		message_is_broadcast = FALSE;

		err = can_config_module_nr_valid(module_nr, &module_silent_awake, &module_found);
		if (!module_found)
		{
			return LI_CAN_SLV_ERR_OK;
		}
	}
	else
	{
		message_is_broadcast = TRUE;
	}
#else // #ifdef LI_CAN_SLV_BOOT
	if (module_nr != CAN_CONFIG_MODULE_NR_BROADCAST)
	{
		message_is_broadcast = FALSE;

		/* find module position in configuration table */
		err = can_config_module_nr_valid(module_nr, &table_pos, &module_silent_awake, &module_found);
		if (!module_found)
		{
			/**
			 * @todo check for next version if we should report an error here
			 */
			/* if the module is not found we ignore this message */
			return LI_CAN_SLV_ERR_OK;
		}
	}
	else
	{
		message_is_broadcast = TRUE;

		can_config_get_nr_of_modules(&nr_of_modules);
		if (nr_of_modules == 0)
		{
			return (ERR_MSG_CAN_CONFIG_MODULE_NOT_FOUND);
		}
	}
#endif // #ifdef LI_CAN_SLV_BOOT
	switch (src[0])
	{
		case CAN_SYS_M2S_STATUS_REQUEST:
			if (can_sys_first_status_detect == 0)
			{
				can_sys_first_status_detect = 1;
				if (NULL != li_can_slv_sys_first_status_request_cbk)
				{
					li_can_slv_sys_first_status_request_cbk();
				}
			}
#ifdef LI_CAN_SLV_BOOT
			err = can_sys_status_ackn();
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
			/*--------------------------------------------------------------*/
			/* send system errors if valid                                   */
			/*--------------------------------------------------------------*/
			while (can_error_sys_error_count() != 0)
			{
				err = can_error_sys_error_get_full(&errnum, &add_info, &priority, &group_subgroup, &module_nr);
				if (err == LI_CAN_SLV_ERR_OK)
				{
					if (LI_CAN_SLV_MODE_OPERATIONAL == li_can_slv_get_node_mode())
					{
						err = can_sys_send_error_full(module_nr, errnum, add_info, priority, group_subgroup);
					}
				}
			}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
#else // #ifdef LI_CAN_SLV_BOOT

#ifdef LI_CAN_SLV_SYS_RANDOM_STATUS_ACKNOWLEDGE
			wait_status_req = li_can_slv_rand();
			li_can_slv_port_wait_us(wait_status_req);
#endif // #ifdef LI_CAN_SLV_SYS_RANDOM_STATUS_ACKNOWLEDGE
			if (message_is_broadcast)
			{
				for (table_pos = 0; table_pos < nr_of_modules; table_pos++)
				{
					err = can_sys_status_ackn(table_pos);
					if (err != LI_CAN_SLV_ERR_OK)
					{
						return (err);
					}
				}
			}
			else
			{
				err = can_sys_status_ackn(table_pos);
			}
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
			/*--------------------------------------------------------------*/
			/* send system errors if valid                                  */
			/*--------------------------------------------------------------*/
			while (can_error_sys_error_count() != 0)
			{
				err = can_error_sys_error_get_full(&errnum, &add_info, &priority, &group_subgroup, &module_nr);
				if (err == LI_CAN_SLV_ERR_OK)
				{
					err = can_sys_send_error_full(module_nr, errnum, add_info, priority, group_subgroup);
				}
			}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
#endif // #ifdef LI_CAN_SLV_BOOT
			break;

		case CAN_SYS_M2S_CHANGE_CAN_BAUDRATE:
			if (message_is_broadcast == FALSE)
			{
				return ERR_MSG_CAN_SYSTEM_MSG_NOT_ALLOWED_DIRECT;
			}
			baudrate = (src[1] << 8) + src[2];
#ifdef LI_CAN_SLV_DEBUG_SYS_CHANGE_CAN_BAUDRATE
			LI_CAN_SLV_DEBUG_PRINT("\nchange CAN baudrate: %d", baudrate);
#endif // #ifdef LI_CAN_SLV_DEBUG_SYS_CHANGE_CAN_BAUDRATE

#ifdef LI_CAN_SLV_CHANGE_CAN_BAUDRATE_DELAY
			for (i = 0; i <= 100; i++)
			{
				can_port_nop();
				can_port_nop();
				can_port_nop();
				can_port_nop();
			}
#endif // #ifdef LI_CAN_SLV_CHANGE_CAN_BAUDRATE_DELAY

			mode = li_can_slv_get_node_mode();
			err = li_can_slv_set_node_mode(LI_CAN_SLV_MODE_STOPPED);
			if (err == LI_CAN_SLV_ERR_OK)
			{
				err = can_config_set_baudrate(baudrate);
			}

			if (err == LI_CAN_SLV_ERR_OK)
			{
				err = li_can_slv_set_node_mode(mode);
			}

			break;

#if defined(OUTER) || defined(OUTER_APP)
		case CAN_SYS_M2S_SYSTEM_ERROR_RX:
			return LI_CAN_SLV_ERR_NOT_IMPLEMENTED;
			break;
#endif // #if defined(OUTER) || defined(OUTER_APP)

		case CAN_SYS_M2S_VERSION_REQUEST:
#ifdef LI_CAN_SLV_BOOT
			err = can_sys_version_ackn();
#else // #ifdef LI_CAN_SLV_BOOT
			if (message_is_broadcast)
			{
				/* broadcast message */
				for (table_pos = 0; table_pos < nr_of_modules; table_pos++)
				{
					err = can_sys_version_ackn(table_pos);
					if (err != LI_CAN_SLV_ERR_OK)
					{
						return (err);
					}
				}
			}
			else
			{
				err = can_sys_version_ackn(table_pos);
			}
#endif // #ifdef LI_CAN_SLV_BOOT
			break;

#if defined(OUTER) || defined(OUTER_APP)
		case CAN_SYS_M2S_CURRENT_SYSTEM_TIME:
			return LI_CAN_SLV_ERR_NOT_IMPLEMENTED;
			break;
#endif // #if defined(OUTER) || defined(OUTER_APP)
#if defined(OUTER) || defined(OUTER_APP)
		case CAN_SYS_M2S_PROCESS_CONFIGURATION:
			err = li_can_slv_sync_set_process_cycle_time((uint32_t)src[2]);
			return err;
			break;
#endif // #if defined(OUTER) || defined(OUTER_APP)
#if defined(OUTER) || defined(OUTER_APP)
		case CAN_SYS_M2S_HARDWARE_VERSION_REQUEST:
			return LI_CAN_SLV_ERR_NOT_IMPLEMENTED;
			break;
#endif // #if defined(OUTER) || defined(OUTER_APP)
#if defined(OUTER) || defined(OUTER_APP)
		case CAN_SYS_M2S_RX_RATE:
			return LI_CAN_SLV_ERR_NOT_IMPLEMENTED;
			break;
#endif // #if defined(OUTER) || defined(OUTER_APP)
#ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR
		case CAN_SYS_M2S_CHANGE_MODULE_NUMBER:
			if (message_is_broadcast)
			{
				return ERR_MSG_CAN_SYSTEM_MSG_NOT_ALLOWED_BROADCAST;
			}
			serial_nr = can_port_get_serialnumber();

			if (check_if_serial_number_is_valid_from_can_data(serial_nr, &src[5], &src[6], &src[7]))
			{
				new_module_nr = (li_can_slv_module_nr_t) src[2];
#ifdef LI_CAN_SLV_BOOT
				err = can_config_change_module_nr(new_module_nr);
#else
				err = can_config_change_module_nr(table_pos, new_module_nr);
#endif // #ifdef LI_CAN_SLV_BOOT
			}
			break;
#endif // #ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR
		case CAN_SYS_M2S_STAY_SILENT:
			if (message_is_broadcast)
			{
				return ERR_MSG_CAN_SYSTEM_MSG_NOT_ALLOWED_BROADCAST;
			}
			err = can_sys_set_silent_awake(src, module_nr, LI_CAN_SLV_CONFIG_MODULE_STATE_SILENT);
			break;

		case CAN_SYS_M2S_AWAKE:
			if (message_is_broadcast)
			{
				return ERR_MSG_CAN_SYSTEM_MSG_NOT_ALLOWED_BROADCAST;
			}
			err = can_sys_set_silent_awake(src, module_nr, LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE);
			break;

		case CAN_SYS_M2S_FACTORY_RESET:
#ifdef LI_CAN_SLV_SYS_FACTORY_RESET_CBK
			if (NULL != li_can_slv_factory_reset_cbk)
			{
				li_can_slv_factory_reset_cbk(src[1]);
			}
#else
			return LI_CAN_SLV_ERR_OK;
#endif // #ifdef LI_CAN_SLV_SYS_FACTORY_RESET_CBK
			break;

#if defined(OUTER) || defined(OUTER_APP)
		case CAN_SYS_M2S_SHOW_MODULE_IDENTIFICATION:
			return LI_CAN_SLV_ERR_NOT_IMPLEMENTED;
			break;
#endif // #if defined(OUTER) || defined(OUTER_APP)

#ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_TYPE
		case CAN_SYS_M2S_CHANGE_MODULE_TYPE:
			if (message_is_broadcast)
			{
				return ERR_MSG_CAN_SYSTEM_MSG_NOT_ALLOWED_BROADCAST;
			}
			serial_nr = can_port_get_serialnumber();

			if (check_if_serial_number_is_valid_from_can_data(serial_nr, &src[5], &src[6], &src[7]))
			{
				can_config_change_module_type(table_pos, (char_t *)&src[1]);
			}
			break;
#endif // #ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_TYPE
		default:
			/*--------------------------------------------------------------*/
			/* unknown system command										*/
			/*--------------------------------------------------------------*/
			err = ERR_MSG_CAN_SYS_UNKNOWN_COMMAND;
			break;
	}
	return (err);
}

#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
li_can_slv_errorcode_t can_sys_send_error_full(li_can_slv_module_nr_t module_nr, msg_code_t errnum, byte_t add_info, err_prio_t priority, byte_t group_subgroup)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	byte_t data[CAN_CONFIG_SYS_MSG_DLC];

	if ((module_nr > CAN_CONFIG_MAX_MODULE_NR) || (module_nr < CAN_CONFIG_MIN_MODULE_NR))
	{
		module_nr = can_config_get_module_nr_main();
	}

#ifdef LI_CAN_SLV_MAIN_MON
	if (can_mainmon_type == CAN_MAINMON_TYPE_MAIN)
	{
#endif // #ifdef LI_CAN_SLV_MAIN_MON
		data[0] = CAN_SYS_S2M_MODULE_ERROR;
		// adopt error code to match global (external) error code system
		// errnum = MSG_SG_EXPAND(errnum) | (group_subgroup >> 4);
		data[1] = (byte_t)(errnum >> 24);
		data[2] = (byte_t)(errnum >> 16);
		data[3] = (byte_t)(errnum >> 8);
		data[4] = (byte_t)(errnum);
		data[5] = add_info;
		/* priority */
		data[6] = (byte_t)(priority);
		/* group/subgroup information */
		data[7] = (byte_t)(group_subgroup);

		err = can_main_send_queue_system_tx(module_nr, &data[0]);
		if (err != LI_CAN_SLV_ERR_OK)
		{
			return (err);
		}
#ifdef LI_CAN_SLV_MAIN_MON
	}
#endif // #ifdef LI_CAN_SLV_MAIN_MON
	return (LI_CAN_SLV_ERR_OK);
}
#endif //#ifdef LI_CAN_SLV_SYS_MODULE_ERROR

/*!
 * \brief can_sys_set_first_status_request_call_fnc
 * \return	#li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_sys_set_first_status_request_call_fnc(void (*pfnc)(void))
{
	li_can_slv_sys_first_status_request_cbk = pfnc;
	return (LI_CAN_SLV_ERR_OK);
}

#ifdef LI_CAN_SLV_SYS_FACTORY_RESET_CBK
/**
 * @param cbk callback function for factory reset
 */
void li_can_slv_sys_set_factory_reset_cbk(li_can_slv_factory_reset_cbk_funcp_t cbk)
{
	if (NULL == li_can_slv_factory_reset_cbk)
	{
		li_can_slv_factory_reset_cbk = cbk;
	}
}
#endif // #ifdef LI_CAN_SLV_SYS_FACTORY_RESET_CBK

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_BOOT
/**
 * @brief send system message status acknowledge
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t can_sys_status_ackn(void)
{
	li_can_slv_module_nr_t module_nr;
	uint32_t serial_number;
	li_can_slv_node_mode_t mode;

	mode = li_can_slv_get_node_mode();
	if (LI_CAN_SLV_MODE_LISTEN_ONLY == mode)
	{
		return LI_CAN_SLV_ERR_OK;
	}

#ifdef LI_CAN_SLV_MAIN_MON
	if (can_mainmon_type == CAN_MAINMON_TYPE_MON)
	{
		return (LI_CAN_SLV_ERR_OK);
	}
#endif // #ifdef LI_CAN_SLV_MAIN_MON

	module_nr = (li_can_slv_module_nr_t)can_config_get_module_nr();
	serial_number = (uint32_t)can_port_get_serialnumber();

	return (can_sys_status_ackn_send(module_nr, &can_config_module_tab.type[0], serial_number));
}
#else // #ifdef LI_CAN_SLV_BOOT
/**
 * @brief send system message status acknowledge
 * @param table_pos	position of module in the config_module table
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t can_sys_status_ackn(uint16_t table_pos)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	char_t module_type[CAN_CONFIG_TYPE_STRING_LENGTH];
	li_can_slv_module_nr_t module_nr;
	uint32_t serial_number;
	li_can_slv_node_mode_t mode;

	mode = li_can_slv_get_node_mode();
	if (LI_CAN_SLV_MODE_LISTEN_ONLY == mode)
	{
		return LI_CAN_SLV_ERR_OK;
	}

#ifdef LI_CAN_SLV_MAIN_MON
	/* not status acknowledge of monitor CPU required */
	if (can_mainmon_type == CAN_MAINMON_TYPE_MON)
	{
		return (LI_CAN_SLV_ERR_OK);
	}
#endif // #ifdef LI_CAN_SLV_MAIN_MON

	err = can_config_get_module_type(table_pos, module_type);
	if (err != LI_CAN_SLV_ERR_OK)
	{
		return (err);
	}

	err = can_config_get_module_nr(table_pos, &module_nr);
	if (err != LI_CAN_SLV_ERR_OK)
	{
		return (err);
	}

	serial_number = can_port_get_serialnumber();

#ifdef LI_CAN_SLV_DEBUG_CAN_SYS_STATUS
	LI_CAN_SLV_DEBUG_PRINT("status ackn 1\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_SYS_STATUS

	return (can_sys_status_ackn_send(module_nr, module_type, serial_number));
}
#endif // #ifdef LI_CAN_SLV_BOOT

CAN_PORT_STATIC_INLINE li_can_slv_errorcode_t can_sys_status_ackn_send(li_can_slv_module_nr_t module_nr, char_t *module_type, uint32_t serial_number)
{
	byte_t data[CAN_CONFIG_SYS_MSG_DLC];

	data[0] = CAN_SYS_S2M_STATUS_AKNOWLEDGE;
	can_port_string_n_cpy((char_t *)&data[1], module_type, (CAN_CONFIG_TYPE_STRING_LENGTH - 1)); // module type
	data[5] = (byte_t)(serial_number >> 16); // mmsb serial number
	data[6] = (byte_t)(serial_number >> 8); // msb serial number
	data[7] = (byte_t)(serial_number); // lsb serial number

	return (can_main_send_queue_system_tx(module_nr, data));
}

#ifdef LI_CAN_SLV_BOOT
/****************************************************************************/
/*!
 * \brief send system message version acknowledge
 * \remarks version acknowledge
 * \return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t can_sys_version_ackn(void)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	li_can_slv_module_nr_t module_nr;
	li_can_slv_node_mode_t mode;

	mode = li_can_slv_get_node_mode();
	if (LI_CAN_SLV_MODE_LISTEN_ONLY == mode)
	{
		return LI_CAN_SLV_ERR_OK;
	}

#ifdef LI_CAN_SLV_MAIN_MON
	if (can_mainmon_type == CAN_MAINMON_TYPE_MON)
	{
		return (LI_CAN_SLV_ERR_OK);
	}
#endif // #ifdef LI_CAN_SLV_MAIN_MON

	module_nr = can_config_get_module_nr();

	// version answer 1
	// in boot mode core sends version 0.0 for boot kernel and module software
	err = can_sys_version_ackn_send_version1(module_nr, 0, 0, 0, 0);
	if (err != LI_CAN_SLV_ERR_OK)
	{
		return (err);
	}

	// version answer 2
	return (can_sys_version_ackn_send_version2(module_nr));
}
#else
/****************************************************************************/
/*!
 * \brief send system message version acknowledge
 * \param table_pos table position of the module number
 * \return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t can_sys_version_ackn(uint16_t table_pos)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	li_can_slv_module_nr_t module_nr;
	uint8_t bootkernel_version_major;
	uint16_t bootkernel_version_minor;
	uint8_t modulsoftware_version_major;
	uint16_t modulsoftware_version_minor;
	li_can_slv_node_mode_t mode;

	mode = li_can_slv_get_node_mode();
	if (LI_CAN_SLV_MODE_LISTEN_ONLY == mode)
	{
		return LI_CAN_SLV_ERR_OK;
	}

#ifdef LI_CAN_SLV_MAIN_MON
	/* not version acknowledge of monitor CPU required */
	if (can_mainmon_type == CAN_MAINMON_TYPE_MON)
	{
		return (LI_CAN_SLV_ERR_OK);
	}
#endif // #ifdef LI_CAN_SLV_MAIN_MON
	err = can_config_get_module_nr(table_pos, &module_nr);
	if (err != LI_CAN_SLV_ERR_OK)
	{
		return (err);
	}

	// version answer 1
	bootkernel_version_major = (uint8_t) can_port_get_boot_kernel_version_major();
	bootkernel_version_minor = (uint16_t) can_port_get_boot_kernel_version_minor();
	modulsoftware_version_major = (uint8_t) can_port_get_modul_software_version_major();
	modulsoftware_version_minor = (uint16_t) can_port_get_modul_software_version_minor();

	err = can_sys_version_ackn_send_version1(module_nr, bootkernel_version_major, bootkernel_version_minor, modulsoftware_version_major, modulsoftware_version_minor);
	if (err != LI_CAN_SLV_ERR_OK)
	{
		return (err);
	}

	// version answer 2
	return (can_sys_version_ackn_send_version2(module_nr));

}
#endif // #ifdef LI_CAN_SLV_BOOT

CAN_PORT_STATIC_INLINE li_can_slv_errorcode_t can_sys_version_ackn_send_version1(li_can_slv_module_nr_t module_nr, uint8_t bootkernel_version_major, uint16_t bootkernel_version_minor, uint8_t modulsoftware_version_major, uint16_t modulsoftware_version_minor)
{
	byte_t data[CAN_CONFIG_SYS_MSG_DLC];

	data[0] = CAN_SYS_S2M_VERSION_ACKNOWLEDGE_1;
	data[1] = bootkernel_version_major; // boot kernel major
	data[2] = (byte_t)((0xFF00 & bootkernel_version_minor) >> 8);  // boot kernel minor MSB
	data[3] = (byte_t)(bootkernel_version_minor);  // boot kernel minor LSB
	data[4] = 0x00; // always zero
	data[5] = modulsoftware_version_major; // module software major
	data[6] = (byte_t)((0xFF00 & modulsoftware_version_minor) >> 8);  // module software major MSB
	data[7] = (byte_t)(modulsoftware_version_minor);  // module software major LSB

#ifdef LI_CAN_SLV_DEBUG_CAN_SYS_VERSION
	LI_CAN_SLV_DEBUG_PRINT("version ackn 1\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_SYS_VERSION

	return (can_main_send_queue_system_tx(module_nr, &data[0]));
}

CAN_PORT_STATIC_INLINE li_can_slv_errorcode_t can_sys_version_ackn_send_version2(li_can_slv_module_nr_t module_nr)
{
	byte_t data[CAN_CONFIG_SYS_MSG_DLC];
	uint32_t ident_number;
	uint32_t change_index;

	ident_number = (uint32_t) can_port_get_identnumber();
	change_index = (uint32_t) can_port_get_changeindex();

	data[0] = CAN_SYS_S2M_VERSION_ACKNOWLEDGE_2;
	data[1] = (byte_t)((ident_number) >> 24U);  // MMSB
	data[2] = (byte_t)((ident_number) >> 16U);  // MSB
	data[3] = (byte_t)((ident_number) >> 8U);  // LSB
	data[4] = (byte_t)(ident_number);  // LLSB
	data[5] = (byte_t)((change_index) >> 16);  // MMSB
	data[6] = (byte_t)((change_index) >> 8);  // MSB
	data[7] = (byte_t)(change_index);  // LSB

#ifdef LI_CAN_SLV_DEBUG_CAN_SYS_VERSION
	LI_CAN_SLV_DEBUG_PRINT("version ackn 2\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_SYS_VERSION

	return (can_main_send_queue_system_tx(module_nr, &data[0]));
}

/**
 * @param[in] src source pointer of the data registers
 * @param module_nr module number of the module to set silent awake
 * @param silent_awake silent or awake
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t can_sys_set_silent_awake(const byte_t *src, li_can_slv_module_nr_t module_nr, can_config_module_silent_t silent_awake)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint32_t serial_nr;
#ifndef LI_CAN_SLV_BOOT
	uint16_t module_found = 0;
	uint16_t table_pos;
	can_config_module_silent_t module_silent;
	char_t module_type[CAN_CONFIG_TYPE_STRING_LENGTH];
#else // #ifndef LI_CAN_SLV_BOOT
	module_nr = module_nr; // dummy assignment
#endif // #ifndef LI_CAN_SLV_BOOT
	serial_nr = ((0x00FF0000 & (uint32_t) src[5] << 16) | (0x0000FF00 & (uint32_t) src[6] << 8) | (0x000000FF & (uint32_t) src[7]));

	// check serial number
	if ((can_port_get_serialnumber()) == serial_nr)
	{
#ifdef LI_CAN_SLV_BOOT
		err = can_config_set_module_silent_awake(silent_awake);
#else // #ifdef LI_CAN_SLV_BOOT
		// check module number
		err = can_config_module_nr_valid(module_nr, &table_pos, &module_silent, &module_found);
		if (err != LI_CAN_SLV_ERR_OK)
		{
			return err;
		}
		if (module_found)
		{
			// check module_type
			err = can_config_get_module_type(table_pos, &module_type[0]);
			if (err != LI_CAN_SLV_ERR_OK)
			{
				return err;
			}
			if (can_port_string_n_cmp(module_type, (char_t *)&src[1], 4) == 0)
			{
				err = can_config_set_module_silent_awake(table_pos, silent_awake);
			}
		}
#endif // #ifdef LI_CAN_SLV_BOOT
	}
	return (err);
}

#ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR
/**
 * @param serial_number_expected
 * @param mmsb
 * @param msb
 * @param lsb
 * @return
 */
static uint8_t check_if_serial_number_is_valid_from_can_data(uint32_t serial_number_expected, const byte_t *mmsb, const byte_t *msb, const byte_t *lsb)
{
	uint32_t serial_nr = 0;

	serial_nr = ((0x00FF0000 & (uint32_t) * mmsb << 16) | (0x0000FF00 & (uint32_t) * msb << 8) | (0x000000FF & (uint32_t) * lsb));

	return (serial_number_expected == serial_nr) ? (1) : (0);
}
#endif // #ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR

#ifdef LI_CAN_SLV_SYS_RANDOM_STATUS_ACKNOWLEDGE
/**
 * randomize the status acknowledge response time delay time needs only to be
 * in arbitration area; (13 Bit: 1 Start Bit, 11 Identifier Bits, 1 RTR Bit)
 * => lowest baudrate 125k => 104us
 * => the random range should be 0us...104us
 */
static uint32_t li_can_slv_rand(void)
{
	li_can_slv_rand_value = li_can_slv_rand_value * 1103515245 + 12345;
	return (uint32_t)(li_can_slv_rand_value * 65536) % 104;
}

static void li_can_slv_seeding(uint32_t seed)
{
	li_can_slv_rand_value = seed;
}
#endif //#ifdef LI_CAN_SLV_SYS_RANDOM_STATUS_ACKNOWLEDGE

/** @} */
