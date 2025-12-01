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
 * @file io_can_mon.h
 * @brief header to io_can_mon.h module
 * @addtogroup can_driver_mon
 * @{
 */

#ifndef IO_CAN_MON_H_
#define IO_CAN_MON_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>
#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/core/io_can_mon_types.h>
#include <li_can_slv/error/li_can_slv_error_types.h>
#include <li_can_slv/config/li_can_slv_config_defines.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
#define CAN_MON_ISR_TXINP	1 /**< */
#define CAN_MON_ISR_RXINP	1 /**< */

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

// LEC:
//1:
//// Stuff Error
//// More than 5 equal bits in a sequence have occurred
//// in a part of a received message where this is not
//// allowed.
//
//2:
//// Form Error
//// A fixed format part of a received frame has the
//// wrong format.
//
//3:
//// Ack Error
//// The message this CAN controller transmitted was
//// not acknowledged by another node.
//
//4:
//// Bit1 Error
//// During the transmission of a message (with the
//// exeption of the arbitration field), the device
//// wanted to send a recessive level ("1"), but the
//// monitored bus value was dominant.
//
//5:
//// Bit0 Error
//// During the transmission of a message (or acknow-
//// ledge bit, active error flag, or overload flag),
//// the device wanted to send a dominant level ("0"),
//// but the monitored bus value was recessive. During
//// busoff recovery this staus is set each time a
//// sequence of 11 recessive bits has been monitored.
//// This enables the CPU to monitor the proceeding of
//// the busoff recovery sequence (indicating the bus
//// is not stuck at dominant or continously disturbed).
//
//6:
//// CRC Error
//// The CRC check sum was incorrect in the message
//// received.

#ifdef CAN_MON_DIAGNOSE
/*!
 * \brief define the diagnose data of monitor CAN controller
 */
typedef struct
{
	uint16_t lec[7]; /**< LEC */
	uint16_t rxok; /**< message RX ok counter */
	uint16_t txok; /**< message TX ok counter */
	uint16_t pr; /**< process request */
	uint16_t err; /**< error frame counter */
	uint16_t ewrn; /**< error warning counter*/
	uint16_t boff; /**< bus off counter */
} can_mon_diagnose_t;
#endif // #ifdef CAN_MON_DIAGNOSE

/*!
 * \brief define the rx message object configuration
 */
typedef struct
{
	dword_t msg_obj_mask; /**< */
	uint16_t msg_obj; /**< */
	uint16_t table_pos; /**< */
} can_mon_rx_msg_obj_t;

/*!
 * \brief define the tx message object configuration
 */
typedef can_mon_rx_msg_obj_t can_mon_tx_msg_obj_t;

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/
extern volatile dword_t can_mon_isr_inp_mask_rx_main; /*!< interrupt pending mask transmit for can_mon_isr */
extern volatile dword_t can_mon_isr_inp_mask_rx; /*!< interrupt pending mask receive for can_mon_isr */
extern volatile dword_t can_mon_objs_mask; /*!< mask that identifies all slots the CAN controller is configured for */
extern can_mon_tx_msg_obj_t can_mon_tx_msg_obj[CAN_CONFIG_SYNC_MON_MAX_NR_OF_TX_OBJ];
extern uint16_t can_mon_tx_msg_obj_used;
extern can_mon_rx_msg_obj_t can_mon_rx_msg_obj[CAN_CONFIG_SYNC_MON_MAX_NR_OF_RX_OBJ];
extern uint16_t can_mon_rx_msg_obj_used;

#ifdef CAN_MON_DIAGNOSE
extern can_mon_diagnose_t can_mon_diagnose;
#endif // #ifdef CAN_MON_DIAGNOSE

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t can_mon_init(void);
li_can_slv_errorcode_t can_mon_deinit(void);
li_can_slv_errorcode_t can_mon_enable(void);
li_can_slv_errorcode_t can_mon_disable(void);
li_can_slv_errorcode_t can_mon_process_stop(void);
li_can_slv_errorcode_t can_mon_process_start(void);
li_can_slv_errorcode_t can_mon_msg_obj_rx_data_cnfg(uint16_t obj_nr, uint16_t table_pos);
li_can_slv_errorcode_t can_mon_msg_obj_tx_data_cnfg(uint16_t obj_nr, uint16_t table_pos);
li_can_slv_errorcode_t can_mon_define_msg_obj(uint16_t msg_obj, uint16_t can_id, uint16_t acceptance_mask, byte_t dlc, byte_t dir, can_mon_service_id_t service_id, bit_t is_sync_obj);

/**
 * @brief get the next free message object number
 * @param[out] msg_obj number of message object
 * @return #LI_CAN_SLV_ERR_OK or #ERR_MSG_CAN_NO_MSG_OBJ_FREE
 */
li_can_slv_errorcode_t can_mon_get_next_free_msg_obj(uint16_t *msg_obj);

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_CAN_MON_H_

/** @} */
