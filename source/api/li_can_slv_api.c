/****************************************************************************/
/*                                                                          */
/*                     Copyright (c) 2019, Liebherr PME1                    */
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
 * @file li_can_slv_api.c
 * @brief LCS API functions
 * @addtogroup lcs_api
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/api/li_can_slv_api.h>
#include <li_can_slv/core/io_can.h>
#include <li_can_slv/config/io_can_config.h>
#include "io_can_port.h"

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
lcsa_errorcode_t lcsa_init(lcsa_bdr_t baudrate)
{
#ifdef LI_CAN_SLV_DLOAD
#ifndef LI_CAN_SLV_NO_XLOAD_INFO
	lcsa_errorcode_t err;
	li_can_slv_xload_info_mode_t mode;
	li_can_slv_config_bdr_t xload_info_baudrate;
#endif // #ifndef LI_CAN_SLV_NO_XLOAD_INFO
#endif // #ifdef LI_CAN_SLV_DLOAD

#ifndef LI_CAN_SLV_NO_XLOAD_INFO
#ifdef LI_CAN_SLV_DLOAD
	err = li_can_slv_xload_info_init();
	if (err != LCSA_ERROR_OK)
	{
		can_error_sys_error_send_full(err, 0, LCSA_ERROR_MSG_PRIO_ERROR, LCSA_ERROR_GROUP_CAN, LCSA_ERROR_SUBGROUP_CAN_APPLICATION, can_config_get_module_nr_main());
	}
#endif // #ifdef LI_CAN_SLV_DLOAD

#ifdef LI_CAN_SLV_DLOAD
	li_can_slv_xload_info_get_mode(&mode);
	if ((mode == LI_CAN_SLV_XLOAD_INFO_MODE_DOWNLOAD_ACKN) || (mode == LI_CAN_SLV_XLOAD_INFO_MODE_DOWNLOAD_OUT) || (mode == LI_CAN_SLV_XLOAD_INFO_MODE_DOWNLOAD_REBOOT))
	{
		// if download or download ackn then use the baud rate of the xload_info
		li_can_slv_xload_info_get_can_baudrate(&xload_info_baudrate);
		lcsa_set_state(LI_CAN_SLV_STATE_DOWNLOAD);
		baudrate = xload_info_baudrate;
	}
#endif // #ifdef LI_CAN_SLV_DLOAD
#endif // #ifndef LI_CAN_SLV_NO_XLOAD_INFO

	return li_can_slv_init(baudrate);
}

