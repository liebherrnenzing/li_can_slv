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
 * @file io_can_config.h
 * @brief header to IO CAN CONFIG module
 * @addtogroup can_config
 * @{
 */

#ifndef IO_CAN_CONFIG_H_
#define IO_CAN_CONFIG_H_

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include "li_can_slv.h"

#include "io_can_types.h"
#include "li_can_slv_error_types.h"
#include "io_can_config_types.h"

#include "li_can_slv_core_defines.h"
#include "li_can_slv_config_defines.h"

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
/* CAN message object configuration of main CAN-controller (process request) */
#define CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS	CAN_MSG_OBJ0 /*!< used CAN message object for receiving process request on main controller */

/* CAN message object configuration of monitor CAN-controller (process request) */
#define CAN_CONFIG_MSG_MON_OBJ_RX_PROCESS	CAN_MSG_OBJ1 /*!< used CAN message object for receiving process request on monitor controller */

/* CAN message object configuration of main CAN-controller (system messages)*/
#define CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS1		CAN_MSG_OBJ2 /*!< used CAN message object for receiving system message (broadcast/module) */
#if defined(OUTER) || defined(OUTER_APP)
#define CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS2		CAN_MSG_OBJ3 /*!< used CAN message object for receiving system message (broadcast/module) */
#endif // #if defined(OUTER) || defined(OUTER_APP)

#if defined(LI_CAN_SLV_MON) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY_SYSTEM_REQ)
#define CAN_CONFIG_MSG_MON_OBJ_RX_SYS1			CAN_MSG_OBJ4
#endif // #if defined(LI_CAN_SLV_MON) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY_SYSTEM_REQ)

#define CAN_CONFIG_MSG_MAIN_OBJ_TX_SYS			CAN_MSG_OBJ9 /*!< used CAN message object for transmitting system message */

#ifdef LI_CAN_SLV_ASYNC
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_DATA_RX	CAN_MSG_OBJ6 /*!< used CAN message object for receiving asynchronous data */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_TX		CAN_MSG_OBJ7 /*!< used CAN message object for transmitting asynchronous control data and asynchronous data */

/* CAN message object configuration of main CAN-controller (asynchronous objects) */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL1_RX	(LI_CAN_SLV_MAIN_NODE_MAX_NOF_MSG_OBJ - 1) /*!< used CAN message object for receiving asynchronous control data (logical module 1) */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL2_RX	(LI_CAN_SLV_MAIN_NODE_MAX_NOF_MSG_OBJ - 2) /*!< used CAN message object for receiving asynchronous control data (logical module 2) */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL3_RX	(LI_CAN_SLV_MAIN_NODE_MAX_NOF_MSG_OBJ - 3) /*!< used CAN message object for receiving asynchronous control data (logical module 3) */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL4_RX	(LI_CAN_SLV_MAIN_NODE_MAX_NOF_MSG_OBJ - 4) /*!< used CAN message object for receiving asynchronous control data (logical module 4) */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL5_RX	(LI_CAN_SLV_MAIN_NODE_MAX_NOF_MSG_OBJ - 5) /*!< used CAN message object for receiving asynchronous control data (logical module 5) */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL6_RX	(LI_CAN_SLV_MAIN_NODE_MAX_NOF_MSG_OBJ - 6) /*!< used CAN message object for receiving asynchronous control data (logical module 6) */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL7_RX	(LI_CAN_SLV_MAIN_NODE_MAX_NOF_MSG_OBJ - 7) /*!< used CAN message object for receiving asynchronous control data (logical module 7) */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL8_RX	(LI_CAN_SLV_MAIN_NODE_MAX_NOF_MSG_OBJ - 8) /*!< used CAN message object for receiving asynchronous control data (logical module 8) */
#endif // #ifdef LI_CAN_SLV_ASYNC

/**
 * @todo on a main monitor system this is not right
 */
#ifndef LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES
#define LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES	1
#endif // #ifndef LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES

/**
 * @todo obj max number for compact module and future usage this should be fixed
 */
#define CAN_CONFIG_SYNC_MAIN_MAX_NR_OF_RX_OBJ	20 /**< (LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES * CAN_CONFIG_NR_OF_MODULE_OBJS) */
#define CAN_CONFIG_SYNC_MAIN_MAX_NR_OF_TX_OBJ	LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES /**< */

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES < 2
#define CAN_CONFIG_MSG_MAIN_OBJ_TX_NR			1 /**< */
#else // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES < 2
#define CAN_CONFIG_MSG_MAIN_OBJ_TX_NR			2 /**< */
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES < 2

/** @todo check the following 2 define usage, etc.. */
#define CAN_CONFIG_SYNC_MON_MAX_NR_OF_RX_OBJ	CAN_CONFIG_SYNC_MAIN_MAX_NR_OF_RX_OBJ /**< */
#define CAN_CONFIG_SYNC_MON_MAX_NR_OF_TX_OBJ	16 /**< */

#define CAN_CONFIG_ACCEPTANCE_ONE_ID	0x07FF /*!< acceptance mask is set to 1 identifier */
#define CAN_CONFIG_ACCEPTANCE_MASK		0x0603 /**< */

/* CAN Liebherr protocol definitions */

/* process request */
#define CAN_CONFIG_PROCESS_ID	0x0001 /*!< CAN identifier of process request */
#define CAN_CONFIG_PROCESS_DLC	LI_CAN_SLV_CONFIG_DLC_USED_1 /*!< DLC of process request */

#define CAN_CONFIG_ID_NULL	0x0000 /*!< CAN identifier null */

/* system messages */
#define CAN_CONFIG_SYS_MSG_ID	0x0002 /*!< CAN identifier of system message */
#define CAN_CONFIG_SYS_MSG_DLC	LI_CAN_SLV_CONFIG_DLC_USED_8 /*!< DLC of system message */
#define CAN_CONFIG_SYS_TX_MASK	0x0003 /*!< transmit system message mask */

#ifdef LI_CAN_SLV_ASYNC
/* asynchron control data */
#define CAN_CONFIG_ASYNC_CTRL_TX_SLAVE_ID	0x0601 /*!< CAN identifier of asynchronous control data tx slave */
#define CAN_CONFIG_ASYNC_CTRL_TX_MASTER_ID	0x0600 /*!< CAN identifier of asynchronous control data tx master */
#define CAN_CONFIG_ASYNC_CTRL_TX_DLC		LI_CAN_SLV_CONFIG_DLC_USED_8	/*!< DLC of asynchronous control data tx */

/* asynchron data */
#define CAN_CONFIG_ASYNC_DATA_TX_SLAVE_ID	0x0603 /*!< CAN identifier of asynchronous data tx slave */
#define CAN_CONFIG_ASYNC_DATA_TX_MASTER_ID	0x0602 /*!< CAN identifier of asynchronous data tx master */
#define CAN_CONFIG_ASYNC_DATA_TX_DLC		LI_CAN_SLV_CONFIG_DLC_USED_8 /*!< DLC of asynchronous data tx */
#define CAN_CONFIG_ASYNC_DATA_RX_DLC		LI_CAN_SLV_CONFIG_DLC_USED_8 /*!< DLC of asynchronous data rx */

#define CAN_CONFIG_ASYNC_CTRL_RX_SLAVE_ID	0x0600u /**< CAN identifier of asynchronous control data rx slave */
#define CAN_CONFIG_ASYNC_CTRL_RX_MASTER_ID	0x0601u /**< CAN identifier of asynchronous control data rx master */
#define CAN_CONFIG_ASYNC_DATA_RX_SLAVE_ID	0x0602u /**< CAN identifier of asynchronous data rx slave */
#define CAN_CONFIG_ASYNC_DATA_RX_MASTER_ID	0x0603u /**< CAN identifier of asynchronous data rx master */
#define CAN_CONFIG_ASYNC_CTRL_RX_DLC		LI_CAN_SLV_CONFIG_DLC_USED_8 /**< DLC of asynchronous control data rx */
#define CAN_CONFIG_ASYNC_MSG_DLC			LI_CAN_SLV_CONFIG_DLC_USED_8 /**< */
#endif // #ifdef LI_CAN_SLV_ASYNC

/* synchronous io data */
#define CAN_CONFIG_DATA_TX_MASK		0x0400u /*!< transmit synchronous process data mask TX 1024 */
#define CAN_CONFIG_DATA_RX_MASK		0x0200u /*!< receive synchronous process data mask RX 512 */

#define CAN_CONFIG_ONLINE_ARB_ID		0x0000 /*!< CAN identifier is online changing on concerning CAN message object */
#define CAN_CONFIG_ONLINE_DLC			LI_CAN_SLV_CONFIG_DLC_USED_0 /*!< DLC is online changing on concerning CAN message object */
#define CAN_CONFIG_MODULE_NR_BROADCAST	1u /*!< Module number for broadcast message */

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/
#ifndef LI_CAN_SLV_BOOT
extern uint16_t can_config_nr_of_modules;
#endif // #ifndef LI_CAN_SLV_BOOT

#ifdef LI_CAN_SLV_BOOT
extern li_can_slv_config_module_t can_config_module_tab;
#endif // #ifndef LI_CAN_SLV_BOOT

#if (defined(OUTER) || defined(OUTER_APP))
extern li_can_slv_config_module_t can_config_module_tab[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES];
#endif // #if (defined(OUTER) || defined(OUTER_APP))

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_BOOT
#ifdef LI_CAN_SLV_TUPLE
li_can_slv_errorcode_t can_config_add_module(char_t *module_type, li_can_slv_module_nr_t module_nr, tuple_id_t tuple_id_module_nr);
#else // #ifdef LI_CAN_SLV_TUPLE
li_can_slv_errorcode_t can_config_add_module(char_t *module_type, li_can_slv_module_nr_t module_nr);
#endif // #ifdef LI_CAN_SLV_TUPLE

li_can_slv_module_nr_t can_config_get_module_nr(void);
#define can_config_get_module_nr_main()	can_config_get_module_nr()
#endif // #ifdef LI_CAN_SLV_BOOT

#if defined(LI_CAN_SLV_BOOT) && defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR)
li_can_slv_errorcode_t can_config_change_module_nr(li_can_slv_module_nr_t module_nr);
#endif // #if defined(LI_CAN_SLV_BOOT) && defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR

#if (defined(OUTER) || defined(OUTER_APP)) && defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR)
li_can_slv_errorcode_t can_config_change_module_nr(uint16_t table_pos, li_can_slv_module_nr_t module_nr);
#endif // #if (defined(OUTER) || defined(OUTER_APP)) && defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR)

#ifdef LI_CAN_SLV_BOOT
li_can_slv_errorcode_t can_config_get_module_silent_awake(can_config_module_silent_t *module_silent_awake);
li_can_slv_errorcode_t can_config_set_module_silent_awake(can_config_module_silent_t  module_silent_awake);
#else // #ifndef LI_CAN_SLV_BOOT
li_can_slv_errorcode_t can_config_get_module_silent_awake(uint16_t table_pos, can_config_module_silent_t *module_silent_awake);
li_can_slv_errorcode_t can_config_set_module_silent_awake(uint16_t table_pos, can_config_module_silent_t  module_silent_awake);
#endif // #ifdef LI_CAN_SLV_BOOT

#if defined(OUTER) || defined(OUTER_APP)
li_can_slv_errorcode_t can_config_init(void);
void can_config_init_module_nr_main(li_can_slv_module_nr_t module_nr);
li_can_slv_errorcode_t can_config_get_module_type(uint16_t table_pos, char_t *module_type);
li_can_slv_errorcode_t can_config_get_module_nr(uint16_t table_pos, li_can_slv_module_nr_t *module_nr);

/**
 * @brief get the main module number
 * @return #li_can_slv_module_nr_t
 */
li_can_slv_module_nr_t can_config_get_module_nr_main(void);
void can_config_get_nr_of_modules(uint16_t *nr_of_modules);

/**
 * @brief check if the chosen module is valid
 * @param type module type
 * @param table_pos position in configuration table
 * @return #LI_CAN_SLV_ERR_OK or #ERR_MSG_CAN_CONFIG_MODULE_TYPE_NOT_FOUND
 */
li_can_slv_errorcode_t can_config_module_type_valid(char_t *type, uint16_t *table_pos);
li_can_slv_errorcode_t can_config_module_valid(char_t *type, li_can_slv_module_nr_t module_nr, uint16_t *table_pos);
#endif // #if defined(OUTER) || defined(OUTER_APP)

#if defined(OUTER) || defined(APP) || defined(OUTER_APP)
li_can_slv_errorcode_t can_config_get_module_nr_min(uint16_t table_pos, li_can_slv_module_nr_t *module_nr_min);
li_can_slv_errorcode_t can_config_get_module_nr_max(uint16_t table_pos, li_can_slv_module_nr_t *module_nr_max);

li_can_slv_errorcode_t can_config_add_module(const li_can_slv_config_module_t *module, li_can_slv_module_nr_t module_nr, void *rx0, void *rx1, void *rx2, void *rx3, void *tx0, void *tx1, void *tx2, void *tx3);

#ifdef LI_CAN_SLV_UNKNOWN_MODULE
li_can_slv_errorcode_t can_config_add_ukwn_module(li_can_slv_module_type_t *module_type, li_can_slv_module_nr_t module_nr);
#endif // #ifdef LI_CAN_SLV_UNKNOWN_MODULE

li_can_slv_errorcode_t can_config_get_module(li_can_slv_config_module_t **module, char_t *type);

/**
 * @brief switch of synchrony process of defined module
 * @param[in] type is the module type
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_set_critical(char_t *type);
li_can_slv_errorcode_t can_config_sync_on(uint16_t table_pos);
void can_config_sync_off(uint16_t table_pos);

#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
li_can_slv_errorcode_t can_config_get_ioconfig(uint16_t table_pos, can_config_ioconfig_t **ioconfig);
li_can_slv_errorcode_t can_config_set_ioconfig(char_t *type, crc16_t crc, uint32_t date);
li_can_slv_errorcode_t can_config_call_ioconfig_set(char_t *type);
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG

li_can_slv_errorcode_t can_config_get_module_nr_by_type(li_can_slv_module_nr_t *modnr, char_t *type);

li_can_slv_errorcode_t can_config_change_module_nr_next(void);
li_can_slv_errorcode_t can_config_change_module_nr_all(uint16_t table_pos, li_can_slv_module_nr_t module_nr);
li_can_slv_errorcode_t can_config_change_module_nr_set(can_config_module_nr_set_t *module_nr_set, char_t *type);

li_can_slv_errorcode_t can_config_set_module_silent_awake_call_fnc(void (*pfnc)(uint16_t table_pos, char_t *type, li_can_slv_module_nr_t module_nr, can_config_module_silent_t module_silent_awake));
#endif // #if defined(OUTER) || defined(APP) || defined(OUTER_APP)

li_can_slv_errorcode_t can_config_set_baudrate(can_config_bdr_t baudrate);
#ifdef LI_CAN_SLV_RECONNECT
li_can_slv_errorcode_t can_config_set_baudrate_listen_only(can_config_bdr_t baudrate);
#endif // #ifdef LI_CAN_SLV_RECONNECT

li_can_slv_errorcode_t can_config_set_baudrate_table(void);
li_can_slv_errorcode_t can_config_set_baudrate_default(void);
li_can_slv_errorcode_t can_config_get_baudrate(can_config_bdr_t *baudrate);
bool_t can_config_is_baudrate_valid(can_config_bdr_t baudrate);

void can_config_set_baudrate_startup(li_can_slv_config_bdr_t baud_rate);
li_can_slv_config_bdr_t can_config_get_baudrate_startup(void);

#ifdef LI_CAN_SLV_BOOT
li_can_slv_errorcode_t can_config_module_nr_valid(li_can_slv_module_nr_t module_nr, can_config_module_silent_t *module_silent, uint16_t *module_found);
#else // #ifdef LI_CAN_SLV_BOOT
li_can_slv_errorcode_t can_config_module_nr_valid(li_can_slv_module_nr_t module_nr, uint16_t *table_pos, can_config_module_silent_t *module_silent, uint16_t *module_found);
#endif // #ifdef LI_CAN_SLV_BOOT

#ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR
li_can_slv_errorcode_t li_can_slv_config_change_module_nr_and_identifiers(char_t *module_type, li_can_slv_module_nr_t new_module_nr, li_can_slv_module_nr_t actual_module_nr);
void li_can_slv_config_set_module_nr_change_voter(lcsa_module_change_voter_state_t (*pfnc)(char_t *module_type, li_can_slv_module_nr_t new_module_nr, li_can_slv_module_nr_t act_module_nr));
li_can_slv_errorcode_t li_can_slv_config_set_module_silent_awake_from_type_and_nr(char_t *module_type, li_can_slv_module_nr_t module_nr, can_config_module_silent_t desired_state);
#endif // #ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR

#endif // #ifndef IO_CAN_CONFIG_H_
/** @} */
