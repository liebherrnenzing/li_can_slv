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
 * \file io_can_main.h
 * \brief header to io_can_main module
 * @addtogroup can_driver_main
 * @{
 */

#ifndef IO_CAN_MAIN_H
#define IO_CAN_MAIN_H

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>
#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/core/io_can_main_types.h>
#include <li_can_slv/error/li_can_slv_error_types.h>
#include <li_can_slv/config/li_can_slv_config_defines.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
#ifdef CAN_MAIN_SYSTEM_MSG_TX_QUEUE
// CAN main system message transmit queue definitions
#ifndef CAN_MAIN_SYSTEM_MSG_TX_QUEUE_LENGTH
#define CAN_MAIN_SYSTEM_MSG_TX_QUEUE_LENGTH		16
#endif // #ifndef CAN_MAIN_SYSTEM_MSG_TX_QUEUE_LENGTH
#endif // #ifdef CAN_MAIN_SYSTEM_MSG_TX_QUEUE

/* CAN sync send request */
#define CAN_MAIN_PROCESS_SYNC_SEND_DATA			0xAAAA /**< */
#define CAN_MAIN_PROCESS_SYNC_SEND_DATA_ACK		0x0000 /**< */

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/
#ifdef CAN_MAIN_DIAGNOSE
/**
 * @brief define the diagnose data of main CAN controller
 * @struct can_main_diagnose_t
 */
typedef struct
{
	uint16_t lec[7]; /**< LEC */
	uint16_t rxok; /**< message RX ok counter */
	uint16_t txok; /**< message TX ok counter */
	uint16_t pr; /**< process request */
	uint16_t syncrx; /**< synchrony data rx */
	uint16_t synctx; /**< synchrony data tx */
	uint16_t sysrx; /**< system message rx */
	uint16_t systx; /**< system message tx */
	uint16_t actrlrx; /**< */
	uint16_t adatarx; /**< */
	uint16_t asynctx; /**< */
	uint16_t err; /**< error frame counter */
	uint16_t ewrn; /**< error warning counter*/
	uint16_t boff; /**< bus off counter */
#ifdef LI_CAN_SLV_DEBUG_MAIN_SYS_HANDLER_EXTENDED
	uint16_t sys_lost; /**< */
#endif // #ifdef LI_CAN_SLV_DEBUG_MAIN_SYS_HANDLER_EXTENDED
#ifdef LI_CAN_SLV_DEBUG_CAN_ASYNC_SYS
	uint16_t actrl_lost; /**< */
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_ASYNC_SYS
} can_main_diagnose_t;
#endif // #ifdef CAN_MAIN_DIAGNOSE

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief define the rx message object configuration
 */
typedef struct
{
	dword_t msg_obj_mask; /**< */
	uint16_t msg_obj; /**< */
	uint16_t table_pos; /**< */
} can_main_rx_msg_obj_t;

/**
 * @brief define the tx message object configuration
 */
typedef struct
{
	dword_t msg_obj_mask; /**< */
	uint16_t msg_obj; /**< */
	uint16_t table_pos; /**< */
} can_main_tx_msg_obj_t;

/**
 * @brief define the control of the process data transmission
 */
typedef struct
{
	uint16_t send_cmd; /**< indicates a new send request for sync data*/
	dword_t send_reg; /**< send register for synchronous process data */
	dword_t send; /**< send flags for synchronous process data */
	uint16_t send_end; /**< bit position of last active send flag */
	uint16_t send_current; /**< bit position of current send flag */
	uint16_t id[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES][CAN_CONFIG_NR_OF_MODULE_OBJS]; /**< standard (11-bit) identifier */
	uint16_t dlc[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES][CAN_CONFIG_NR_OF_MODULE_OBJS]; /**< data length code */
} can_main_sync_process_tx_data_ctrl_t;
#endif // #if defined(OUTER) || defined(OUTER_APP)

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/
/* interrupt pending masks */
extern volatile dword_t can_main_isr_inp_mask_rx; /*!< interrupt pending mask receive for can_main_isr */
extern volatile dword_t can_main_isr_inp_mask_tx; /*!< interrupt pending mask transmit for can_main_isr */

extern volatile dword_t can_main_objs_mask; /*!< mask that identifies all slots the CAN controller is configured for */
extern volatile dword_t can_main_async_objs_mask; /*!< mask that identifies all async data slots the CAN controller is configured for */
#if defined(OUTER) || defined(OUTER_APP)
extern volatile dword_t can_main_async_ctrl_objs_mask; /*!< mask that identifies all async ctrl data slots the CAN controller is configured for */
#endif // #if defined(OUTER) || defined(OUTER_APP)

#if defined(OUTER) || defined(OUTER_APP)
extern volatile uint16_t li_can_slv_sync_main_rx_msg_obj_used; /**< */
extern volatile uint16_t li_can_slv_sync_main_tx_msg_obj_used; /**< */
#endif // #if defined(OUTER) || defined(OUTER_APP)

#if defined(OUTER) || defined(OUTER_APP)
/* process data transmit control */
extern volatile can_main_sync_process_tx_data_ctrl_t can_main_sync_process_tx_data_ctrl;
#endif // #if defined(OUTER) || defined(OUTER_APP)

#if defined(OUTER) || defined(OUTER_APP)
extern volatile can_main_rx_msg_obj_t li_can_slv_sync_main_rx_msg_obj[CAN_CONFIG_SYNC_MAIN_MAX_NR_OF_RX_OBJ]; /**< */
extern volatile can_main_tx_msg_obj_t li_can_slv_sync_main_tx_msg_obj[CAN_CONFIG_SYNC_MAIN_MAX_NR_OF_TX_OBJ]; /**< */
#endif // #if defined(OUTER) || defined(OUTER_APP)

#ifdef CAN_MAIN_DIAGNOSE
extern can_main_diagnose_t can_main_diagnose;
#endif // #ifdef CAN_MAIN_DIAGNOSE

#ifdef CAN_SMP
extern uint16_t FASTRAM can_main_canid; /**< */
extern uint16_t FASTRAM can_main_d0; /**< */
extern uint16_t FASTRAM can_main_lec; /**< */
extern uint16_t FASTRAM can_main_recmax; /**< */
extern uint16_t FASTRAM can_main_tecmax; /**< */
#endif // #ifdef CAN_SMP

/**
 * @todo the global variables should be removed
 */
#ifdef CAN_MAIN_SYSTEM_MSG_TX_QUEUE
extern volatile uint16_t can_main_system_msg_tx_queue_state; /*!< state of the system message transmit queue */
#endif // #ifdef CAN_MAIN_SYSTEM_MSG_TX_QUEUE

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t can_main_init(void);
li_can_slv_errorcode_t can_main_send_queue_system_tx(li_can_slv_module_nr_t module_nr, byte_t const *src);
#if defined(OUTER) || defined(OUTER_APP)
li_can_slv_errorcode_t can_main_process_start(void);
li_can_slv_errorcode_t can_main_process_stop(void);
li_can_slv_errorcode_t can_main_sync_process_tx_data_cnfg(uint16_t table_pos, uint16_t obj, li_can_slv_module_nr_t module_nr);
void can_main_synchron_tx_data_off(uint16_t table_pos, uint16_t obj);
void can_main_synchron_tx_data_on(uint16_t table_pos, uint16_t obj);
li_can_slv_errorcode_t can_main_msg_obj_rx_data_cnfg(uint16_t msg_obj, uint16_t table_pos);
li_can_slv_errorcode_t can_main_msg_obj_tx_data_cnfg(uint16_t msg_obj);
li_can_slv_errorcode_t can_main_process_data_tx_set(void);
#endif	// #if defined(OUTER) || defined(OUTER_APP)

li_can_slv_errorcode_t can_main_enable(void);
li_can_slv_errorcode_t can_main_disable(void);

#ifdef CAN_MAIN_SYSTEM_MSG_TX_QUEUE
li_can_slv_errorcode_t can_main_hndl_queue_system_tx(void);
#endif // #ifdef CAN_MAIN_SYSTEM_MSG_TX_QUEUE

#ifdef CAN_ASYNC_CTRL_TX_QUEUE
li_can_slv_errorcode_t can_main_hndl_queue_async_ctrl_tx(void);
#endif // #ifdef CAN_ASYNC_CTRL_TX_QUEUE

#if defined(OUTER) || defined(OUTER_APP)
li_can_slv_errorcode_t can_async_handle_async_data_tx_queue(void);
void can_main_sync_process_tx_data(void);
#endif // #if defined(OUTER) || defined(OUTER_APP)

li_can_slv_errorcode_t can_main_define_msg_obj(uint16_t msg_obj, uint16_t can_id, uint16_t acceptance_mask, byte_t dlc, byte_t dir, can_main_service_id_t service_id, bit_t is_sync_obj);

/**
 * @brief get the next free message object number
 * @param[out] msg_obj number of message object
 * @return #LI_CAN_SLV_ERR_OK or #ERR_MSG_CAN_NO_MSG_OBJ_FREE
 */
li_can_slv_errorcode_t can_main_get_next_free_msg_obj(uint16_t *msg_obj);

#endif // #ifndef IO_CAN_MAIN_H
/** @} */