#ifndef LI_CAN_SLV_BOOT
lcsa_errorcode_t lcsa_start(void)
{
	lcsa_errorcode_t err = LCSA_ERROR_OK;
#ifndef LI_CAN_SLV_NO_XLOAD_INFO
	li_can_slv_xload_component_t xload_component;
	li_can_slv_xload_info_mode_t xload_info_mode;
#endif

#ifdef LI_CAN_SLV_UNKNOWN_MODULE
	uint16_t module_nr;

	can_config_get_nr_of_modules(&module_nr);
	if (module_nr == 0)
	{
		can_config_add_ukwn_module(CAN_CONFIG_MODULE_TYPE_UNKNOWN, can_config_get_module_nr_main());
	}
#endif // #ifdef LI_CAN_SLV_UNKNOWN_MODULE

#ifdef LI_CAN_SLV_MAIN_MON
	if (can_mainmon_type == CAN_MAINMON_TYPE_MAIN)
	{
#endif // #ifdef LI_CAN_SLV_MAIN_MON
		can_port_transceiver_enable();
#ifdef LI_CAN_SLV_MAIN_MON
	}
#endif // #ifdef LI_CAN_SLV_MAIN_MON

#ifdef LI_CAN_SLV_RECONNECT
#ifdef LI_CAN_SLV_MAIN_MON
	if (can_mainmon_type == CAN_MAINMON_TYPE_MAIN)
	{
		// main cpu
#endif // #ifdef LI_CAN_SLV_MAIN_MON
		if (err == LI_CAN_SLV_ERR_OK)
		{
			err = li_can_slv_reconnect_init();
			if (err == LI_CAN_SLV_ERR_OK)
			{
				if (lcsa_get_state() == LI_CAN_SLV_STATE_DOWNLOAD)
				{
					li_can_slv_reconnect_set_state(CAN_RECONNECT_STATE_OFF);
					// in case a download is pending switch off the reconnect feature after reset

					// ignore error in stopped mode
					(void)li_can_slv_set_node_mode(LI_CAN_SLV_MODE_STOPPED);

					if (err == LCSA_ERROR_OK)
					{
						err = lcsa_set_baudrate(can_config_get_baudrate_startup());
#ifdef LI_CAN_SLV_MAIN_MON
						// send baud rate to monitor cpu
						if (can_mainmon_type == CAN_MAINMON_TYPE_MAIN)
						{
							li_can_slv_port_sync_baudrate(can_config_get_baudrate_startup());
						}
#endif // #ifdef LI_CAN_SLV_MAIN_MON
					}

					li_can_slv_reconnect_download();

					err = li_can_slv_set_node_mode(LI_CAN_SLV_MODE_OPERATIONAL);

					if (err == LCSA_ERROR_OK)
					{
						lcsa_set_state(LI_CAN_SLV_STATE_RUNNING);
					}

				}
				else
				{
					li_can_slv_reconnect_set_state(CAN_RECONNECT_STATE_STARTUP);

					// ignore error in stopped mode
					(void)li_can_slv_set_node_mode(LI_CAN_SLV_MODE_STOPPED);

					if (err == LCSA_ERROR_OK)
					{
						err = lcsa_set_baudrate(can_config_get_baudrate_startup());
					}

					li_can_slv_reconnect_startup();

					// diagnostic connection to CAN bus
					err = li_can_slv_set_node_mode(LI_CAN_SLV_MODE_LISTEN_ONLY);

				}
			}
			else
			{
				(void)li_can_slv_set_node_mode(LI_CAN_SLV_MODE_STOPPED);

				if (err == LCSA_ERROR_OK)
				{
					err = lcsa_set_baudrate(can_config_get_baudrate_startup());
				}

				if (err == LCSA_ERROR_OK)
				{
					err = li_can_slv_set_node_mode(LI_CAN_SLV_MODE_OPERATIONAL);
					if (err == LCSA_ERROR_OK)
					{
						lcsa_set_state(LI_CAN_SLV_STATE_RUNNING);
					}
				}
			}
		}
#ifdef LI_CAN_SLV_MAIN_MON
	}
	else
	{
		// monitor cpu go online
		(void)li_can_slv_set_node_mode(LI_CAN_SLV_MODE_STOPPED);

		if (err == LCSA_ERROR_OK)
		{
			err = lcsa_set_baudrate(can_config_get_baudrate_startup());
		}

		if (err == LCSA_ERROR_OK)
		{
			err = li_can_slv_set_node_mode(LI_CAN_SLV_MODE_OPERATIONAL);
			if (err == LCSA_ERROR_OK)
			{
				lcsa_set_state(LI_CAN_SLV_STATE_RUNNING);
			}
		}
	}
#endif // #ifdef LI_CAN_SLV_MAIN_MON

#else // #ifdef LI_CAN_SLV_RECONNECT

#ifdef LI_CAN_SLV_MAIN_MON
	if (can_mainmon_type == CAN_MAINMON_TYPE_MAIN)
	{
		li_can_slv_port_sync_baudrate(lcsa_get_baudrate());
	}
#endif // #ifdef LI_CAN_SLV_MAIN_MON

	// ignore error in stopped mode
	(void)li_can_slv_set_node_mode(LI_CAN_SLV_MODE_STOPPED);

	if (err == LCSA_ERROR_OK)
	{
		err = lcsa_set_baudrate(can_config_get_baudrate_startup());
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT
		LI_CAN_SLV_DEBUG_PRINT("lcsa_start err:%08x\n", err);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT
	}
	if (err == LCSA_ERROR_OK)
	{
		err = li_can_slv_set_node_mode(LI_CAN_SLV_MODE_OPERATIONAL);
#ifdef LI_CAN_SLV_DEBUG_CAN_INIT
		LI_CAN_SLV_DEBUG_PRINT("lcsa_start node:%08x\n", err);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT
		if (err == LCSA_ERROR_OK)
		{
			lcsa_set_state(LI_CAN_SLV_STATE_RUNNING);
		}
	}

#endif // #ifdef LI_CAN_SLV_RECONNECT

	if (err == LCSA_ERROR_OK)
	{
#ifdef LI_CAN_SLV_DLOAD
#ifndef LI_CAN_SLV_NO_XLOAD_INFO
		li_can_slv_xload_info_get_mode(&xload_info_mode);
#ifdef LI_CAN_SLV_MAIN_MON
		if ((xload_info_mode == LI_CAN_SLV_XLOAD_INFO_MODE_DOWNLOAD_ACKN) && (can_mainmon_type == CAN_MAINMON_TYPE_MAIN))
#else
		if (xload_info_mode == LI_CAN_SLV_XLOAD_INFO_MODE_DOWNLOAD_ACKN)
#endif // #ifdef LI_CAN_SLV_MAIN_MON
		{
			li_can_slv_xload_info_get_dload_component(&xload_component);
			err = li_can_slv_dload_end_ackn(xload_component.module_nr);
			// set new mode ignore err here
			li_can_slv_xload_info_set_mode(LI_CAN_SLV_XLOAD_INFO_MODE_DOWNLOAD_RUNNING);
		}
#endif // #ifndef LI_CAN_SLV_NO_XLOAD_INFO
		lcsa_set_state(LI_CAN_SLV_STATE_RUNNING);

#ifdef LI_CAN_SLV_DEBUG_CAN_INIT
		LI_CAN_SLV_DEBUG_PRINT("start\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_INIT
#endif // #ifdef LI_CAN_SLV_DLOAD
	}

	return err;
}

lcsa_errorcode_t lcsa_add_module(const lcsa_module_config_t *module, lcsa_module_number_t module_nr, void *rx0, void *rx1, void *rx2, void *rx3, void *tx0, void *tx1, void *tx2, void *tx3)
{
	return can_config_add_module(module, module_nr, rx0, rx1, rx2, rx3, tx0, tx1, tx2, tx3);
}
#endif // #ifndef LI_CAN_SLV_BOOT

lcsa_bdr_t lcsa_get_baudrate(void)
{
	lcsa_bdr_t baudrate;

	(void) can_config_get_baudrate(&baudrate);
	return (baudrate);
}

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/** @} */
