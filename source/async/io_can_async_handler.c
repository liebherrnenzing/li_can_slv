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
 * @file io_can_async_handler.c
 * @addtogroup can_async
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "io_can_async_handler.h"
#include "io_can_config.h"

#include "io_can_async.h"
#include "io_can_main.h"

#include "io_can_hw.h"

#include "io_can_error.h"
#include "io_can_errno.h"

#ifdef LI_CAN_SLV_ASYNC_TUNNEL
#include "io_can_port.h"
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL

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
#ifndef LI_CAN_SLV_BOOT
#ifdef LI_CAN_SLV_ASYNC
uint8_t can_async_handler_rx(uint16_t msg_obj, uint8_t dlc, uint16_t canid, uint8_t *data)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	li_can_slv_module_nr_t module_nr;
	uint32_t rxi;

	dlc = dlc;
	rxi = msg_obj;

	module_nr = ((canid - CAN_CONFIG_ASYNC_CTRL_RX_SLAVE_ID) >> 2) + 1;

	if ((1L << rxi) & can_main_async_ctrl_objs_mask)
	{
#ifdef LI_CAN_SLV_DEBUG_CAN_ASYNC_HW_HANDLER
		LI_CAN_SLV_DEBUG_PRINT("\ncan_async_hw_handler_rx: msg_obj: %d", rxi);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_ASYNC_HW_HANDLER

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 1
		/*------------------------------------------------------------------------------*/
		/* asynchronous control data received for logical module 1 on obj_async_ctrl1   */
		/*------------------------------------------------------------------------------*/
		if ((rxi == (CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL1_RX)))
		{
			msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL1_RX;

#ifdef LI_CAN_SLV_DEBUG_CAN_ASYNC_HW_HANDLER
			LI_CAN_SLV_DEBUG_PRINT("CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL1_RX\n");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_ASYNC_HW_HANDLER

			err = can_async_send_data_to_async_ctrl_rx_queue(module_nr, &data[0]);

#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
			if (err != LI_CAN_SLV_ERR_OK)
			{
				error_syserr_send(err, ERR_LVL_INFO, can_config_get_module_nr_main(), ERR_LVL_INFO);
			}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR

		}
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 1

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 2
		/*------------------------------------------------------------------------------*/
		/* asynchronous control data received for logical module 2 on obj_async_ctrl2   */
		/*------------------------------------------------------------------------------*/
		if ((rxi == (CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL2_RX)))
		{
			msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL2_RX;
			err = can_async_send_data_to_async_ctrl_rx_queue(module_nr, &data[0]);
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
			if (err != LI_CAN_SLV_ERR_OK)
			{
				error_syserr_send(err, ERR_LVL_INFO, can_config_get_module_nr_main(), ERR_LVL_INFO);
			}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
		}
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 2

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 3
		/*------------------------------------------------------------------------------*/
		/* asynchronous control data received for logical module 3 on obj_async_ctrl3   */
		/*------------------------------------------------------------------------------*/
		if ((rxi == (CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL3_RX)))
		{
			msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL3_RX;
			err = can_async_send_data_to_async_ctrl_rx_queue(module_nr, &data[0]);
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
			if (err != LI_CAN_SLV_ERR_OK)
			{
				error_syserr_send(err, ERR_LVL_INFO, can_config_get_module_nr_main(), ERR_LVL_INFO);
			}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
		}
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 3

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 4
		/*------------------------------------------------------------------------------*/
		/* asynchronous control data received for logical module 4 on obj_async_ctrl4   */
		/*------------------------------------------------------------------------------*/
		if ((rxi == (CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL4_RX)))
		{
			msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL4_RX;
			err = can_async_send_data_to_async_ctrl_rx_queue(module_nr, &data[0]);
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
			if (err != LI_CAN_SLV_ERR_OK)
			{
				error_syserr_send(err, ERR_LVL_INFO, can_config_get_module_nr_main(), ERR_LVL_INFO);
			}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
		}
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 4

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 5
		/*------------------------------------------------------------------------------*/
		/* asynchronous control data received for logical module 5 on obj_async_ctrl5   */
		/*------------------------------------------------------------------------------*/
		if ((rxi == (CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL5_RX)))
		{
			msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL5_RX;
			err = can_async_send_data_to_async_ctrl_rx_queue(module_nr, &data[0]);
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
			if (err != LI_CAN_SLV_ERR_OK)
			{
				error_syserr_send(err, ERR_LVL_INFO, can_config_get_module_nr_main(), ERR_LVL_INFO);
			}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
		}
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 5

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 6
		/*------------------------------------------------------------------------------*/
		/* asynchronous control data received for logical module 6 on obj_async_ctrl6   */
		/*------------------------------------------------------------------------------*/
		if ((rxi == (CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL6_RX)))
		{
			msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL6_RX;
			err = can_async_send_data_to_async_ctrl_rx_queue(module_nr, &data[0]);
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
			if (err != LI_CAN_SLV_ERR_OK)
			{
				error_syserr_send(err, ERR_LVL_INFO, can_config_get_module_nr_main(), ERR_LVL_INFO);
			}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
		}
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 6

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 7
		/*------------------------------------------------------------------------------*/
		/* asynchronous control data received for logical module 7 on obj_async_ctrl7   */
		/*------------------------------------------------------------------------------*/
		if ((rxi == (CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL7_RX)))
		{
			msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL7_RX;
			err = can_async_send_data_to_async_ctrl_rx_queue(module_nr, &data[0]);
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
			if (err != LI_CAN_SLV_ERR_OK)
			{
				error_syserr_send(err, ERR_LVL_INFO, can_config_get_module_nr_main(), ERR_LVL_INFO);
			}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
		}
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 7

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 8
		/*------------------------------------------------------------------------------*/
		/* asynchronous control data received for logical module 8 on obj_async_ctrl8   */
		/*------------------------------------------------------------------------------*/
		if ((rxi == (CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL8_RX)))
		{
			msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL8_RX;
			err = can_async_send_data_to_async_ctrl_rx_queue(module_nr, &data[0]);
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
			if (err != LI_CAN_SLV_ERR_OK)
			{
				error_syserr_send(err, ERR_LVL_INFO, can_config_get_module_nr_main(), ERR_LVL_INFO);
			}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
		}
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 8

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 9
#warning LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES out of range!
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES
	}
	return 0;
}
#endif // #ifdef LI_CAN_SLV_ASYNC

#ifdef LI_CAN_SLV_ASYNC_TUNNEL
uint8_t can_async_handler_data_rx(uint16_t msg_obj, uint8_t dlc, uint16_t canid, uint8_t *data)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	uint32_t  rxi;

	rxi = msg_obj;
	canid = canid;

	/* asynchronous data received */
	if (rxi == CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_DATA_RX)
	{
		li_can_slv_port_async_tunnel_data_rx_handle((byte_t *)data, dlc);
	}

	return (err);
}
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL
#endif // #ifndef LI_CAN_SLV_BOOT

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/** @} */
