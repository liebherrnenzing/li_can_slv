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
 * \remarks <b>The CAN asynchronous protocol is use for: </b>
 *  - CAN download
 *  - Service and maintenance
 *
 *  - <b>initialization</b><br>
 *  - <b>CAN asynchronous rx queue</b><br>
 *  The can_async includes the whole management (data structures and functions) used for the CAN asynchronous RX queue.
 *  - <b>receive system messages</b><br>
 *  System messages with a DLC != 8 will be not accepted (no reaction). If a correct system message is received
 *  it will be processed by functions of this software module. Currently the following system messages are implemented:<br>
 *
 *  <table>
 *  	<tr> <td ><b>asynchronous control messages</b>	<td ><b>Byte1 (ID of the message)</b> </td></tr>
 *  	<tr> <td >download version request short     	<td >0x10 </td></tr>
 *  	<tr> <td >download version request long 1     	<td >0x11 </td></tr>
 *  	<tr> <td >download version request long 2     	<td >0x12 </td></tr>
 *  	<tr> <td >download start request 1    			<td >0x13 </td></tr>
 *  	<tr> <td >download start request 2    			<td >0x14 </td></tr>
 *  	<tr> <td >download data                  		<td >0x15 </td></tr>
 *  	<tr> <td >download data block end             	<td >0x16 </td></tr>
 *  	<tr> <td >download end                			<td >0x17 </td></tr
 *  </table>
 *
 * 	\arg asynchronous RX data\n
 * 	The data is stored in the CAN asynchronous RX queue and a request is set to process the data in the CAN
 * 	asynchronous RX queue.
 *
 *
 * 	Includes following queues with its management:
 * 	 - CAN asynchronous tx queue\n\n

 */

/**
 * @file io_can_async.c
 * @brief Asynchronous part of the Liebherr CAN protocol
 * @addtogroup can_async
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/async/io_can_async.h>
#ifdef LI_CAN_SLV_DLOAD
#include <li_can_slv/xload/li_can_slv_xload.h>
#endif // #ifdef LI_CAN_SLV_DLOAD

#include <li_can_slv/config/io_can_config.h>
#include <li_can_slv/config/li_can_slv_config_defines.h>

#include <li_can_slv/error/io_can_errno.h>

#include <li_can_slv/core/io_can_main.h>
#include <li_can_slv/core/io_can.h>

#include "io_can_async_hw.h"
#include "io_can_main_hw.h"

#include "io_can_port.h"

#ifdef LI_CAN_SLV_RECONNECT
#include <li_can_slv/core/io_can_reconnect.h>
#endif // #ifdef LI_CAN_SLV_RECONNECT

#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_debug.h"
#endif // #ifdef LI_CAN_SLV_DEBUG

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_ASYNC_TUNNEL
/* CAN asynchronous tunnel definitions */
#define CAN_ASYNC_TNL_PARTLYFILLES_TXOBJ_DELAY_MS	50 /*!< Time in msec that a partly filled TX asynchronous data object is delayed until it is transmitted to the TX queue */
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL

#if defined(OUTER) || defined(OUTER_APP)
#define CAN_ASYNC_DATA_TX_QUEUE_LENGTH		4 /**< */
#endif // #if defined(OUTER) || defined(OUTER_APP)

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_ASYNC
#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief define an element of the asynchronous data tx queue
 */
typedef struct
{
	uint16_t can_id; /**< identifier */
	byte_t dlc; /**< data length code */
	byte_t data[CAN_CONFIG_ASYNC_DATA_TX_DLC]; /**< data */
} can_async_data_tx_queue_t;
#endif	// #if defined(OUTER) || defined(OUTER_APP)
#endif // #ifdef LI_CAN_SLV_ASYNC

#ifdef LI_CAN_SLV_ASYNC_TUNNEL
/*!
 * \brief possible tunnel response types
 * \enum can_async_tnl_response_tag
 * \typedef enum can_async_tnl_response_tag can_async_tnl_response_t
 */
typedef enum can_async_tnl_response_tag
{
	CAN_ASYNC_TNL_OPEN_DENY, /*!< tunnel open deny */
	CAN_ASYNC_TNL_OPEN_ACK, /*!< tunnel open acknowledge */
	CAN_ASYNC_TNL_CLOSE_DENY, /*!< tunnel close deny */
	CAN_ASYNC_TNL_CLOSE_ACK, /*!< tunnel close acknowledge */
	CAN_ASYNC_TNL_LOGIN_ACK, /*!< tunnel login acknowledge */
	CAN_ASYNC_TNL_LOGIN_DENY, /*!< tunnel login deny */
	CAN_ASYNC_TNL_CLOSE /*!< tunnel close command */
} can_async_tnl_response_t;
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL

#ifdef LI_CAN_SLV_ASYNC_TUNNEL
/**
 * @brief temporary asynchronous data tx object (until the CAN message is full)
 * @struct can_async_tmpdata_tag
 * @typedef struct can_async_tmpdata_tag can_async_tmpdata_t
 */
typedef struct can_async_tmpdata_tag
{
	byte_t data[CAN_CONFIG_ASYNC_DATA_TX_DLC]; /**< data*/
	volatile uint16_t used; /**< used*/
#ifdef LI_CAN_SLV_ASYNC_TUNNEL_TIME_STAMP
	volatile uint32_t lastadd; /**< last add*/
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL_TIME_STAMP
} can_async_tmpdata_t;
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_ASYNC
#if defined(OUTER) || defined(OUTER_APP)
// asynchronous transmit data queue
static volatile uint16_t can_async_data_tx_queue_write = 0; /*!< write pointer of the asynchronous transmit data queue */
static volatile uint16_t can_async_data_tx_queue_read = 0; /*!< read pointer of the asynchronous transmit data queue */
volatile uint16_t can_async_data_tx_queue_state = 0; /*!< state of the asynchronous transmit data queue */
static volatile can_async_data_tx_queue_t can_async_data_tx_queue[CAN_ASYNC_DATA_TX_QUEUE_LENGTH]; /*!< asynchronous transmit data queue */
#endif // #if defined(OUTER) || defined(OUTER_APP)
#endif // #ifdef LI_CAN_SLV_ASYNC

