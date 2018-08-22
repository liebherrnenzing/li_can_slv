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
 * @file io_can_sync.h
 * @brief header to IO CAN SYNC module
 * @addtogroup can_sync
 * @{
 */
#ifndef IO_CAN_SYNC_H
#define IO_CAN_SYNC_H

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/core/li_can_slv.h>

#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/error/li_can_slv_error_types.h>

#include <li_can_slv/config/li_can_slv_config_defines.h>
#include <li_can_slv/core/li_can_slv_core_defines.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
#define CAN_SYNC_VALID_NR_OF_PROCESS_REQUEST    1 /**< valid number of of process requests */
#define CAN_SYNC_VALID_DLC_MAX_PROCESS_REQUEST	1 /**< maximal valid data length code of process requests */
#define CAN_SYNC_VALID_NR_OF_RX_DATA		1 /**< valid number of received data objects */
#define CAN_SYNC_VALID_NR_OF_TX_DATA		1 /**< valid number of transmit data objects */

#ifndef LI_CAN_SLV_SYNC_PROCESS_SAFETY_TIME
#define LI_CAN_SLV_SYNC_PROCESS_SAFETY_TIME	(100) /**< process safety time in milliseconds */
#else // #ifndef LI_CAN_SLV_SYNC_PROCESS_PERIODE_MAX
#if (LI_CAN_SLV_SYNC_PROCESS_SAFETY_TIME > 100)
#warning the LI_CAN_SLV_SYNC_PROCESS_SAFETY_TIME is greater than 100 msec
#endif
#endif // #ifndef LI_CAN_SLV_SYNC_PROCESS_SAFETY_TIME

#ifndef LI_CAN_SLV_SYNC_PROCESS_PERIODE_MAX
#define LI_CAN_SLV_SYNC_PROCESS_PERIODE_MAX	(100) /**< maximum time in milliseconds for process period */
#else // #ifndef LI_CAN_SLV_SYNC_PROCESS_PERIODE_MAX
#if (LI_CAN_SLV_SYNC_PROCESS_PERIODE_MAX > 200)
#warning the LI_CAN_SLV_SYNC_PROCESS_PERIODE_MAX is greater than 200 msec
#endif
#endif // #ifndef LI_CAN_SLV_SYNC_PROCESS_PERIODE_MAX

//#define CAN_SYNC_PROCESS_NOT_VALID 	0 /**< */
//#define CAN_SYNC_PROCESS_VALID		1 /**< */
#define CAN_SYNC_OBJ_MASK				0x0003 /**< mask to get object number from CAN identifier */

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/
/**
 * @brief define all error types of synchronous process
 * @struct can_sync_err_tag
 * @typedef struct can_sync_err_tag can_sync_err_t
 */
typedef struct can_sync_err_tag
{
	uint16_t main_pr_cnt; /**< main process counter*/
#ifdef LI_CAN_SLV_MON
	uint16_t mon_pr_cnt; /**< mon process counter*/
#endif // #ifdef LI_CAN_SLV_MON
	uint16_t main_rx_cnt[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES]; /**< main rx counter*/
	uint16_t main_tx_cnt[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES]; /**< main tx counter*/
#ifdef LI_CAN_SLV_MON
	uint16_t mon_rx_cnt[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES]; /**< mon rx counter*/
	uint16_t mon_tx_cnt[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES]; /**< mon tx counter*/
#endif // #ifdef LI_CAN_SLV_MON
	uint16_t data[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES]; /**< error data*/
} can_sync_err_t;

/**
 * @brief define all data informations to check a correct process image
 * @struct can_sync_tag
 * @typedef struct can_sync_tag can_sync_t
 */
typedef struct can_sync_tag
{
	uint32_t main_pr_time; /*!< time stamp of received process request on main */
#ifdef LI_CAN_SLV_MON
	uint32_t mon_pr_time; /*!< time stamp of received process request on monitor */
#endif // #ifdef LI_CAN_SLV_MON
	uint16_t main_pr_cnt; /*!< main process request counter */
	uint16_t main_pr_dlc; /*!< main data length code of process request */
	//uint16_t main_pr_index; /*!< main index of process request */
#ifdef LI_CAN_SLV_MON
	uint16_t mon_pr_cnt; /*!< monitor process request counter */
	uint16_t mon_pr_dlc; /*!< monitor data length code of process request */
	//uint16_t mon_pr_index; /*!< monitor index of process request */
#endif // #ifdef LI_CAN_SLV_MON
	uint16_t main_rx_cnt[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES][CAN_CONFIG_NR_OF_MODULE_OBJS]; /*!< main rx object counter of each defined rx-object */
	uint16_t main_tx_cnt[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES][CAN_CONFIG_NR_OF_MODULE_OBJS]; /*!< main tx object counter of each defined tx-object */
#ifdef LI_CAN_SLV_MON
	uint16_t mon_rx_cnt[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES][CAN_CONFIG_NR_OF_MODULE_OBJS]; /*!< monitor rx object counter of each defined rx-object */
	uint16_t mon_tx_cnt[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES][CAN_CONFIG_NR_OF_MODULE_OBJS]; /*!< monitor tx object counter of each defined tx-object */
#endif // #ifdef LI_CAN_SLV_MON
	uint16_t main_pr_flag; /*!< main process request flag */
#ifdef LI_CAN_SLV_MON
	uint16_t mon_pr_flag; /*!< monitor process request flag */
#endif // #ifdef LI_CAN_SLV_MON
	uint32_t pr_time_valid; /*!< time stamp of last valid process image */
	uint32_t pr_time_valid_module[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES]; /*!< time stamp of last valid process image for each logical module */
	uint32_t pr_periode; /*!< process period */
	uint8_t image_valid[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES]; /*!< image valid */
	can_sync_err_t err; /*!< error counter structure for diagnostics */
} can_sync_t;

/**
 * @brief data storage buffer for synchronous process data
 * @struct can_sync_data_tag
 * @typedef struct can_sync_data_tag can_sync_data_t
 */
typedef struct can_sync_data_tag
{
	byte_t data[CAN_CONFIG_NR_OF_MODULE_OBJS][CAN_DLC_MAX]; /**< transmit data buffer */
} can_sync_data_t;

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/
extern can_sync_t can_sync; /**< can sync*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t can_sync_init(void);

li_can_slv_errorcode_t can_sync_rx_process_main(uint16_t dlc, byte_t const *can);
li_can_slv_errorcode_t can_sync_rx_data_main(uint16_t table_pos, uint16_t int_id, uint16_t dlc, byte_t *can);
li_can_slv_errorcode_t can_sync_tx_data_main(uint16_t table_pos, uint16_t obj, uint16_t dlc, byte_t *can);
li_can_slv_errorcode_t can_sync_tx_data_main_ok(uint16_t table_pos, uint16_t obj);

#ifdef LI_CAN_SLV_MON
li_can_slv_errorcode_t can_sync_rx_process_mon(uint16_t dlc, byte_t const *can);
li_can_slv_errorcode_t can_sync_rx_data_mon(uint16_t table_pos, uint16_t int_id, uint16_t dlc, byte_t *can);
li_can_slv_errorcode_t can_sync_tx_data_mon(uint16_t table_pos, uint16_t int_id, uint16_t dlc, byte_t *can);
#endif // #ifdef LI_CAN_SLV_MON

li_can_slv_errorcode_t can_sync_clr_rx_data_main_mon(uint16_t taple_pos);

void li_can_slv_sync_check_process_image(void);
uint32_t can_sync_get_pr_time_valid(void);

/**
 * @brief li_can_slv_sync_get_process_periode
 * @return current process period
 */
uint32_t li_can_slv_sync_get_process_periode(void);

li_can_slv_errorcode_t li_can_slv_sync_set_process_time_valid_fnc(char_t *type, uint32_t(**pfnc)(void));
li_can_slv_errorcode_t can_sync_set_pr_call_fnc(void(*pfnc)(void));
li_can_slv_errorcode_t li_can_slv_sync_set_process_image_valid_cbk(char_t *type, li_can_slv_module_nr_t module_number, void (*pfnc)(void));
li_can_slv_errorcode_t li_can_slv_sync_set_process_image_not_valid_cbk(char_t *type, li_can_slv_module_nr_t module_number, void (*pfnc)(void));
li_can_slv_errorcode_t li_can_slv_sync_set_process_request_cbk(char_t *type, li_can_slv_module_nr_t module_number, void (*pfnc)(void));

/**
 * @brief This functions is used to increase the period if no trigger happens
 * from the CAN IRQ. The period is limited to #LI_CAN_SLV_SYNC_PROCESS_PERIODE_MAX
 */
void li_can_slv_sync_trigger_process_periode(void);

li_can_slv_errorcode_t can_sync_set_first_process_request_call_fnc(void (*pfnc)(void));
void li_can_slv_sync_process_request_rx(void);

#endif // #ifndef IO_CAN_SYNC_H
/** @} */

