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
 * @file io_can_reconnect.h
 * @brief header to IO CAN RECONNECT module
 * @addtogroup can_reconnect
 * @{
 */
#ifndef IO_CAN_RECONNECT_H
#define IO_CAN_RECONNECT_H

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>
#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/error/li_can_slv_error_types.h>
#ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
#include <li_can_slv/config/io_can_config_types.h>
#endif // #ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
/**
 * @todo fix for XE16x devices
 */
#ifdef LI_CAN_SLV_RECONNECT
#define CAN_RECONNECT_IDLE_CNT 2 /**< */
#else // #ifdef LI_CAN_SLV_RECONNECT
#define CAN_RECONNECT_IDLE_CNT 20 /**< */
#endif // #ifdef LI_CAN_SLV_RECONNECT

#ifndef LI_CAN_SLV_RECONNECT_BAUDRATE_SWITCH_TIME
#define LI_CAN_SLV_RECONNECT_BAUDRATE_SWITCH_TIME	((uint16_t)(50)) /**< baudrate switch time 50 msec default */
#endif // #ifndef LI_CAN_SLV_RECONNECT_BAUDRATE_SWITCH_TIME

#ifndef LI_CAN_SLV_RECONNECT_STARTUP_NO_TRAFFIC_TIME
#define CAN_RECONNECT_STARTUP_NO_TRAFFIC_TIME (600) /**< startup reconnect time 600 msec default */
#else
#define CAN_RECONNECT_STARTUP_NO_TRAFFIC_TIME (LI_CAN_SLV_RECONNECT_STARTUP_NO_TRAFFIC_TIME) /**< startup reconnect time */
#endif // #ifndef LI_CAN_SLV_RECONNECT_STARTUP_NO_TRAFFIC_TIME

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
typedef void (*li_can_slv_reconnect_online_change_baudrate_start_cbk_funcp_t)(void);
typedef void (*li_can_slv_reconnect_online_change_baudrate_stop_cbk_funcp_t)(li_can_slv_config_bdr_t baudrate);
#endif // #ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK

#ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK
typedef void (*li_can_slv_reconnect_startup_change_baudrate_start_cbk_funcp_t)(void);
typedef void (*li_can_slv_reconnect_startup_change_baudrate_stop_cbk_funcp_t)(li_can_slv_config_bdr_t baudrate);
#endif // #ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK

/**
 * \typedef li_can_slv_reconnect_state_t
 * \brief define the possible states reconnect
 */
typedef enum
{
	CAN_RECONNECT_STATE_DEACTIVATED = 0, /*!< reconnect deactivated */ //!< CAN_RECONNECT_STATE_DEACTIVATED
	CAN_RECONNECT_STATE_OFF = 1, /*!< reconnect off */ //!< CAN_RECONNECT_STATE_OFF
	CAN_RECONNECT_STATE_STARTUP = 2, /*!< reconnect startup */ //!< CAN_RECONNECT_STATE_STARTUP
	CAN_RECONNECT_STATE_ON = 3, /*!< reconnect online */ //!< CAN_RECONNECT_STATE_ON
	CAN_RECONNECT_STATE_IDLE = 4 /*!< reconnect idle */ //!< CAN_RECONNECT_STATE_IDLE
} li_can_slv_reconnect_state_t;

/**
 * \typedef li_can_slv_reconnect_cause_t
 * \brief define the possible reconnect cause
 */
typedef enum
{
	CAN_RECONNECT_CAUSE_STARTUP_NO_TRAFFIC = 0, /*!< reconnect startup no traffic */
	CAN_RECONNECT_CAUSE_STARTUP = 1, /*!< reconnect startup */
	CAN_RECONNECT_CAUSE_ONLINE_MAIN_EWRN = 2, /*!< reconnect main error warning */
	CAN_RECONNECT_CAUSE_ONLINE_MAIN_BOFF = 3, /*!< reconnect main bus off */
	CAN_RECONNECT_CAUSE_ONLINE_MON_EWRN = 4, /*!< reconnect monitor error warning */
	CAN_RECONNECT_CAUSE_ONLINE_MON_BOFF = 5, /*!< reconnect monitor bus off */
	CAN_RECONNECT_CAUSE_DOWNLOAD_ACKN = 6 /*!< reconnect download */
} li_can_slv_reconnect_cause_t;

/*!
 * \typedef li_can_slv_reconnect_back_t
 * \brief define the possible reconnect cause
 */
typedef enum
{
	CAN_RECONNECT_BACK_UNKNOWN = 0, /*!<  */
	CAN_RECONNECT_BACK_STARTUP_TIME = 1, /*!<  */
	CAN_RECONNECT_BACK_STARTUP_VALID_EVENT = 2, /*!<  */
	CAN_RECONNECT_BACK_VALID_EVENT = 3, /*!<  */
	CAN_RECONNECT_BACK_NO_LEC = 4, /*!<  */
	CAN_RECONNECT_BACK_BIT0 = 5, /*!<  */
	CAN_RECONNECT_BACK_NO_IRQ_PENDING = 6, /*!<  */
	CAN_RECONNECT_BACK_DOWNLOAD_ACKN = 7 /*!<  */
} li_can_slv_reconnect_back_t;

/**
 * @brief can_reconnect type definition
 * @struct li_can_slv_reconnect_t
 */
typedef struct
{
	li_can_slv_reconnect_state_t state; /**< */
	li_can_slv_reconnect_cause_t cause; /**< */
	li_can_slv_reconnect_back_t back; /**< */
	uint16_t lec_cnt_bit0; /**< last error code bit0 counter */
	uint32_t time; /**< time stamp */
	uint16_t bdr_index; /**< */
	uint16_t nr; /**< */
	uint16_t idle_cnt; /**< */
	uint32_t time_next_bdr_change; /**< time stamp of next allowed baud rate  change in reconnect (used for timed checks for next baud rate  on no CAN messages */
	uint16_t id_on;
	uint16_t id_off;
} li_can_slv_reconnect_t;

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/
extern volatile int16_t li_can_slv_reconnect_main_node_msg_pending;
extern volatile int16_t li_can_slv_reconnect_mon_node_msg_pending;

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t li_can_slv_reconnect_init(void);
li_can_slv_errorcode_t li_can_slv_reconnect_startup(void);
li_can_slv_errorcode_t li_can_slv_reconnect_download(void);
li_can_slv_reconnect_state_t li_can_slv_reconnect_get_state(void);
li_can_slv_errorcode_t li_can_slv_reconnect_set_state(li_can_slv_reconnect_state_t state);

#ifdef LI_CAN_SLV_RECONNECT_MAIN_NODE
li_can_slv_errorcode_t li_can_slv_reconnect_on_main_node_online(uint16_t ewrn);
li_can_slv_errorcode_t li_can_slv_reconnect_on_main_node_recovery(uint16_t boff);
#endif // #ifdef LI_CAN_SLV_RECONNECT_MAIN_NODE

#ifdef LI_CAN_SLV_RECONNECT_MON_NODE
li_can_slv_errorcode_t li_can_slv_reconnect_on_mon_node_online(uint16_t ewrn);
li_can_slv_errorcode_t li_can_slv_reconnect_on_mon_node_recovery(uint16_t boff);
#endif // #ifdef LI_CAN_SLV_RECONNECT_MON_NODE
void li_can_slv_reconnect_process_on(void);
void li_can_slv_reconnect_process_off(void);
li_can_slv_errorcode_t li_can_slv_reconnect_process(int16_t intid, int16_t lec);

#ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
void li_can_slv_reconnect_set_online_change_baudrate_start_cbk(li_can_slv_reconnect_online_change_baudrate_start_cbk_funcp_t cbk);
void li_can_slv_reconnect_set_online_change_baudrate_stop_cbk(li_can_slv_reconnect_online_change_baudrate_stop_cbk_funcp_t cbk);
#endif // #ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK

#ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK
void li_can_slv_reconnect_set_startup_change_baudrate_start_cbk(li_can_slv_reconnect_startup_change_baudrate_start_cbk_funcp_t cbk);
void li_can_slv_reconnect_set_startup_change_baudrate_stop_cbk(li_can_slv_reconnect_startup_change_baudrate_stop_cbk_funcp_t cbk);
#endif // #ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK

#endif // #ifndef IO_CAN_RECONNECT_H
/** @} */