#ifdef LI_CAN_SLV_ASYNC_TUNNEL
volatile can_async_tunnel_t can_async_tunnel =
{
	LI_CAN_SLV_ASYNC_TUNNEL_FREE,
	LI_CAN_SLV_ASYNC_TUNNEL_UNDEFINED,
	LI_CAN_SLV_CONFIG_DEF_MOD_NR1,
	LI_CAN_SLV_CONFIG_DEF_MOD_NR1,
	LI_CAN_SLV_CONFIG_DEF_MOD_NR1,
	0,
	0
}; /**< asynchronous tunnel information*/
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#if defined(OUTER) || defined(OUTER_APP)
#ifdef LI_CAN_SLV_ASYNC_TUNNEL
static li_can_slv_errorcode_t can_async_tnl_ctl_response(can_async_tnl_response_t response, li_can_slv_module_nr_t modnr_prim, li_can_slv_module_nr_t modnr_own, uint16_t data);
static li_can_slv_errorcode_t can_async_xoff(void);
static li_can_slv_errorcode_t can_async_xon(void);
static li_can_slv_errorcode_t can_async_tnl_reset_port(void);
static li_can_slv_errorcode_t can_async_tunnel_putc(char_t dat);
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL
#endif	// #if defined(OUTER) || defined(OUTER_APP)

#if defined (QTP) && defined(LI_CAN_SLV_RECONNECT)
static li_can_slv_errorcode_t can_async_qtp_can_test_send_data(li_can_slv_module_nr_t module_nr, byte_t *src);
#endif // #if defined (QTP) && defined(LI_CAN_SLV_RECONNECT)

#ifdef LI_CAN_SLV_ASYNC_TEST_MESSAGE
void li_can_slv_async_send_test_message_ack(void);
#endif // #ifdef LI_CAN_SLV_ASYNC_TEST_MESSAGE

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/
#ifdef CAN_ASYNC_CTRL_RX_QUEUE
// asynchronous message receive queue
static volatile uint16_t can_async_rx_queue_write = 0; /**< write pointer of the queue */
volatile uint16_t can_async_rx_queue_read = 0; /**< read pointer of the queue */
volatile uint16_t can_async_rx_queue_state = 0; /**< state of the asynchronous message receive queue */
volatile can_async_rx_queue_t can_async_ctrl_rx_queue[CAN_ASYNC_CTRL_RX_QUEUE_LENGTH]; /**< asynchronous message receive queue */
#endif // #ifdef CAN_ASYNC_CTRL_RX_QUEUE

#ifdef LI_CAN_SLV_ASYNC_TUNNEL
#ifdef LI_CAN_SLV_ASYNC_TUNNEL_TIME_STAMP
static can_async_tmpdata_t can_async_tunnel_tmpTXobj =
{
	{0},
	0,
	0
}; /**< temporary asynchronous data object */
#else // #ifdef LI_CAN_SLV_ASYNC_TUNNEL_TIME_STAMP
static can_async_tmpdata_t can_async_tunnel_tmpTXobj =
{
	{0},
	0
}; /**< temporary asynchronous data object */
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL_TIME_STAMP
static volatile uint16_t can_async_tnl_lock = 0; /**< tunnel lock/unlock */
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL

#ifdef LI_CAN_SLV_ASYNC_TUNNEL
typedef li_can_slv_errorcode_t (*can_async_tunnel_open_handle_funcp_t)(byte_t connection_type);
static can_async_tunnel_open_handle_funcp_t can_async_tunnel_open_handle_funcp = NULL;
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
#ifdef CAN_ASYNC_CTRL_RX_QUEUE
/****************************************************************************/
/*!
 * \brief initializes the asynchronous can receive task
 * \return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_async_init(void)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	err = can_async_hw_init();

	return (err);
}
#endif // #ifdef CAN_ASYNC_CTRL_RX_QUEUE


#ifdef CAN_ASYNC_CTRL_RX_QUEUE
/****************************************************************************/
/*!
 * \brief add data to the asynchronous ctrl RX queue
 * \param module_nr module number
 * \param[in] src source pointer of data
 * \return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_async_send_data_to_async_ctrl_rx_queue(li_can_slv_module_nr_t module_nr, byte_t const *src)
{
	li_can_slv_errorcode_t err;
	uint16_t table_pos, module_found = 0, table_end = 0;
	can_config_module_silent_t module_silent;

	/* find module position in configuration table */
	err = can_config_module_nr_valid(module_nr, &table_pos, &module_silent, &module_found);
	if (!module_found)
	{
		return LI_CAN_SLV_ERR_OK;
	}

	if (module_silent == LI_CAN_SLV_CONFIG_MODULE_STATE_SILENT)
	{
		return LI_CAN_SLV_ERR_OK;
	}

	if (!(can_async_rx_queue_state < CAN_ASYNC_CTRL_RX_QUEUE_LENGTH))
	{
		return (ERR_MSG_CAN_ASYNC_RX_QUEUE_OVERFLOW);
	}

	can_async_ctrl_rx_queue[can_async_rx_queue_write].module_nr = module_nr;

#ifdef LI_CAN_SLV_BOOT
	//use for code size optimization following code
	can_port_memory_cpy(&can_async_ctrl_rx_queue[can_async_rx_queue_write].data[0], src, CAN_CONFIG_ASYNC_MSG_DLC);
