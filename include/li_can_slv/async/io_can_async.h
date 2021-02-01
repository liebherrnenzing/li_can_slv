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
 * @file io_can_async.h
 * @brief header to IO CAN ASYNC module
 * @addtogroup can_async
 * @{
 */
#ifndef IO_CAN_ASYNC_H
#define IO_CAN_ASYNC_H

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>
#include <li_can_slv/config/io_can_config.h>
#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/error/li_can_slv_error_types.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
// CAN async receive queue definitions
#ifndef CAN_ASYNC_CTRL_RX_QUEUE_LENGTH
#define CAN_ASYNC_CTRL_RX_QUEUE_LENGTH	16
#endif // #ifndef CAN_ASYNC_CTRL_RX_QUEUE_LENGTH

#define LI_CAN_SLV_ASYNC_TUNNEL_OPEN			0x01 /*!< tunnel open request */
#define LI_CAN_SLV_ASYNC_TUNNEL_CLOSE			0x02 /*!< tunnel close request */
#define LI_CAN_SLV_ASYNC_TUNNEL_OPEN_ACK		0x03 /*!< tunnel open acknowledge */
#define LI_CAN_SLV_ASYNC_TUNNEL_CLOSE_ACK		0x04 /*!< tunnel close acknowledge */
#define LI_CAN_SLV_ASYNC_TUNNEL_OPEN_DENY		0x05 /*!< tunnel open deny */
#define LI_CAN_SLV_ASYNC_TUNNEL_CLOSE_DENY		0x06 /*!< tunnel close deny */
#define LI_CAN_SLV_ASYNC_TUNNEL_REMOTE_OPEN		0x07 /*!< tunnel remote open request */
#define LI_CAN_SLV_ASYNC_TUNNEL_RESET_PORT		0x08 /*!< tunnel empty all buffers/XOFF */
#define CAN_ASYNC_XOFF							0x09 /*!< start transmission (XOFF) */
#define CAN_ASYNC_XON							0x0A /*!< start transmission (XON) */
#define LI_CAN_SLV_ASYNC_TUNNEL_LOGIN_REQUEST	0x0B /*!< tunnel login request */
#define LI_CAN_SLV_ASYNC_TUNNEL_LOGIN_ACK		0x0C /*!< tunnel login acknowledge */
#define LI_CAN_SLV_ASYNC_TUNNEL_LOGIN_DENY		0x0D /*!< tunnel login deny */

#define	CAN_ASYNC_DL_VERSION_REQUEST_SHORT	0x10u /*!< download version request short */
#define	CAN_ASYNC_DL_VERSION_REQUEST_LONG1	0x11u /*!< download version request long 1 */
#define	CAN_ASYNC_DL_VERSION_REQUEST_LONG2	0x12u /*!< download version request long 2 */
#define	CAN_ASYNC_DL_START_REQUEST1			0x13u /*!< download start request 1 */
#define	CAN_ASYNC_DL_START_REQUEST2			0x14u /*!< download start request 2 */
#define	CAN_ASYNC_DL_DATA					0x15u /*!< download data */
#define	CAN_ASYNC_DL_DATA_BLOCK_END			0x16u /*!< download data block end */
#define	CAN_ASYNC_DL_END					0x17u /*!< download end */
#define	CAN_ASYNC_DL_VERSION_ANSWER			0x18u /*!< download version answer */
#define	CAN_ASYNC_DL_START_ACKN				0x19u /*!< download start acknowledge */
#define	CAN_ASYNC_DL_BLOCK_ACKN				0x1Au /*!< download block acknowledge */
#define	CAN_ASYNC_DL_END_ACKN				0x1Bu /*!< download end acknowledge */
#define	CAN_ASYNC_DL_TERMINATION			0x1Cu /*!< download termination */
#define	CAN_ASYNC_UL_START_REQUEST1			0x1Du /*!< upload start request 1 */
#define	CAN_ASYNC_UL_START_REQUEST2			0x1Eu /*!< upload start request 2 */

#define	CAN_ASYNC_UL_INFO_REQUEST_SHORT		0x50u /*!< upload info request short */
#define	CAN_ASYNC_UL_INFO_ANSWER			0x53u /*!< upload info answer */

#define	CAN_ASYNC_CONNECTION_TYPE_DEFAULT				0x00 /*!< default connectivity type */
#define	CAN_ASYNC_CONNECTION_TYPE_SPS_MONITOR_LP		0x01 /*!< SPS Monitor connectivity type */
#define	CAN_ASYNC_CONNECTION_TYPE_HEINZMANN_PROTOCOL	0x02 /*!< Heinzmann protocol connectivity type */
#define	CAN_ASYNC_CONNECTION_TYPE_UDS_PROTOCOL			0x03 /*!< UDS protocol connectivity type */

#ifdef LIDIS
#define	CAN_ASYNC_DIAGNOSTICS_LIDIS		0x30 /*!< diagnostics LIDIS */
#endif // #ifdef LIDIS

#ifdef QTP
#define	CAN_ASYNC_QTP_CAN_TEST_DEACTIVATE_RECONNECT		0xFE /*!< use asynchronous control data for QTP CAN reconnect off */
#endif // #ifdef QTP

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_ASYNC_TUNNEL
/*!
 * \brief possible states of the asynchronous tunnel
 * \enum can_async_tunnel_state_tag
 * \typedef enum can_async_tunnel_state_tag can_async_tunnel_state_t
 */
typedef enum can_async_tunnel_state_tag
{
	LI_CAN_SLV_ASYNC_TUNNEL_FREE = 0, /*!< CAN tunnel is free and not active */
	LI_CAN_SLV_ASYNC_TUNNEL_ACTIVE = 1, /*!< CAN tunnel is occupied and active */
	LI_CAN_SLV_ASYNC_TUNNEL_SHUTDOWN = 2 /*!< CAN tunnel shutdown has been initiated by own module, has not been answered yet */
} can_async_tunnel_state_t;

/*!
 * \brief direction of tunnel
 * \enum can_async_tunnel_type_tag
 * \typedef enum can_async_tunnel_type_tag can_async_tunnel_type_t
 */
typedef enum can_async_tunnel_type_tag
{
	LI_CAN_SLV_ASYNC_TUNNEL_UNDEFINED, /*!< Undefined direction of tunnel */
	LI_CAN_SLV_ASYNC_TUNNEL_PRIMARY, /*!< This module has triggered the  tunnel (PRIMARY) */
	LI_CAN_SLV_ASYNC_TUNNEL_SECONDARY /*!< This module is the responding partner (SECONDARY) */
} can_async_tunnel_type_t;

/*!
 * \brief define info struct for the asynchronous CAN tunnel
 * \struct can_async_tunnel_tag
 * \typedef struct can_async_tunnel_tag can_async_tunnel_t
 */
typedef struct can_async_tunnel_tag
{
	can_async_tunnel_state_t state; /*!< actual state of CAN asynchronous tunnel */
	can_async_tunnel_type_t type; /*!< type of tunnel (PRIM/SEC) from own point of view*/
	li_can_slv_module_nr_t prim_modnr; /*!< module number of primary node of tunnel */
	li_can_slv_module_nr_t sec_modnr; /*!< module number of secondary node */
	li_can_slv_module_nr_t mod_own; /*!< module number of own used logical module */
	uint16_t async_data_delay; /*!< Negotiated delay time in us between two consecutive asynchronous tunnel data objects */
	byte_t connection_type; /*!< decision if default CAN asynchronous connection(0x00) or SPS Monitor LP connection(0x01) */
} can_async_tunnel_t;
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL

#ifdef CAN_ASYNC_CTRL_RX_QUEUE
/*!
 * \brief defines an element of the asynchronous RX queue
 * \struct can_async_rx_queue_tag
 * \typedef struct can_async_rx_queue_tag can_async_rx_queue_t
 */
typedef struct can_async_rx_queue_tag
{
	li_can_slv_module_nr_t module_nr; /**< module number*/
	byte_t data[CAN_CONFIG_SYS_MSG_DLC]; /**< data*/
} can_async_rx_queue_t;
#endif // #ifdef CAN_ASYNC_CTRL_RX_QUEUE

/*!
 * \brief define the asynchronous ctrl transmit type
 * \typedef can_async_ctrl_tx_type_t
 */
typedef enum
{
	CAN_ASYNC_CTRL_TX_TYPE_SLAVE = 0x00, /*!< asynchronous control type slave */
	CAN_ASYNC_CTRL_TX_TYPE_MASTER = 0x01 /*!< asynchronous control type master */
} can_async_ctrl_tx_type_t;

/*!
 * \brief define the asynchronous data transmit type
 * \enum can_async_data_tx_type_t
 */
typedef enum
{
	CAN_ASYNC_DATA_TX_TYPE_SLAVE = 0x00, /*!< asynchronous data type slave */
	CAN_ASYNC_DATA_TX_TYPE_MASTER = 0x01 /*!< asynchronous data type master */
} can_async_data_tx_type_t;

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_ASYNC_TUNNEL
/* asynchronous tunnel information */
extern volatile can_async_tunnel_t can_async_tunnel;
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL

#ifdef CAN_ASYNC_CTRL_RX_QUEUE
/* asynchronous message receive queue */
extern volatile uint16_t can_async_rx_queue_read;
extern volatile uint16_t can_async_rx_queue_state;
extern volatile can_async_rx_queue_t can_async_ctrl_rx_queue[CAN_ASYNC_CTRL_RX_QUEUE_LENGTH];
#endif // #ifdef CAN_ASYNC_CTRL_RX_QUEUE

#if defined(OUTER) || defined(OUTER_APP)
extern volatile uint16_t can_async_data_tx_queue_state; /*!< state of the asynchronous transmit data queue */
#endif // #if defined(OUTER) || defined(OUTER_APP)

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
#ifdef CAN_ASYNC_CTRL_RX_QUEUE
li_can_slv_errorcode_t can_async_init(void);
#endif // #ifdef CAN_ASYNC_CTRL_RX_QUEUE

li_can_slv_errorcode_t can_async_rx(li_can_slv_module_nr_t module_nr, byte_t const *data);

li_can_slv_errorcode_t can_async_send_data_to_async_ctrl_rx_queue(li_can_slv_module_nr_t module_nr, byte_t const *src);
li_can_slv_errorcode_t can_async_send_data_to_async_ctrl_tx_queue(li_can_slv_module_nr_t module_nr, can_async_ctrl_tx_type_t type, const byte_t *src);

#if defined(OUTER) || defined(OUTER_APP)
li_can_slv_errorcode_t can_async_send_data_to_async_data_tx_queue(li_can_slv_module_nr_t module_nr, can_async_data_tx_type_t type, byte_t dlc, const byte_t *src);
#endif // #if defined(OUTER) || defined(OUTER_APP)

#if defined(OUTER) || defined(OUTER_APP)
li_can_slv_errorcode_t can_async_handle_async_data_tx_queue(void);
#endif // #if defined(OUTER) || defined(OUTER_APP)

#ifdef LI_CAN_SLV_ASYNC_TUNNEL
#if defined(OUTER) || defined(OUTER_APP)
li_can_slv_errorcode_t can_async_tunnel_process_tx_data(void);
li_can_slv_errorcode_t can_async_tunnel_shutdown(void);
#endif // #if defined(OUTER) || defined(OUTER_APP)
#endif // #ifdef LI_CAN_SLV_ASYNC_TUNNEL

#endif // #ifndef IO_CAN_ASYNC_H
/** @} */

