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
 * @file canpie_adapter/io_can_port.h
 * @brief header to io_can_port.h module
 * @addtogroup lcs_arch_canpie_adapter
 * @{
 */

#ifndef IO_CAN_PORT_H_
#define IO_CAN_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>

#include "li_can_slv_port.h"
#include <li_can_slv/core/io_can_main_handler.h>

#ifdef CAN_RANDOM_STATUS_ACKNOWLEDGE
#include "io_util.h"
#include "io_modhw.h"
#endif // #ifdef CAN_RANDOM_STATUS_ACKNOWLEDGE

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
#if CP_VERSION_MAJOR <= 2
#define CAN_LEC_NO_ERROR						CP_ERR_TYPE_NONE /*!< */
#define CAN_LEC_STUFF							CP_ERR_TYPE_STUFF /*!< */
#define CAN_LEC_FORM							CP_ERR_TYPE_FORM /*!< */
#define CAN_LEC_ACK								CP_ERR_TYPE_ACK /*!< */
#define CAN_LEC_BIT1							CP_ERR_TYPE_BIT1 /*!< */
#define CAN_LEC_BIT0							CP_ERR_TYPE_BIT0 /*!< */
#define CAN_LEC_CRC								CP_ERR_TYPE_CRC /*!< */
#else // #if CP_VERSION_MAJOR <= 2
#define CAN_LEC_NO_ERROR						eCP_ERR_TYPE_NONE /*!< */
#define CAN_LEC_STUFF							eCP_ERR_TYPE_STUFF /*!< */
#define CAN_LEC_FORM							eCP_ERR_TYPE_FORM /*!< */
#define CAN_LEC_ACK								eCP_ERR_TYPE_ACK /*!< */
#define CAN_LEC_BIT1							eCP_ERR_TYPE_BIT1 /*!< */
#define CAN_LEC_BIT0							eCP_ERR_TYPE_BIT0 /*!< */
#define CAN_LEC_CRC								eCP_ERR_TYPE_CRC /*!< */
#endif // #if CP_VERSION_MAJOR <= 2
#define CAN_LEC_NOT_DEFINED						-1 /*!< */

#define CAN_PORT_STATIC_INLINE LI_CAN_SLV_PORT_STATIC_INLINE

#define CAN_PORT_DISABLE_IRQ()	li_can_slv_port_disable_irq()
#define CAN_PORT_ENABLE_IRQ()	li_can_slv_port_enable_irq()

#ifdef CAN_SYSTEM_TX_QUEUE
#warning "CAN_SYSTEM_TX_QUEUE is deprecated and will be removed soon so use CAN_MAIN_SYSTEM_MSG_TX_QUEUE instead"
#define CAN_MAIN_SYSTEM_MSG_TX_QUEUE
#endif // #ifdef CAN_SYSTEM_TX_QUEUE

#define can_port_nop() li_can_slv_port_nop()

#define can_port_get_system_ticks()	li_can_slv_port_get_system_ticks()
#define can_port_msec_2_ticks(a)	li_can_slv_port_msec_2_ticks(a)
#define can_port_ticks_2_msec(a)	li_can_slv_port_ticks_2_msec(a)

#ifdef CAN_MAIN_SYSTEM_MSG_TX_QUEUE
#warning can_port_trigger_can_main_system_msg_tx_queue() not implemented!
#define can_port_trigger_can_main_system_msg_tx_queue()
#endif // #ifdef CAN_MAIN_SYSTEM_MSG_TX_QUEUE

#ifdef CAN_ASYNC_CTRL_TX_QUEUE
#warning can_port_trigger_can_main_async_ctrl_tx_queue() not implemented!
#define can_port_trigger_can_main_async_ctrl_tx_queue()
#endif // #ifdef CAN_ASYNC_CTRL_TX_QUEUE

#if defined(OUTER) || defined(OUTER_APP)
#define can_port_trigger_can_main_async_data_tx_queue() can_main_handler_tx(3)
#endif // #if defined(OUTER) || defined(OUTER_APP)

#define can_port_trigger_can_main_sync_process_data_tx_queue() can_main_handler_tx(4)

#ifdef CAN_ASYNC_CTRL_RX_QUEUE
#warning can_port_trigger_can_async_ctrl_rx_queue() not implemented!
#define can_port_trigger_can_async_ctrl_rx_queue()
#endif // #ifdef CAN_ASYNC_CTRL_RX_QUEUE

#define can_port_get_boot_kernel_version_major()	li_can_slv_port_get_boot_kernel_version_major()
#define can_port_get_boot_kernel_version_minor()	li_can_slv_port_get_boot_kernel_version_minor()

#define can_port_get_modul_software_version_major()	li_can_slv_port_get_modul_software_version_major()
#define can_port_get_modul_software_version_minor()	li_can_slv_port_get_modul_software_version_minor()

#define can_port_get_identnumber()	li_can_slv_port_get_identnumber()
#define can_port_get_changeindex()	li_can_slv_port_get_changeindex()
#define can_port_get_serialnumber()	li_can_slv_port_get_serialnumber()

#define can_port_memory_cmp(ptr1, ptr2, size)		li_can_slv_port_memory_cmp(ptr1, ptr2, size)
#define can_port_memory_cpy(dest, source, size)		li_can_slv_port_memory_cpy(dest, source, size)
#define can_port_memory_set(ptr, value, num)		li_can_slv_port_memory_set(ptr, value, num)

#define can_port_string_cmp(str1, str2)				li_can_slv_port_string_cmp(str1, str2)
#define can_port_string_n_cmp(str1, str2, num)		li_can_slv_port_string_n_cmp(str1, str2, num)
#define can_port_string_cpy(dest, source)			li_can_slv_port_string_cpy(dest, source)
#define can_port_string_n_cpy(dest, source, size)	li_can_slv_port_string_n_cpy(dest, source, size)

#define can_port_transceiver_enable()	li_can_slv_port_transceiver_enable();
#define can_port_transceiver_disable()	li_can_slv_port_transceiver_disable();

#ifdef LI_CAN_SLV_BYTE_ORDER_BIG_ENDIAN
#define can_port_swap_uint16(a)	li_can_slv_port_swap_uint16(a)
#endif // #ifdef LI_CAN_SLV_BYTE_ORDER_BIG_ENDIAN

#ifdef LI_CAN_SLV_MAIN_MON
#define can_port_get_mainmon_type()		li_can_slv_port_get_mainmon_type()
#endif // #ifdef LI_CAN_SLV_MAIN_MON

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_CAN_PORT_H_

/** @} */