#else // #ifdef LI_CAN_SLV_BOOT
	//use for speed optimization following code - no standard functions
	can_async_ctrl_rx_queue[can_async_rx_queue_write].data[0] = *src;
	src++;
	can_async_ctrl_rx_queue[can_async_rx_queue_write].data[1] = *src;
	src++;
	can_async_ctrl_rx_queue[can_async_rx_queue_write].data[2] = *src;
	src++;
	can_async_ctrl_rx_queue[can_async_rx_queue_write].data[3] = *src;
	src++;
	can_async_ctrl_rx_queue[can_async_rx_queue_write].data[4] = *src;
	src++;
	can_async_ctrl_rx_queue[can_async_rx_queue_write].data[5] = *src;
	src++;
	can_async_ctrl_rx_queue[can_async_rx_queue_write].data[6] = *src;
	src++;
	can_async_ctrl_rx_queue[can_async_rx_queue_write].data[7] = *src;
#endif // #ifdef LI_CAN_SLV_BOOT
	if (can_async_rx_queue_write < (CAN_ASYNC_CTRL_RX_QUEUE_LENGTH - 1))
	{
		can_async_rx_queue_write++;
	}
	else
	{
		can_async_rx_queue_write = 0;
	}

	can_async_rx_queue_state++;

	// trigger queue handling
	can_port_trigger_can_async_ctrl_rx_queue();

	return (LI_CAN_SLV_ERR_OK);
}
#else // #ifdef CAN_ASYNC_CTRL_RX_QUEUE

