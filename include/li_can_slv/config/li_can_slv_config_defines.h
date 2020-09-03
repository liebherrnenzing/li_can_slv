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
 * @file li_can_slv_config_defines.h
 * @brief header to li_can_slv_config_defines.h module
 * @addtogroup can_config
 * @{
 */

#ifndef LI_CAN_SLV_CONFIG_DEFINES_H_
#define LI_CAN_SLV_CONFIG_DEFINES_H_

#ifdef __cplusplus
extern "C" {
#endif /*  #ifdef __cplusplus */

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>
#include <li_can_slv/core/li_can_slv_core_defines.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
/* CAN message object configuration of main CAN-controller (process request) */
#ifndef CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS
#define CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS	CAN_MSG_OBJ0 /*!< used CAN message object for receiving process request on main controller */
#endif // #ifndef CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS

/* CAN message object configuration of monitor CAN-controller (process request) */
#define CAN_CONFIG_MSG_MON_OBJ_RX_PROCESS	CAN_MSG_OBJ1 /*!< used CAN message object for receiving process request on monitor controller */

/* CAN message object configuration of main CAN-controller (system messages)*/
#define CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS1	CAN_MSG_OBJ2 /*!< used CAN message object for receiving system message (broadcast/module) */
#if defined(OUTER) || defined(OUTER_APP)
#ifdef LI_CAN_SLV_SYS_OBJ2
#define CAN_CONFIG_MSG_MAIN_OBJ_RX_SYS2	CAN_MSG_OBJ3 /*!< used CAN message object for receiving system message (broadcast/module) */
#endif // #ifdef LI_CAN_SLV_SYS_OBJ2
#endif // #if defined(OUTER) || defined(OUTER_APP)

#if defined(LI_CAN_SLV_MON) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY_SYSTEM_REQ)
#define CAN_CONFIG_MSG_MON_OBJ_RX_SYS1	CAN_MSG_OBJ4
#endif // #if defined(LI_CAN_SLV_MON) || defined(CAN_NODE_B_USED_FOR_RECONNECT_ONLY_SYSTEM_REQ)

#ifndef CAN_CONFIG_MSG_MAIN_OBJ_TX_SYS
#define CAN_CONFIG_MSG_MAIN_OBJ_TX_SYS	CAN_MSG_OBJ9 /*!< used CAN message object for transmitting system message */
#endif // #ifndef CAN_CONFIG_MSG_MAIN_OBJ_TX_SYS

#ifdef LI_CAN_SLV_ASYNC
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_DATA_RX	CAN_MSG_OBJ6 /*!< used CAN message object for receiving asynchronous data */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_TX		CAN_MSG_OBJ7 /*!< used CAN message object for transmitting asynchronous control data and asynchronous data */

/* CAN message object configuration of main CAN-controller (asynchronous objects) */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL1_RX	(0) /*!< used CAN message object for receiving asynchronous control data (logical module 1) */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL2_RX	(1) /*!< used CAN message object for receiving asynchronous control data (logical module 2) */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL3_RX	(2) /*!< used CAN message object for receiving asynchronous control data (logical module 3) */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL4_RX	(3) /*!< used CAN message object for receiving asynchronous control data (logical module 4) */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL5_RX	(4) /*!< used CAN message object for receiving asynchronous control data (logical module 5) */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL6_RX	(5) /*!< used CAN message object for receiving asynchronous control data (logical module 6) */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL7_RX	(6) /*!< used CAN message object for receiving asynchronous control data (logical module 7) */
#define CAN_CONFIG_MSG_MAIN_OBJ_ASYNC_CTRL8_RX	(7) /*!< used CAN message object for receiving asynchronous control data (logical module 8) */
#endif // #ifdef LI_CAN_SLV_ASYNC

#ifndef LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES
#define LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES	1
#endif // #ifndef LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES < 2
#define CAN_CONFIG_MSG_MAIN_OBJ_TX_NR			1 /**< */
#else // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES < 2
#define CAN_CONFIG_MSG_MAIN_OBJ_TX_NR			2 /**< */
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES < 2

#define CAN_CONFIG_SYNC_MAIN_MAX_NR_OF_RX_OBJ	(LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES * CAN_CONFIG_NR_OF_MODULE_OBJS)
#define CAN_CONFIG_SYNC_MAIN_MAX_NR_OF_TX_OBJ	CAN_CONFIG_MSG_MAIN_OBJ_TX_NR /**< */

#define CAN_CONFIG_SYNC_MON_MAX_NR_OF_RX_OBJ	CAN_CONFIG_SYNC_MAIN_MAX_NR_OF_RX_OBJ /**< */

/** @todo check 16 something like this should be used maybe (LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES * CAN_CONFIG_NR_OF_MODULE_OBJS) */
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

#define CAN_CONFIG_TYPE_STRING_LENGTH	5 /*!< type string length inclusive 0-character*/

#define LI_CAN_SLV_CONFIG_OBJ_USED_NOT	0 /**< */
#define LI_CAN_SLV_CONFIG_OBJ_USED_1	1 /**< */
#define LI_CAN_SLV_CONFIG_OBJ_USED_2	2 /**< */
#define LI_CAN_SLV_CONFIG_OBJ_USED_3	3 /**< */
#define LI_CAN_SLV_CONFIG_OBJ_USED_4	4 /**< */

#define LI_CAN_SLV_CONFIG_DLC_USED_0		0 /**< */
#define LI_CAN_SLV_CONFIG_DLC_USED_1		1 /**< */
#define LI_CAN_SLV_CONFIG_DLC_USED_2		2 /**< */
#define LI_CAN_SLV_CONFIG_DLC_USED_3		3 /**< */
#define LI_CAN_SLV_CONFIG_DLC_USED_4		4 /**< */
#define LI_CAN_SLV_CONFIG_DLC_USED_5		5 /**< */
#define LI_CAN_SLV_CONFIG_DLC_USED_6		6 /**< */
#define LI_CAN_SLV_CONFIG_DLC_USED_7		7 /**< */
#define LI_CAN_SLV_CONFIG_DLC_USED_8		8 /**< */

#define CAN_CONFIG_MODULE_TYPE_NOT_USED	"\0\0\0\0" /*!< module type string for not used table position */
#define CAN_CONFIG_MODULE_TYPE_UNKNOWN	"UKWN\0" /*!< module type string for unknown module type */
#define LI_CAN_SLV_CONFIG_DEF_MOD_NR1		125u /*!< default module number1 */
#define LI_CAN_SLV_CONFIG_DEF_MOD_NR2		126u /*!< default module number2 */
#define LI_CAN_SLV_CONFIG_DEF_MOD_NR3		127u /*!< default module number3 */
#define LI_CAN_SLV_CONFIG_DEF_MOD_NR4		127u /*!< default module number4 */
#define LI_CAN_SLV_CONFIG_DEF_MOD_NR5		127u /*!< default module number5 */
#define LI_CAN_SLV_CONFIG_DEF_MOD_NR6		127u /*!< default module number6 */
#define LI_CAN_SLV_CONFIG_DEF_MOD_NR7		127u /*!< default module number7 */
#define LI_CAN_SLV_CONFIG_DEF_MOD_NR8		127u /*!< default module number8 */

#define CAN_CONFIG_DIR_TX	(CAN_DIR_TX) /*!< CAN direction transmit */
#define CAN_CONFIG_DIR_RX	(CAN_DIR_RX) /*!< CAN direction receive */

#define CAN_OBJECT_IS_SYNC	1 /*!< Indicates that a given message object shall serve as synchronous data object */
#define CAN_OBJECT_NOT_SYNC	0 /*!< Indicates that a given message object is not a synchronous data object */

/* CAN baud rate definitions */
#define CAN_CONFIG_DEF_BDR	250u /*!< default baud rate */
#define LI_CAN_SLV_CONFIG_BAUD_RATE_DEFAULT	CAN_CONFIG_DEF_BDR /*!< default baud rate 250kBaud */
#define LI_CAN_SLV_CONFIG_BAUD_RATE_125K	125u /*!< 125 kBaud */
#define LI_CAN_SLV_CONFIG_BAUD_RATE_250K	250u /*!< 250 kBaud */
#define LI_CAN_SLV_CONFIG_BAUD_RATE_500K	500u /*!< 500 kBaud */
#define LI_CAN_SLV_CONFIG_BAUD_RATE_1M		1000u /*!< 1000 kBaud */

#define CAN_CONFIG_SIZE_OF_BDR_TAB 4u /*!< size of baud rate table */

#define CAN_CONFIG_NR_OF_MODULE_OBJS 4u /*!< number of module msg objects */

#define CAN_CONFIG_MIN_MODULE_NR	2u /*!< minimum valid module number */
#define CAN_CONFIG_MAX_MODULE_NR	124u /*!< maximum valid module number */

#define LI_CAN_SLV_CONFIG_TABLE_POS_MAIN_MODULE 0u

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
}/* closing brace for extern "C" */
#endif /* #ifdef __cplusplus */

#endif /* #ifndef LI_CAN_SLV_CONFIG_DEFINES_H_ */
/** @} */

