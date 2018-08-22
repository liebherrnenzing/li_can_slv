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
 * @file io_can_errno.h
 * @brief header to io_can_errno.h module
 * @addtogroup errno
 * @{
 */

#ifndef IO_CAN_ERRNO_H_
#define IO_CAN_ERRNO_H_

#ifdef __cplusplus
extern "C"
{
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/core/li_can_slv.h>
#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/error/li_can_slv_error_types.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/

// example => can system 0x71000008
// groug sub group msgcode
// 7     1         000008
// 0x1008 => error used on io modules

// (x>>16 | (x & 0x00000FF))


/*********************************************************/
/* groups */
/*********************************************************/
/*!
 * \name message groups
 */
/*@{*/
#define MSG_GROUP_UNDEFINED							0x00000000L /*! service code group definition: undefined/unknown/unused */
#define MSG_GROUP_CAN								0x70000000L /*!< service code group definition: CAN bus */

#define MSG_CODE_MASK								0x00FFFFFFL /*!< */
#define MSG_SUBGROUP_MASK							0x0F000000L /*!< */
#define MSG_GROUP_MASK								0xF0000000L /*!< */
/*@}*/

/*********************************************************/
/* subgroups */
/*********************************************************/
/**
 * @name message subgroups
 */
/*@{*/
#define MSG_SUBGROUP_UNDEFINED						0x00000000L /*!< service code subgroup definition: undefined/unknown/unused */
#define MSG_SUBGROUP_CAN_SYSTEM						0x01000000L /*!< service code subgroup definition for group CAN bus: system layer */
#define MSG_SUBGROUP_CAN_APPLICATION				0x02000000L /*!< service code subgroup definition for group CAN bus: application layer */
#define MSG_SUBGROUP_CAN_BOOT						0x03000000L /*!< service code subgroup definition for group CAN bus: boot system of module */
#define MSG_SUBGROUP_CAN_OS							0x04000000L /*!< service code subgroup definition for group CAN bus: OS system of module */
#define MSG_SUBGROUP_CAN_CAN						0x05000000L /*!< service code subgroup definition for group CAN bus: CAN system of module */
#define MSG_SUBGROUP_CAN_DRIVERS					0x06000000L /*!< service code subgroup definition for group CAN bus: drivers of module */
#define MSG_SUBGROUP_CAN_SYSCHECK					0x07000000L /*!< service code subgroup definition for group CAN bus: system checks of module */
#define MSG_SUBGROUP_CAN_UTILITIES					0x08000000L /*!< service code subgroup definition for group CAN bus: utilities of module */
#define MSG_SUBGROUP_CAN_RECORDER					0x09000000L /*!< service code subgroup definition for group CAN bus: recorder subsystem (e.g. SMP, MDE) of module */
#define MSG_SUBGROUP_CAN_SENSOR						0x0A000000L /*!< service code subgroup definition for group CAN bus: sensor subsystem */

#define MSG_SG_SHIFT(x) ((li_can_slv_errorcode_t)((msg_code_t)(x) >> 12)) /*!< Adopt subgroup information from 32 value to fit into 16 Bit value (module internal) */

#define MSG_SG_EXPAND_CODE(x) ((msg_code_t)(x) & ~(MSG_SUBGROUP_MASK >> 12)) /*!< Expands the given 3 digit error code */
#define MSG_SG_EXPAND_GROUP(x) (((msg_code_t)(x) << 12) & MSG_SUBGROUP_MASK) /*!< Expands the given 1 digit error sub group */
#define MSG_SG_EXPAND(x) (MSG_SG_EXPAND_GROUP(x) | MSG_SG_EXPAND_CODE(x)) /*!< Expands the given 16bit service code to 32 bit and shifts the subgroup info to correct position */
#define MSG_SG_EXTRACT(x) ((uint8_t)(((msg_code_t)(x) & MSG_SUBGROUP_MASK)>>24)) /*!< Extracts the group/subgroup information from a module-internal 16Bit service code */

#define MSG_EXTRACT_CODE(x) (((msg_code_t)(x)) & (MSG_CODE_MASK))
#define MSG_EXTRACT_SUBGROUP(x) ((byte_t)((((msg_code_t)(x)) & (MSG_SUBGROUP_MASK))>>24))
#define MSG_EXTRACT_GROUP(x) ((byte_t)((((msg_code_t)(x)) & (MSG_GROUP_MASK))>>28))
#define MSG_EXTRACT_GROUP_SUBGROUP(x) ((byte_t)((((msg_code_t)(x)) & (MSG_GROUP_MASK | MSG_SUBGROUP_MASK))>>24))
/*@}*/

#define LI_CAN_SLV_ERR_OK	(0x0000u) /**<no error occured */
#define LI_CAN_SLV_ERR_NOT_IMPLEMENTED (ERR_MSG_CAN_NOT_IMPLEMENTED)

#define MSG_CAN_BOOT_BOOT_LOADER_VERSION_MAJOR			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_BOOT) | 0x039u)
#define MSG_CAN_BOOT_BOOT_LOADER_VERSION_MINOR			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_BOOT) | 0x03Au)
#define MSG_CAN_BOOT_BOOT_LOADER_ACTIVE_WITH_INFO		(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_BOOT) | 0x03Bu)

//CAN system errors
#define ERR_MSG_CAN_ASYNC_TNL_ALREADY_USED				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x001u) /*!< CAN async tunnel already opened */
#define ERR_MSG_CAN_ASYNC_TNL_WRONG_STATE				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x002u) /*!< CAN async tunnel has wrong state for desired action */
#define ERR_MSG_CAN_MAIN_UNDEFINED_ISR_ID				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x003u) /*!< CAN main undefined id of interrupt service routine */
#define ERR_MSG_CAN_MON_UNDEFINED_ISR_ID 				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x004u) /*!< CAN monitor undefined id of interrupt service routine */
#define ERR_MSG_CAN_CONFIG_SET_INVALID_BAUDRATE			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x005u) /*!<Try to set invalid CAN baudrate.*/
#define ERR_MSG_CAN_DEFINE_OBJ_INVALID_NR				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x006u) /*!<Try to set invalid CAN message object.*/
#define ERR_MSG_CAN_DEFINE_OBJ_INVALID_ID				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x007u) /*!<Try to set invalid CAN message ID.*/
#define ERR_MSG_CAN_DEFINE_OBJ_INVALID_DLC				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x008u) /*!<Try to set invalid CAN message DLC.*/
#define ERR_MSG_CAN_DEFINE_OBJ_INVALID_DIR				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x009u) /*!<Try to set invalid CAN message DIR.*/
#define ERR_MSG_CAN_MAIN_OVER_OBJECT_NR					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x00Au) /*!<object number of maximum number of transmit objects */
#define ERR_MSG_CAN_CONFIG_REDECLARED_MODULE_TYPE		(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x00Bu) /*!<Detect redeclared module type definition.*/
#define ERR_MSG_CAN_CONFIG_MAXIMUM_NR_OF_MODULE_TYPE	(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x00Cu) /*!<Over maximum number of module type definition. */
#define ERR_MSG_CAN_CONFIG_REDECLARED_MODULE_NR			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x00Du) /*!<Detect redeclared module number. */
#define ERR_MSG_CAN_TX_SYS_MSG_QUEUE_OVERFLOW			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x00Eu) /*!<Overflow on CAN system message transmit queue detected. */
#define ERR_MSG_CAN_MAIN_TIMEOUT_PROCESS_DATA_TX		(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x00Fu) /*!<timeout while sending process data */
#define ERR_MSG_CAN_MAIN_TIMEOUT_SYS_MSG_TX				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x010u) /*!<timeout while sending system message */
#define ERR_MSG_CAN_MAIN_RX_WRONG_MODULE_NR				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x011u) /*!<different (wrong) module number received for defined module */
#define ERR_MSG_CAN_MAIN_RX_WRONG_DLC					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x012u) /*!<different (wrong) data length code received for defined module */
#define ERR_MSG_CAN_MAIN_NR_OF_PROCESS_REQUEST			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x013u) /*!<not valid number off process request receive on main CAN-controller */
#define ERR_MSG_CAN_MAIN_DLC_OF_PROCESS_REQUEST			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x014u) /*!<not valid data length code of process request received on main CAN-controller */
#define ERR_MSG_CAN_MAIN_NR_OF_RX_DATA					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x015u) /*!<not valid number off receive data on main CAN-controller */
#define ERR_MSG_CAN_MAIN_NR_OF_TX_DATA					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x016u) /*!<not valid number off transmit data on main CAN-controller */
#define ERR_MSG_CAN_MAIN_OVER_TABLE_POS					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x017u) /*!<table position over maximum number of logical modules */
#define ERR_MSG_CAN_MON_RX_WRONG_MODULE_NR				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x018u) /*!<different (wrong) module number received for defined module */

//Also defined above as FATAL error and to be held free
//#define ERR_MSG_CAN_INIT_FAILED						(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x019u) /*!<Fatal error while initializing CAN base system (25)*/

#define ERR_MSG_CAN_MON_RX_WRONG_DLC					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x01Au) /*!<different (wrong) data length code received for defined module */
#define ERR_MSG_CAN_MON_TX_WRONG_MODULE_NR				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x01Bu) /*!<different (wrong) module number received for defined module */
#define ERR_MSG_CAN_MON_TX_WRONG_DLC					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x01Cu) /*!<different (wrong) data length code received for defined module */
#define ERR_MSG_CAN_MON_NR_OF_PROCESS_REQUEST			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x01Du) /*!<not valid number off process request received on monitor CAN-controller */
#define ERR_MSG_CAN_MON_DLC_OF_PROCESS_REQUEST			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x01Eu) /*!<not valid data length code of process request received on monitor CAN-controller */
#define ERR_MSG_CAN_MON_NR_OF_RX_DATA					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x01Fu) /*!<not valid number off receive data on monitor CAN-controller */
#define ERR_MSG_CAN_MON_NR_OF_TX_DATA					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x020u) /*!<not valid number off transmit data on monitor CAN-controller */
#define ERR_MSG_CAN_MAIN_MON_DATA_RX					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x021u) /*!<received data on main and monitor CAN-Controller not identically */
#define ERR_MSG_CAN_MAIN_MON_DATA_TX					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x022u) /*!<transmited data on main and monitor CAN-Controller not identically */
#define ERR_MSG_CAN_NO_MSG_OBJ_FREE						(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x023u) /*!<no CAN message object free */
#define ERR_MSG_CAN_MAIN_RX_WRONG_SLOT					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x024u) /*!<corresponding entry in object table could not be found*/
#define ERR_MSG_CAN_MAIN_TX_WRONG_SLOT					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x025u) /*!<corresponding entry in object table could not be found*/
#define ERR_MSG_CAN_MON_RX_WRONG_SLOT					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x026u) /*!<corresponding entry in object table could not be found*/
#define ERR_MSG_CAN_MON_TX_WRONG_SLOT					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x027u) /*!<corresponding entry in object table could not be found*/
#define ERR_MSG_CAN_MAIN_MSG_OBJ_RX_DATA_CNFG			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x028u) /*!<wrong configuration on message objects for synchron transmit data on main CAN-controller */
#define ERR_MSG_CAN_MAIN_MSG_OBJ_TX_DATA_CNFG			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x029u) /*!<wrong configuration on message objects for synchron receive data on main CAN-controller */
#define ERR_MSG_CAN_MON_MSG_OBJ_RX_DATA_CNFG			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x02Au) /*!<wrong configuration on message objects for synchron receive data on monitor CAN-controller */
#define ERR_MSG_CAN_MON_MSG_OBJ_TX_DATA_CNFG			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x02Bu) /*!<wrong configuration on message objects for synchron transmit data on monitor CAN-controller */
#define ERR_MSG_CAN_CONFIG_MODULE_WRONG_TABLE_POS		(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x02Cu) /*!<given table position of configuration table is not OK */
#define ERR_MSG_CAN_ASYNC_RX_QUEUE_OVERFLOW				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x02Du) /*!<Overflow on CAN async rx queue detected. */
#define ERR_MSG_CAN_TX_ASYNC_MSG_QUEUE_OVERFLOW			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x02Eu) /*!<Overflow on CAN asynchrone message transmit queue detected. */
#define ERR_MSG_CAN_ASYNC_CTRL_TX_QUEUE_OVERFLOW		(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x02Fu) /*!<Overflow on CAN asynchrone control transmit queue detected. */
#define ERR_MSG_CAN_ASYNC_DATA_TX_QUEUE_OVERFLOW		(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x030u) /*!<Overflow on CAN asynchrone data transmit queue detected. */
#define ERR_MSG_CAN_ASYNC_DATA_TX_QUEUE_DLC				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x031u) /*!<Wrong dlc on asynchrone data transmit queue detected. */
#define ERR_MSG_CAN_CONFIG_MODULE_TYPE_NOT_FOUND		(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x032u) /*!<module type not found while changing module number */
#define ERR_MSG_CAN_CONFIG_MODULE_TYPE_NOT_MAIN			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x033u) /*!<module type not main module type while changing module number */
#define ERR_MSG_CAN_DLOAD_UNKNOWN_COMPONENT				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x034u) /*!<Unknown component received */
#define ERR_MSG_CAN_DLOAD_IN_PROGRESS					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x035u) /*!<A download is actually in progress */
#define ERR_MSG_CAN_DLOAD_BUF_FULL						(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x036u) /*!<Download data buffer is full */
#define ERR_MSG_CAN_DLOAD_BUF_OVERFLOW					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x037u) /*!<Download data buffer overflow */
#define ERR_MSG_CAN_DLOAD_BUF_UNDERFLOW					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x038u) /*!<Download data buffer underflow */
#define ERR_MSG_CAN_DLOAD_BYTE_COUNT_NOT_MATCH			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x039u) /*!<Byte counter of download does not match */
#define ERR_MSG_CAN_DLOAD_BAD_CRC						(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x03Au) /*!<Bad crc of component download */
#define ERR_MSG_CAN_INVALID_SYS_MSG_DLC					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x03Bu)  /*!<Invalid system message (dlc) received.*/
#define ERR_MSG_CAN_CONFIG_INVALID_SILENT_AWAKE_STATE	(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x03Cu)  /*!<Invalid silent/awake state */
#define ERR_MSG_CAN_CONFIG_SFTY_STAY_AWAKE				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x03Du)  /*!<SFTY module with module number 5 stays always awake */
#define ERR_MSG_CAN_RECONNECT_UNEXPECTED_BAUDRATE 		(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x03Eu) /*!< An unexpected baudrate was encountered on CAN reconnect */
#define ERR_MSG_CAN_ASYNC_TNL_INVALID_COMMAND			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x03Fu) /* An invalid CAN sync tunnel command has been received */
#define ERR_MSG_CAN_ASYNC_TNL_BUSY						(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x040u) /* CAN async tunnel queue was already busy (mutex problem) */
#define ERR_MSG_CAN_ULOAD_BLOCK_NACK					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x041u)  /*!<An uploaded block received an negative Acknowledge */
#define ERR_MSG_CAN_MAIN_SYS_RX_LOST					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x042u)  /*!<A CAN System message was lost on main CAN controller */
#define ERR_MSG_CAN_CHANGING_TO_INVALID_MODULE_NR		(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x043u) /*!<Tried to set the CAN module number to an invalid number */
#define ERR_MSG_CAN_MAIN_ASYNC_CTRL_RX_LOST				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x044u) /*!<A CAN asynchrone ctrl message was lost on main CAN controller */
#define ERR_MSG_CAN_MAIN_ASYNC_DATA_RX_LOST				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x045u) /*!<A CAN asynchrone data message was lost on main CAN controller */
#define ERR_MSG_CAN_CONFIG_CHANGE_MODULE_IN_OPERATION	(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x046u) /*!<A CAN module number change on the selected logical module is already in progress */
#define ERR_MSG_CAN_MODULE_NR_TUPLE_ID_INVALID			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x047u) /*!< While changing module number an invalid tuple ID was detected.*/
#define ERR_MSG_CAN_DLOAD_DENIED						(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x048u) /*!<Unknown component received */
#define ERR_MSG_CAN_CONNECT_STARTUP_125kB				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x052u) /*!< CAN traffic on startup detected, initialized with 125kB (maybe module reset)*/
#define ERR_MSG_CAN_CONNECT_STARTUP_250kB				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x053u) /*!< CAN traffic on startup detected, initialized with 250kB (maybe module reset)*/
#define ERR_MSG_CAN_CONNECT_STARTUP_500kB				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x054u) /*!< CAN traffic on startup detected, initialized with 500kB (maybe module reset)*/
#define ERR_MSG_CAN_CONNECT_STARTUP_1000kB				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x055u) /*!< CAN traffic on startup detected, initialized with 1000kB (maybe module reset)*/
#define ERR_MSG_CAN_CONNECT_ONLINE_125kB				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x056u) /*!< problem in CAN traffic detected while online, initialized with 125kB (problem might be in master not in this module)*/
#define ERR_MSG_CAN_CONNECT_ONLINE_250kB				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x057u) /*!< problem in CAN traffic detected while online, initialized with 250kB (problem might be in master not in this module)*/
#define ERR_MSG_CAN_CONNECT_ONLINE_500kB				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x058u) /*!< problem in CAN traffic detected while online, initialized with 500kB (problem might be in master not in this module)*/
#define ERR_MSG_CAN_CONNECT_ONLINE_1000kB				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x059u) /*!< problem in CAN traffic detected while online, initialized with 1000kB (problem might be in master not in this module)*/
#define ERR_MSG_CAN_ASYNC_UNDEFINED_CONNECTION_TYPE		(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x05Au)
#define ERR_MSG_CAN_CONFIG_MODULE_NOT_FOUND				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x05Bu)
#define ERR_MSG_CAN_STATE_MACHINE_ERROR 				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x05Cu) /*!<Wrong state machine command order for hex download */

#define ERR_MSG_CAN_DLOAD_FLASH_WRITE_MAIN				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x05Du) /*!< \brief message 0x7500005D */
#define ERR_MSG_CAN_DLOAD_FLASH_WRITE_MON				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x05Eu) /*!< \brief message 0x7500005E */
#define ERR_MSG_CAN_DLOAD_FLASH_READ_MAIN				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x05Fu) /*!< \brief message 0x7500005F */
#define ERR_MSG_CAN_DLOAD_FLASH_READ_MON				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x060u) /*!< \brief message 0x75000060 */
#define ERR_MSG_CAN_DLOAD_FLASH_ERASE_MAIN				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x061u) /*!< \brief message 0x75000061 */
#define ERR_MSG_CAN_DLOAD_FLASH_ERASE_MON				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x062u) /*!< \brief message 0x75000062 */
#define ERR_MSG_CAN_DLOAD_MAINMON_CONNECTION			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x063u) /*!< \brief message 0x75000062 */

#define ERR_MSG_CAN_ASYNC_UNKNOWN_COMMAND				(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x064u) /**< Unknown asynchrony command received on module */
#define ERR_MSG_CAN_SYS_UNKNOWN_COMMAND					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x065u) /**< Unknown system command received on module */
#define ERR_MSG_CAN_MODULE_TYPE_CHANGE_FAILED			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x066u)
#define ERR_MSG_CAN_BAUD_RATE_CONFIG_INVALID			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x067u)
#define ERR_MSG_CAN_NOT_IMPLEMENTED 					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x068u)
#define ERR_MSG_CAN_SYNC_PROCESS_IMAGE_NOT_VALID		(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x06Au)
#define ERR_MSG_CAN_MODULE_NR_CONFIGURATION_FAILED		(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x06Bu)
#define ERR_MSG_CAN_SYSTEM_MSG_NOT_ALLOWED_BROADCAST	(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x06Cu)

#define ERR_MSG_CAN_ASYNC_TUNNEL_HNDL_NOT_DEFINED		(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x06Du)
#define ERR_MSG_CAN_SYSTEM_MSG_NOT_IMPLEMENTED			(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x06Eu)
#define ERR_MSG_CAN_SYS_ERR_QUEUE_FULL					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x06Fu)
#define ERR_MSG_CAN_ERR_NOT_INITIALIZED					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x070u)
#define ERR_MSG_CAN_ERR_NO_ENTRY						(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x071u)
#define ERR_MSG_CAN_DLOAD_ADD_HANDLE 					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x072u)
#define ERR_MSG_CAN_ULOAD_ADD_HANDLE			 		(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x073u)
#define ERR_MSG_CAN_XLOAD_INFO_BAD_CRC					(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x074u)

#define ERR_MSG_CAN_SYSTEM_MSG_NOT_ALLOWED_DIRECT		(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_CAN) | 0x075u)

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/
#if defined(OUTER) || defined(OUTER_APP)
extern const char_t *error_can_subgroup_text[16];
#endif // #if defined(OUTER) || defined(OUTER_APP)

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_CAN_ERRNO_H_

/** @} */