/**
 * @brief send data to asynchronous control RX queue
 * @param module_nr: given module number
 * @param src: pointer to the received data
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_async_send_data_to_async_ctrl_rx_queue(li_can_slv_module_nr_t module_nr, byte_t const *src)
{
	li_can_slv_errorcode_t err;
	err = can_async_rx(module_nr, src);
	return (err);
}
#endif // #ifdef CAN_ASYNC_CTRL_RX_QUEUE

/**
 * @brief
 * @param module_nr: given module number
 * @param data: given data of the module
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_async_rx(li_can_slv_module_nr_t module_nr, byte_t const *data)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t module_found = 0;
	can_config_module_silent_t module_silent;
#ifndef LI_CAN_SLV_BOOT
	uint16_t table_pos;
#endif // #ifndef LI_CAN_SLV_BOOT
#ifdef LI_CAN_SLV_BOOT
	li_can_slv_mode_t mode;
#endif // #ifdef LI_CAN_SLV_BOOT

#if defined(LI_CAN_SLV_ASYNC_TUNNEL)
#if defined(OUTER) || defined(OUTER_APP)
	li_can_slv_module_nr_t module_nr_tunnel;
#endif // #if defined(OUTER) || defined(OUTER_APP)
#endif // #if defined(LI_CAN_SLV_ASYNC_TUNNEL)

#ifdef LI_CAN_SLV_MAIN_MON
	if (can_mainmon_type == CAN_MAINMON_TYPE_MON)
	{
		return (LI_CAN_SLV_ERR_OK);
	}
#endif // #ifdef LI_CAN_SLV_MAIN_MON

#ifdef LI_CAN_SLV_BOOT
	err = can_config_module_nr_valid(module_nr, &module_silent, &module_found);
#else // #ifdef LI_CAN_SLV_BOOT
	err = can_config_module_nr_valid(module_nr, &table_pos, &module_silent, &module_found);
#endif // #ifdef LI_CAN_SLV_BOOT

	if (!module_found)
	{
		return LI_CAN_SLV_ERR_OK;
	}

	if (module_silent == LI_CAN_SLV_CONFIG_MODULE_STATE_SILENT)
	{
		return LI_CAN_SLV_ERR_OK;
	}

#ifdef LI_CAN_SLV_BOOT
	mode = li_can_slv_get_mode();
	if ((LI_CAN_SLV_MODE_LISTEN_ONLY == mode) && (data[0] != CAN_ASYNC_DL_VERSION_REQUEST_SHORT) && (data[0] != CAN_ASYNC_DL_VERSION_REQUEST_LONG1) && (data[0] != CAN_ASYNC_DL_VERSION_REQUEST_LONG2))
	{
		return LI_CAN_SLV_ERR_OK;
	}
#endif // #ifdef LI_CAN_SLV_BOOT

	switch (data[0])
	{
#ifdef LI_CAN_SLV_DLOAD
		case CAN_ASYNC_DL_VERSION_REQUEST_SHORT:
			err = li_can_slv_dload_version_request_short(module_nr, data);
			break;

		case CAN_ASYNC_DL_VERSION_REQUEST_LONG1:
			err = li_can_slv_dload_version_request_long1(module_nr, data);
			break;

		case CAN_ASYNC_DL_VERSION_REQUEST_LONG2:
			err = li_can_slv_dload_version_request_long2(module_nr, data);
			break;

		case CAN_ASYNC_DL_START_REQUEST1:
			err = li_can_slv_dload_start_request1(module_nr, data);
			break;

		case CAN_ASYNC_DL_START_REQUEST2:
			err = li_can_slv_dload_start_request2(module_nr, data);
			break;

		case CAN_ASYNC_DL_DATA:
			err = li_can_slv_dload_data(module_nr, data);
			break;

		case CAN_ASYNC_DL_DATA_BLOCK_END:
			err = li_can_slv_dload_data_block_end(module_nr, data);
			break;

		case CAN_ASYNC_DL_END:
			err = li_can_slv_dload_end(module_nr);
			break;
#endif // #ifdef LI_CAN_SLV_DLOAD

#ifdef LI_CAN_SLV_ASYNC_TUNNEL
#if defined(OUTER) || defined(OUTER_APP)
		case CAN_ASYNC_XON:
			//check if the asynchronous tunnel is open; if yes, resume transmission
			if ((can_async_tunnel.state == LI_CAN_SLV_ASYNC_TUNNEL_ACTIVE) && (can_async_tunnel.prim_modnr == (li_can_slv_module_nr_t) data[1]))
			{
				li_can_slv_port_async_tunnel_xon_rx_handle();
			}
			break;

		case CAN_ASYNC_XOFF:
			//check if the asynchronous tunnel is open; if yes, stop transmission
			if ((can_async_tunnel.state == LI_CAN_SLV_ASYNC_TUNNEL_ACTIVE) && (can_async_tunnel.prim_modnr == (li_can_slv_module_nr_t)data[1]))
			{
				li_can_slv_port_async_tunnel_xoff_rx_handle();
			}
			break;

		case LI_CAN_SLV_ASYNC_TUNNEL_OPEN:
			//handle tunnel open request
			//store primary module number
			module_nr_tunnel = (li_can_slv_module_nr_t)data[1];
			//check if channel is free
			if (can_async_tunnel.state == LI_CAN_SLV_ASYNC_TUNNEL_FREE)
			{
				err = li_can_slv_port_async_tunnel_open_handle(&can_async_tunnel_putc, &can_async_xon, &can_async_xoff);

				//open channel
				if (err == LI_CAN_SLV_ERR_OK)
				{
					can_async_tunnel.mod_own = module_nr;
					can_async_tunnel.prim_modnr = module_nr_tunnel;
					can_async_tunnel.sec_modnr = can_async_tunnel.mod_own;
					can_async_tunnel.state = LI_CAN_SLV_ASYNC_TUNNEL_ACTIVE;
					can_async_tunnel.connection_type = data[4];
					can_async_tunnel.async_data_delay = (((uint16_t)data[5]) << 8) + (uint16_t)(data[6]);

					switch (can_async_tunnel.connection_type)
					{
						case CAN_ASYNC_CONNECTION_TYPE_DEFAULT:
							can_main_define_msg_obj(CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_DATA_RX, ((can_async_tunnel.mod_own - 1) * CAN_CONFIG_NR_OF_MODULE_OBJS + CAN_CONFIG_ASYNC_DATA_RX_SLAVE_ID), CAN_CONFIG_ACCEPTANCE_ONE_ID, CAN_CONFIG_ASYNC_DATA_RX_DLC, CAN_CONFIG_DIR_RX, CAN_MAIN_ASYNC_SERVICE_ID_RX, CAN_OBJECT_NOT_SYNC);
							can_async_tnl_ctl_response(CAN_ASYNC_TNL_OPEN_ACK, module_nr_tunnel, module_nr, 0);
							break;

#ifdef SPS_MONITOR_CONNECTION_TYPE_LP
						case CAN_ASYNC_CONNECTION_TYPE_SPS_MONITOR_LP:
#endif // #ifdef SPS_MONITOR_CONNECTION_TYPE_LP
							can_main_define_msg_obj(CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_DATA_RX, ((can_async_tunnel.mod_own - 1) * CAN_CONFIG_NR_OF_MODULE_OBJS + CAN_CONFIG_ASYNC_DATA_RX_SLAVE_ID), CAN_CONFIG_ACCEPTANCE_ONE_ID, CAN_CONFIG_ASYNC_DATA_RX_DLC, CAN_CONFIG_DIR_RX, CAN_MAIN_ASYNC_SERVICE_ID_RX, CAN_OBJECT_NOT_SYNC);
							can_async_tnl_ctl_response(CAN_ASYNC_TNL_OPEN_ACK, module_nr_tunnel, module_nr, 0);
							break;

						default:
							/* call download handle */
							if (NULL != can_async_tunnel_open_handle_funcp)
							{
								err = can_async_tunnel_open_handle_funcp(can_async_tunnel.connection_type);
								if (err != LI_CAN_SLV_ERR_OK)
								{
									can_async_tnl_ctl_response(CAN_ASYNC_TNL_OPEN_DENY, module_nr_tunnel, module_nr, err);

								}
								//								can_async_tnl_ctl_response(CAN_ASYNC_TNL_OPEN_DENY, module_nr_tunnel, module_nr, ERR_MSG_CAN_ASYNC_UNDEFINED_CONNECTION_TYPE);
							}
							else
							{
								/* response tunnel deny */
								can_async_tnl_ctl_response(CAN_ASYNC_TNL_OPEN_DENY, module_nr_tunnel, module_nr, ERR_MSG_CAN_ASYNC_TUNNEL_HNDL_NOT_DEFINED);
							}
							break;
					}

#ifdef SPS_MONITOR_CONNECTION_TYPE_LP
					if (can_async_tunnel.connection_type == CAN_ASYNC_CONNECTION_TYPE_SPS_MONITOR_LP)
					{
						li_can_slv_start_sps_monitor_handle();
					}
#endif // #ifdef SPS_MONITOR_CONNECTION_TYPE_LP
				}
				else
				{
					can_async_tnl_ctl_response(CAN_ASYNC_TNL_OPEN_DENY, module_nr_tunnel, module_nr, err);
				}
			}
			else
			{
				//not possible because already used
				can_async_tnl_ctl_response(CAN_ASYNC_TNL_OPEN_DENY, module_nr_tunnel, module_nr, ERR_MSG_CAN_ASYNC_TNL_ALREADY_USED);
			}
			break;

		case LI_CAN_SLV_ASYNC_TUNNEL_CLOSE:
			//handle tunnel close request and store primary module number
			module_nr_tunnel = (li_can_slv_module_nr_t)data[1];
			//don't check if channel is used, simply close the channel

			err = li_can_slv_port_async_tunnel_free_handle();

			if (err == LI_CAN_SLV_ERR_OK)
			{
				can_async_tunnel.mod_own = LI_CAN_SLV_CONFIG_DEF_MOD_NR1;
				can_async_tunnel.prim_modnr = LI_CAN_SLV_CONFIG_DEF_MOD_NR1;
				can_async_tunnel.sec_modnr = LI_CAN_SLV_CONFIG_DEF_MOD_NR1;
				can_async_tunnel.state = LI_CAN_SLV_ASYNC_TUNNEL_FREE;
				can_async_tnl_ctl_response(CAN_ASYNC_TNL_CLOSE_ACK, module_nr_tunnel, module_nr, 0);
			}
			else
			{
				can_async_tnl_ctl_response(CAN_ASYNC_TNL_CLOSE_DENY, module_nr_tunnel, module_nr, err);
			}
			break;

		case LI_CAN_SLV_ASYNC_TUNNEL_RESET_PORT:
			can_async_tnl_reset_port(); //reset the com channel
			break;
