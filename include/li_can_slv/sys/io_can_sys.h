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
 * @file io_can_sys.h
 * @brief header to the io_can_sys module
 * @addtogroup can_sys
 * @{
 */

#ifndef IO_CAN_SYS_H
#define IO_CAN_SYS_H

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>
#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/error/li_can_slv_error_types.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
// system commands / system messages from master to module (M2S)
#define	CAN_SYS_M2S_STATUS_REQUEST				0x01 /**< system message status request */
#define	CAN_SYS_M2S_CHANGE_CAN_BAUDRATE			0x02 /**< system message change CAN baud rate */
#define CAN_SYS_M2S_SYSTEM_ERROR_RX				0x03 /**< system message error send */
#define CAN_SYS_M2S_RESERVE_0					0x04 /**< system message reserve 0 */
#define	CAN_SYS_M2S_VERSION_REQUEST				0x05 /**< system message version request */
#define CAN_SYS_M2S_RESERVE_1					0x06 /**< system message reserve 1 */
#define CAN_SYS_M2S_CURRENT_SYSTEM_TIME			0x07 /**< system message current system time */
#define	CAN_SYS_M2S_PROCESS_CONFIGURATION		0x08 /**< system message process configuration */
#define	CAN_SYS_M2S_HARDWARE_VERSION_REQUEST	0x0A /**< system message hardware version request */
#define	CAN_SYS_M2S_RX_RATE						0x0B /**< system message RX rate */
#define	CAN_SYS_M2S_TX_RATE						0x0C /**< system message TX rate */
#define	CAN_SYS_M2S_CHANGE_MODULE_NUMBER		0x20 /**< system message change module number */
#define	CAN_SYS_M2S_STAY_SILENT					0x21 /**< system message stay silent stop transmission */
#define	CAN_SYS_M2S_AWAKE						0x22 /**< system message awake resume transmission */
#define	CAN_SYS_M2S_SHOW_MODULE_IDENTIFICATION	0x23 /**< system message show module identification */
#define	CAN_SYS_M2S_FACTORY_RESET				0x30 /**< system message factory reset */
#define	CAN_SYS_M2S_CHANGE_MODULE_TYPE			0x31 /**< system message change module type */

// system commands / system messages from module to master (S2M)
#define CAN_SYS_S2M_STATUS_ANSWER_OLD_ID		0x01 /**< system message old status answer */
#define CAN_SYS_S2M_NOT_USED					0x02 /**< system message not used */
#define CAN_SYS_S2M_MODULE_ERROR				0x03 /**< system message module error */
#define CAN_SYS_S2M_STATUS_AKNOWLEDGE			0x04 /**< system message status acknowledge */
#define CAN_SYS_S2M_VERSION_ACKNOWLEDGE_1		0x05 /**< system message version acknowledge 1 */
#define CAN_SYS_S2M_VERSION_ACKNOWLEDGE_2		0x06 /**< system message version acknowledge 2 */
#define CAN_SYS_S2M_HW_VERSION_ACKNOWLEDGE		0x0A /**< system message HW version acknowledge */

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/
typedef void(*li_can_slv_sys_system_time_cbk_funcp_t)(li_can_slv_system_time_t);

#ifdef LI_CAN_SLV_SYS_FACTORY_RESET_CBK
typedef void (*li_can_slv_factory_reset_cbk_funcp_t)(byte_t reset_reason);
#endif // #ifdef LI_CAN_SLV_SYS_FACTORY_RESET_CBK

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t can_sys_init(void);
li_can_slv_errorcode_t can_sys_msg_rx(li_can_slv_module_nr_t module_nr, uint16_t dlc, byte_t const *src);
li_can_slv_errorcode_t can_sys_set_first_status_request_call_fnc(void (*pfnc)(void));
void can_sys_set_system_time_cbk(li_can_slv_sys_system_time_cbk_funcp_t cbk);
#ifdef LI_CAN_SLV_SYS_FACTORY_RESET_CBK
void li_can_slv_sys_set_factory_reset_cbk(li_can_slv_factory_reset_cbk_funcp_t cbk);
#endif // #ifdef LI_CAN_SLV_SYS_FACTORY_RESET_CBK

#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
li_can_slv_errorcode_t can_sys_send_error_full(li_can_slv_module_nr_t module_nr, msg_code_t errnum, byte_t add_info, err_prio_t priority, byte_t group_subgroup);
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR

#endif // #ifndef IO_CAN_SYS_H
/** @} */

