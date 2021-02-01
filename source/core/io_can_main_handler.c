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
 * @file io_can_main_handler.c
 * @addtogroup can_driver_main
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/core/io_can_main_handler.h>
#include <li_can_slv/config/io_can_config.h>
#include <li_can_slv/core/io_can_main.h>

#include <li_can_slv/error/li_can_slv_error_types.h>
#include <li_can_slv/error/io_can_error.h>
#include <li_can_slv/error/io_can_errno.h>

#ifdef LI_CAN_SLV_ASYNC
#if defined(OUTER) || defined(OUTER_APP)
#include <li_can_slv/async/io_can_async.h>
#endif // #if defined(OUTER) || defined(OUTER_APP)
#endif // #ifdef LI_CAN_SLV_ASYNC

#ifdef LI_CAN_SLV_RECONNECT
#include <li_can_slv/core/io_can_reconnect.h>
#endif // #ifdef LI_CAN_SLV_RECONNECT

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
#if defined(OUTER) || defined(OUTER_APP)
/**
 * @param caller_idx
 * @return LI_CAN_SLV_ERR_OK
 */
uint8_t can_main_handler_tx(uint16_t caller_idx)
{
#if defined (LI_CAN_SLV_ASYNC)
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
#endif // #if defined (LI_CAN_SLV_ASYNC)

#ifdef LI_CAN_SLV_DEBUG_MAIN_TX_HANDLER
	LI_CAN_SLV_DEBUG_PRINT("c_m_hw_t_isr: %d\n", caller_idx);
#else
	caller_idx = caller_idx;
#endif // #ifdef LI_CAN_SLV_DEBUG_MAIN_TX_HANDLER

#ifdef LI_CAN_SLV_RECONNECT
	/*--------------------------------------------------------------------------*/
	/* only try to send if not in reconnect and new data available to send      */
	/*--------------------------------------------------------------------------*/
	if (li_can_slv_reconnect_get_state() != CAN_RECONNECT_STATE_ON)
	{
#endif // #ifdef LI_CAN_SLV_RECONNECT

#if defined(OUTER) || defined(OUTER_APP)
		/*----------------------------------------------------------------------*/
		/* check if any synchronous process transmit are valid                  */
		/*----------------------------------------------------------------------*/
		if (can_main_sync_process_tx_data_ctrl.send_cmd != CAN_MAIN_PROCESS_SYNC_SEND_DATA_ACK)
		{
			/* start transmission of new sync data set */
			can_main_sync_process_tx_data_ctrl.send_cmd = CAN_MAIN_PROCESS_SYNC_SEND_DATA_ACK;
			can_main_sync_process_tx_data_ctrl.send = can_main_sync_process_tx_data_ctrl.send_reg;
			can_main_sync_process_tx_data_ctrl.send_current = 0;
#ifdef LI_CAN_SLV_DEBUG_MAIN_TX_HANDLER
			LI_CAN_SLV_DEBUG_PRINT("-> new c_m_s_p_t_d_c_send: %08lx\n", can_main_sync_process_tx_data_ctrl.send_reg);
#endif // #ifdef LI_CAN_SLV_DEBUG_MAIN_TX_HANDLER
		}

		can_main_sync_send_process_data();

#endif // #if defined(OUTER) || defined(OUTER_APP)

#ifdef LI_CAN_SLV_ASYNC
#if defined(OUTER) || defined(OUTER_APP)
		/*----------------------------------------------------------------------*/
		/* check if any asynchronous data should be transmitted                 */
		/*----------------------------------------------------------------------*/
		if (can_async_data_tx_queue_state > 0)
		{
			err = can_async_handle_async_data_tx_queue();
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
			if (err != LI_CAN_SLV_ERR_OK)
			{
				error_syserr_send(err, ERR_LVL_INFO,  can_config_get_module_nr_main(), ERR_LVL_INFO);
			}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR
		}
#endif // #if defined(OUTER) || defined(OUTER_APP)
#endif // #ifdef LI_CAN_SLV_ASYNC

#ifdef LI_CAN_SLV_RECONNECT
	}
#endif // #ifdef LI_CAN_SLV_RECONNECT

	return 0;
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/** @} */