#endif // #if defined(OUTER) || defined(OUTER_APP)

		case LI_CAN_SLV_ASYNC_TUNNEL_OPEN_ACK:
			break;

		case LI_CAN_SLV_ASYNC_TUNNEL_CLOSE_ACK:
#if defined(OUTER) || defined(OUTER_APP)
			if ((can_async_tunnel.state == LI_CAN_SLV_ASYNC_TUNNEL_SHUTDOWN) && (can_async_tunnel.prim_modnr == data[1]) && (can_async_tunnel.mod_own == module_nr))
			{
				//now we received a matching close acknowledge
				//free up CAN channel completely
				err = li_can_slv_port_async_tunnel_free_handle();

				can_async_tunnel.mod_own = LI_CAN_SLV_CONFIG_DEF_MOD_NR1;
				can_async_tunnel.prim_modnr = LI_CAN_SLV_CONFIG_DEF_MOD_NR1;
				can_async_tunnel.sec_modnr = LI_CAN_SLV_CONFIG_DEF_MOD_NR1;
				can_async_tunnel.state = LI_CAN_SLV_ASYNC_TUNNEL_FREE;
			}
#endif // #if defined(OUTER) || defined(OUTER_APP)
			break;

		case LI_CAN_SLV_ASYNC_TUNNEL_OPEN_DENY:
			break;

		case LI_CAN_SLV_ASYNC_TUNNEL_CLOSE_DENY:
#if defined(OUTER) || defined(OUTER_APP)
			if ((can_async_tunnel.state == LI_CAN_SLV_ASYNC_TUNNEL_SHUTDOWN) &&
			        (can_async_tunnel.prim_modnr == data[1]) &&
			        (can_async_tunnel.mod_own == module_nr))
			{
				//now we received a matching close deny
				//reset tunnel to working state as nothing happened
				can_async_tunnel.state = LI_CAN_SLV_ASYNC_TUNNEL_ACTIVE;
			}
#endif // #if defined(OUTER) || defined(OUTER_APP)
			break;
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL

#ifdef LI_CAN_SLV_DLOAD
		case CAN_ASYNC_DL_VERSION_ANSWER:
			break;

		case CAN_ASYNC_DL_START_ACKN:
			break;
#endif // #ifdef LI_CAN_SLV_DLOAD

#ifdef LI_CAN_SLV_ULOAD
		case CAN_ASYNC_DL_BLOCK_ACKN:
			err = li_can_slv_uload_block_ackn(module_nr, data);
			break;

		case CAN_ASYNC_DL_END_ACKN:
			err = li_can_slv_uload_end_ackn(module_nr, data);
			break;

		case CAN_ASYNC_DL_TERMINATION:
			err = li_can_slv_uload_termination(module_nr, data, 0);
			break;

		case CAN_ASYNC_UL_INFO_REQUEST_SHORT:
			err = li_can_slv_uload_info_request_short(module_nr, data);
			break;

		case CAN_ASYNC_UL_START_REQUEST1:
			err = li_can_slv_uload_start_request1(module_nr, data);
			break;

		case CAN_ASYNC_UL_START_REQUEST2:
			err = li_can_slv_uload_start_request2(module_nr, data);
			break;
#endif // #ifdef LI_CAN_SLV_ULOAD

#if defined(QTP) && defined (LI_CAN_SLV_RECONNECT)
		case CAN_ASYNC_QTP_CAN_TEST_DEACTIVATE_RECONNECT:
			li_can_slv_reconnect_set_state(CAN_RECONNECT_STATE_DEACTIVATED);
			break;
#endif // #if defined(QTP) && defined (LI_CAN_SLV_RECONNECT)

#ifdef LIDIS
		case CAN_ASYNC_DIAGNOSTICS_LIDIS:
			err = lidis_get_msg(module_nr, data);
			break;
#endif // #ifdef LIDIS

#if (defined(QTP) && defined (LI_CAN_SLV_RECONNECT)) || defined (LI_CAN_SLV_ASYNC_TEST_MESSAGE)
		case CAN_ASYNC_TEST_MESSAGE:
#if defined(QTP) && defined (LI_CAN_SLV_RECONNECT)
			li_can_slv_reconnect_set_state(CAN_RECONNECT_STATE_DEACTIVATED);
			err = can_async_qtp_can_test_send_data(module_nr, data);
#endif // #if defined(QTP) && defined (LI_CAN_SLV_RECONNECT)

#ifdef LI_CAN_SLV_ASYNC_TEST_MESSAGE
			li_can_slv_async_send_test_message_ack();
#endif // #ifdef LI_CAN_SLV_ASYNC_TEST_MESSAGE
			break;
#endif // #if (defined(QTP) && defined (LI_CAN_SLV_RECONNECT)) || defined (LI_CAN_SLV_ASYNC_TEST_MESSAGE)

		default:
			err = ERR_MSG_CAN_ASYNC_UNKNOWN_COMMAND;
			break;
	}
	return (err);
}

/*!
 * \param module_nr module number
 * \param type asynchronous ctrl transmit
 * \param[in] src source pointer of data
 * \return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_async_send_data_to_async_ctrl_tx_queue(li_can_slv_module_nr_t module_nr, can_async_ctrl_tx_type_t type, const byte_t *src)
{
	word_t can_id;

	/**
	 * @todo change implementation of can_id with error handling
	 */
	// calculation of CAN identifier
	switch (type)
	{
		case CAN_ASYNC_CTRL_TX_TYPE_SLAVE:
			can_id = CAN_CONFIG_ASYNC_CTRL_TX_SLAVE_ID + ((module_nr - 1) << 2);
			break;

		case CAN_ASYNC_CTRL_TX_TYPE_MASTER:
			can_id = CAN_CONFIG_ASYNC_CTRL_TX_MASTER_ID + ((module_nr - 1) << 2);
			break;

		default:
			can_id = CAN_CONFIG_ID_NULL;
			break;
	}

	(void) can_main_hw_send_msg(can_id, CAN_CONFIG_ASYNC_CTRL_TX_DLC, src);

	return (LI_CAN_SLV_ERR_OK);
}

#if defined(OUTER) || defined(OUTER_APP)
/*!
 * \param module_nr module number
 * \param type asynchronous ctrl transmit type
 * \param dlc data length code of object
 * \param[in] src source pointer of data
 * \return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_async_send_data_to_async_data_tx_queue(li_can_slv_module_nr_t module_nr, can_async_data_tx_type_t type, byte_t dlc, const byte_t *src)
{
	if (!(can_async_data_tx_queue_state < CAN_ASYNC_DATA_TX_QUEUE_LENGTH))
	{
		return (ERR_MSG_CAN_ASYNC_DATA_TX_QUEUE_OVERFLOW);
	}

	if (dlc > CAN_CONFIG_ASYNC_DATA_TX_DLC)
	{
		return (ERR_MSG_CAN_ASYNC_DATA_TX_QUEUE_DLC);
	}

	can_port_memory_cpy((can_async_data_tx_queue_t *)&can_async_data_tx_queue[can_async_data_tx_queue_write].data[0], src, dlc);

	can_async_data_tx_queue[can_async_data_tx_queue_write].dlc = dlc;

	switch (type)
	{
		case CAN_ASYNC_DATA_TX_TYPE_SLAVE:
			can_async_data_tx_queue[can_async_data_tx_queue_write].can_id = CAN_CONFIG_ASYNC_DATA_TX_SLAVE_ID + ((module_nr - 1) << 2);
			break;

		case CAN_ASYNC_DATA_TX_TYPE_MASTER:
			return (ERR_MSG_CAN_NOT_IMPLEMENTED);
			break;

		default:
			break;
	}

	if (can_async_data_tx_queue_write < (CAN_ASYNC_DATA_TX_QUEUE_LENGTH - 1))
	{
		can_async_data_tx_queue_write++;
	}
	else
	{
		can_async_data_tx_queue_write = 0;
	}

	can_async_data_tx_queue_state++;

	// trigger queue handling
	can_port_trigger_can_main_async_data_tx_queue();

	return (LI_CAN_SLV_ERR_OK);
}
#endif // #if defined(OUTER) || defined(OUTER_APP)


#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief can_main_hndl_queue_async_data_tx handles the asynchronous transmit data queue
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_async_handle_async_data_tx_queue(void)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if (can_async_data_tx_queue_state > 0)
	{
		/**
		 * @todo fix this volatile problem here
		 */
		err = can_main_hw_send_msg(can_async_data_tx_queue[can_async_data_tx_queue_read].can_id, (uint16_t)(can_async_data_tx_queue[can_async_data_tx_queue_read].dlc), &can_async_data_tx_queue[can_async_data_tx_queue_read].data[0]);

		if (err == LI_CAN_SLV_ERR_OK)
		{
			// service read pointer
			if (can_async_data_tx_queue_read < (CAN_ASYNC_DATA_TX_QUEUE_LENGTH - 1))
			{
				can_async_data_tx_queue_read++;
			}
			else
			{
				can_async_data_tx_queue_read = 0;
			}
			can_async_data_tx_queue_state--;
		}
	}
	return (err);
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#if defined(OUTER) || defined(OUTER_APP)
#ifdef LI_CAN_SLV_ASYNC_TUNNEL
/**
 * @brief sends out an control message for asynchronous tunnel protocol
 * @remarks Handles all control message sending occurrences for the asynchronous CAN tunnel
 * @param[in] response type of response to prepare
 * @param[in] modnr_prim number of primary module that initiated the original request
 * @param[in] modnr_own number of own logical module that should answer
 * @param data data of the module
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t can_async_tnl_ctl_response(can_async_tnl_response_t response, li_can_slv_module_nr_t modnr_prim,
        li_can_slv_module_nr_t modnr_own, uint16_t data)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	byte_t snd_data[CAN_CONFIG_ASYNC_CTRL_TX_DLC] = {0};
	uint32_t datal;

	switch (response)
	{
		case CAN_ASYNC_TNL_OPEN_ACK:
			snd_data[0] = LI_CAN_SLV_ASYNC_TUNNEL_OPEN_ACK;
			snd_data[1] = (byte_t) modnr_prim;
			snd_data[5] = (byte_t)(can_async_tunnel.async_data_delay >> 8);
			snd_data[6] = (byte_t)(can_async_tunnel.async_data_delay & 0x00FF);
			break;
		case CAN_ASYNC_TNL_CLOSE:
			snd_data[0] = LI_CAN_SLV_ASYNC_TUNNEL_CLOSE;
			snd_data[1] = (byte_t) modnr_prim;
			break;
		case CAN_ASYNC_TNL_CLOSE_ACK:
			snd_data[0] = LI_CAN_SLV_ASYNC_TUNNEL_CLOSE_ACK;
			snd_data[1] = (byte_t) modnr_prim;
			break;
		case CAN_ASYNC_TNL_LOGIN_ACK:
			snd_data[0] = LI_CAN_SLV_ASYNC_TUNNEL_LOGIN_ACK;
			snd_data[1] = (byte_t) modnr_prim;
			//login in ID
			snd_data[2] = (byte_t)(data >> 8);
			snd_data[3] = (byte_t)(data & 0x00FF);
			break;
		case CAN_ASYNC_TNL_OPEN_DENY:
			snd_data[0] = LI_CAN_SLV_ASYNC_TUNNEL_OPEN_DENY;
			snd_data[1] = (byte_t) modnr_prim;
			datal = (uint32_t) data;
			snd_data[4] = (byte_t)(datal >> 24);
			snd_data[5] = (byte_t)((datal >> 16) & 0x000000FF);
			snd_data[6] = (byte_t)((datal >> 8) & 0x000000FF);
			snd_data[7] = (byte_t)(datal & 0x000000FF);
			break;
		case CAN_ASYNC_TNL_CLOSE_DENY:
			snd_data[0] = LI_CAN_SLV_ASYNC_TUNNEL_CLOSE_DENY;
			snd_data[1] = (byte_t) modnr_prim;
			//add error number
			datal = (uint32_t) data;
			snd_data[4] = (byte_t)(datal >> 24);
			snd_data[5] = (byte_t)((datal >> 16) & 0x000000FF);
			snd_data[6] = (byte_t)((datal >> 8) & 0x000000FF);
			snd_data[7] = (byte_t)(datal & 0x000000FF);
			break;
		case CAN_ASYNC_TNL_LOGIN_DENY:
			snd_data[0] = LI_CAN_SLV_ASYNC_TUNNEL_LOGIN_DENY;
			snd_data[1] = (byte_t) modnr_prim;
			//add error number
			datal = (uint32_t) data;
			snd_data[4] = (byte_t)(datal >> 24);
			snd_data[5] = (byte_t)((datal >> 16) & 0x000000FF);
			snd_data[6] = (byte_t)((datal >> 8) & 0x000000FF);
			snd_data[7] = (byte_t)(datal & 0x000000FF);
			break;
		default:
			err = ERR_MSG_CAN_ASYNC_TNL_INVALID_COMMAND;
			break;
	}
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_async_send_data_to_async_ctrl_tx_queue(modnr_own, CAN_ASYNC_CTRL_TX_TYPE_SLAVE, &snd_data[0]);
	}
	return (err);
}
#endif //#ifdef LI_CAN_SLV_ASYNC_TUNNEL


#ifdef LI_CAN_SLV_ASYNC_TUNNEL
/**
 * @brief empties the serial RX queue and resets the XON/XOFF state to XON
 * @remarks used to make the flow going on again on a hanging tunnel
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t can_async_tnl_reset_port(void)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if (can_async_tunnel.state == LI_CAN_SLV_ASYNC_TUNNEL_ACTIVE)
	{
		err = li_can_slv_port_async_tunnel_clear_rx_buffer_handle();

		if (err == LI_CAN_SLV_ERR_OK)
		{
			li_can_slv_port_async_tunnel_xon_rx_handle();
		}
	}
	else
	{
		err = ERR_MSG_CAN_ASYNC_TNL_WRONG_STATE;
	}
	return (err);
}


/**
 * @brief can_async_xon
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t can_async_xon(void)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	byte_t snd_data[CAN_CONFIG_ASYNC_CTRL_TX_DLC] = {0};

	if (can_async_tunnel.state != LI_CAN_SLV_ASYNC_TUNNEL_ACTIVE)
	{
		err = ERR_MSG_CAN_ASYNC_TNL_WRONG_STATE;
	}
	else
	{
		snd_data[0] = CAN_ASYNC_XON;
		snd_data[1] = (byte_t) can_async_tunnel.prim_modnr;
		err = can_async_send_data_to_async_ctrl_tx_queue(can_async_tunnel.mod_own, CAN_ASYNC_CTRL_TX_TYPE_SLAVE, &snd_data[0]);
	}
	return (err);
}


/**
 * @brief can_async_xoff
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t can_async_xoff(void)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	byte_t snd_data[CAN_CONFIG_ASYNC_CTRL_TX_DLC] = {0};

	if (can_async_tunnel.state != LI_CAN_SLV_ASYNC_TUNNEL_ACTIVE)
	{
		err = ERR_MSG_CAN_ASYNC_TNL_WRONG_STATE;
	}
	else
	{
		snd_data[0] = CAN_ASYNC_XOFF;
		snd_data[1] = (byte_t) can_async_tunnel.prim_modnr;
		err = can_async_send_data_to_async_ctrl_tx_queue(can_async_tunnel.mod_own, CAN_ASYNC_CTRL_TX_TYPE_SLAVE, &snd_data[0]);
	}
	return (err);
}


/**
 * @brief adds data bytes from serial stream manager to the temporary tx object
 * @remarks issues a send data bytes command when an object is full
 * @param[in] dat character to add to TX queue
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t can_async_tunnel_putc(char_t dat)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	can_async_tnl_lock++;

	if (can_async_tnl_lock == 1)
	{
		//check if tunnel is active
		if (can_async_tunnel.state == LI_CAN_SLV_ASYNC_TUNNEL_ACTIVE)
		{
			can_async_tunnel_tmpTXobj.data[can_async_tunnel_tmpTXobj.used] = dat;
			can_async_tunnel_tmpTXobj.used++;
#ifdef LI_CAN_SLV_ASYNC_TUNNEL_TIME_STAMP
			can_async_tunnel_tmpTXobj.lastadd = can_port_get_system_ticks();
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL_TIME_STAMP
			if (can_async_tunnel_tmpTXobj.used == sizeof(can_async_tunnel_tmpTXobj.data))
			{
				//add the data to the real asynchronous data TX queue
				do
				{
					err = can_async_send_data_to_async_data_tx_queue(can_async_tunnel.mod_own, CAN_ASYNC_DATA_TX_TYPE_SLAVE, can_async_tunnel_tmpTXobj.used, can_async_tunnel_tmpTXobj.data);
				}
				while (err != LI_CAN_SLV_ERR_OK);

				if (err == LI_CAN_SLV_ERR_OK)
				{
					//signal that the temporary object is free if the adding was successful
					can_async_tunnel_tmpTXobj.used = 0;
				}
			}
		}
		else
		{
			err = ERR_MSG_CAN_ASYNC_TNL_WRONG_STATE;
		}
	}
	else
	{
		err = ERR_MSG_CAN_ASYNC_TNL_BUSY; //queue is already busy....abort
	}
	can_async_tnl_lock--;
	return (err);
}


/**
 * @brief handles partly filled asynchronous data tx objects for timeout
 * @remarks sends partly filled objects after a timeout has occurred (no new data has arrived until timeout - flush buffer
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_async_tunnel_process_tx_data(void)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	can_async_tnl_lock++;

	if (can_async_tnl_lock == 1)
	{
		//check if tunnel is active
		if (can_async_tunnel.state == LI_CAN_SLV_ASYNC_TUNNEL_ACTIVE)
		{
			if ((can_async_tunnel_tmpTXobj.used > 0) && ((uint32_t)(can_async_tunnel_tmpTXobj.used + can_port_msec_2_ticks(CAN_ASYNC_TNL_PARTLYFILLES_TXOBJ_DELAY_MS)) < li_can_slv_port_get_system_ticks()))
			{
				//transmit entry to asynchronous data tx queue
				err = can_async_send_data_to_async_data_tx_queue(can_async_tunnel.mod_own, CAN_ASYNC_DATA_TX_TYPE_SLAVE, can_async_tunnel_tmpTXobj.used, &can_async_tunnel_tmpTXobj.data[0]);
				if (err == LI_CAN_SLV_ERR_OK)
				{
					//signal that the temporary object is free if the adding was successful
					can_async_tunnel_tmpTXobj.used = 0;
				}
			}
		}
		else
		{
			err = ERR_MSG_CAN_ASYNC_TNL_WRONG_STATE;
		}
	}
	else
	{
		err = ERR_MSG_CAN_ASYNC_TNL_BUSY;
	}

	can_async_tnl_lock--;
	return (err);
}
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL
#endif // #if defined(OUTER) || defined(OUTER_APP)


#if defined (QTP) && defined(LI_CAN_SLV_RECONNECT)
/**
 * @brief can_async_qtp_can_test_send_data
 * @param module_nr number of the module for the download
 * @param src pointer to the source of the data
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t can_async_qtp_can_test_send_data(li_can_slv_module_nr_t module_nr, byte_t *src)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	byte_t msg[8];
	li_can_slv_module_nr_t module_nr_send;
	module_nr = module_nr; //dummy assignment

	module_nr_send = can_port_irol(*((uint16_t *)&src[1]), 8);

	msg[0] = CAN_ASYNC_QTP_CAN_TEST_DEACTIVATE_RECONNECT;
	msg[1] = 0x01;
	msg[2] = 0x02;
	msg[3] = 0x03;
	msg[4] = 0x04;
	msg[5] = 0x05;
	msg[6] = 0x06;
	msg[7] = 0x07;
	for (;;)
	{
		err = can_async_send_data_to_async_ctrl_tx_queue(module_nr_send, CAN_ASYNC_CTRL_TX_TYPE_MASTER, &msg[0]);
	}
	return (err);
}
#endif // #if defined (QTP) && defined(LI_CAN_SLV_RECONNECT)


#ifdef LI_CAN_SLV_ASYNC_TEST_MESSAGE
/**
 * @brief can_async_qtp_can_test_send_data
 * @param module_nr number of the module for the download
 * @param src pointer to the source of the data
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
void li_can_slv_async_send_test_message_ack(void)
{
	byte_t msg[8] = {1, 2, 3, 4, 5, 6, 7, 8};

	(void) can_async_send_data_to_async_ctrl_tx_queue(can_config_get_module_nr_main(), CAN_ASYNC_CTRL_TX_TYPE_MASTER, &msg[0]);
}
#endif // #ifdef LI_CAN_SLV_ASYNC_TEST_MESSAGE


#if defined(OUTER) || defined(OUTER_APP)
#ifdef LI_CAN_SLV_ASYNC_TUNNEL
/**
 * @brief issues the shutdown request for an open CAN tunnel to the partner module
 * @remarks actual connection is held until a close acknowledge / close deny is received
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_async_tunnel_shutdown(void)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	//check if we have an active tunnel
	if (can_async_tunnel.state != LI_CAN_SLV_ASYNC_TUNNEL_ACTIVE)
	{
		return (ERR_MSG_CAN_ASYNC_TNL_WRONG_STATE);
	}

	//set tunnel state accordingly
	can_async_tunnel.state = LI_CAN_SLV_ASYNC_TUNNEL_SHUTDOWN;
	//send out the close command
	err = can_async_tnl_ctl_response(CAN_ASYNC_TNL_CLOSE, can_async_tunnel.prim_modnr, can_async_tunnel.mod_own, 0);

	return (err);
}
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL
#endif // #if defined(OUTER) || defined(OUTER_APP)
/** @} */

