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
 * @file io_can_config_types.h
 * @brief header to io_can_config_types.h module
 * @addtogroup lcs_config_types
 * @{
 */

#ifndef IO_CAN_CONFIG_TYPES_H_
#define IO_CAN_CONFIG_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>
#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/error/li_can_slv_error_types.h>
#include <li_can_slv/config/li_can_slv_config_defines.h>
#if defined (LI_CAN_SLV_TUPLE) || defined (LI_CAN_SLV_MODULE_IO_CONFIG)
#include "io_tuple.h"
#endif // #if defined (LI_CAN_SLV_TUPLE) || defined (LI_CAN_SLV_MODULE_IO_CONFIG)

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/
/**
 * @brief define the baud rate type
 * @typedef can_config_bdr_t
 */
typedef uint16_t can_config_bdr_t;
typedef can_config_bdr_t li_can_slv_config_bdr_t; /**< internal baud rate type */

/**
 * @brief define the CAN baud rate table
 * @struct can_config_bdr_tab_tag
 * @typedef struct can_config_bdr_tab_tag can_config_bdr_tab_t
 */
typedef struct can_config_bdr_tab_tag
{
	can_config_bdr_t baudrate; /**< */
	uint8_t baudrate_prescaler;
	uint8_t propagation_segment;
	uint8_t resync_jump_width;
	uint8_t phase_time_segment_one;
	uint8_t phase_time_segment_two;
	uint8_t plattform_specific;
} can_config_bdr_tab_t;

#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
/**
 * @brief define the module io configuration infos
 * @struct can_config_ioconfig_tag
 * @typedef struct can_config_ioconfig_tag can_config_ioconfig_t
 */
typedef struct can_config_ioconfig_tag
{
	tuple_id_t tuple_id; /*!< tuple id for io configuration */
	crc16_t crc; /*!< crc of io configuration */
	uint32_t date; /*!< date of io configuration */
	li_can_slv_errorcode_t (*set)(void); /*!< set function for io configuration */
} can_config_ioconfig_t;
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG

/**
 * @brief define the module io configuration info
 * @struct can_config_module_nr_set_tag
 * @typedef struct can_config_module_nr_set_tag can_config_module_nr_set_t
 */
typedef struct can_config_module_nr_set_tag
{
#ifdef LI_CAN_SLV_TUPLE
	tuple_id_t tuple_id; /*!< tuple id for io configuration */
#endif // #ifdef LI_CAN_SLV_TUPLE
	li_can_slv_module_nr_t nr; /*!< module number */
	li_can_slv_module_nr_t min; /*!< minimum of module number */
	li_can_slv_module_nr_t max; /*!< maximum of module number */
} can_config_module_nr_set_t;

/**
 * @brief define the can module configuration type
 * @struct can_config_module_tag
 * @typedef struct can_config_module_tag li_can_slv_config_module_t
 */
typedef struct can_config_module_tag
{
	char_t type[CAN_CONFIG_TYPE_STRING_LENGTH]; /*!< module type string with '0'-char */
	li_can_slv_module_nr_t module_nr;
#ifndef LI_CAN_SLV_BOOT
#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
	can_config_ioconfig_t ioconfig; /*!< io configuration info */
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG
	byte_t rx_obj; /*!< number of receive objects of logical module (master to module) */
	byte_t tx_obj; /*!< number of transmit objects of logical module (module to master) */
	byte_t rx_dlc[CAN_CONFIG_NR_OF_MODULE_OBJS]; /*!< data length codes of receive objects (master to module) */
	byte_t tx_dlc[CAN_CONFIG_NR_OF_MODULE_OBJS]; /*!< data length codes of transmit objects (module to master) */
	void *rx[CAN_CONFIG_NR_OF_MODULE_OBJS]; /*!< data pointer to rx data of modules (from master) */
	void *tx[CAN_CONFIG_NR_OF_MODULE_OBJS]; /*!< data pointer to tx data of modules (to master) */
	li_can_slv_errorcode_t (*rx_main[CAN_CONFIG_NR_OF_MODULE_OBJS])(byte_t *can, void *app, uint16_t dlc); /*!< pointer to main rx conversion code (from master) */
	li_can_slv_errorcode_t (*tx_main[CAN_CONFIG_NR_OF_MODULE_OBJS])(byte_t *can, void *app, uint16_t dlc); /*!< pointer to main tx conversion code (to master) */

#ifdef LI_CAN_SLV_MAIN_MON
	li_can_slv_errorcode_t (*tx_main_rx_mon[CAN_CONFIG_NR_OF_MODULE_OBJS])(byte_t *can, void *app, uint16_t dlc); /*!< pointer to tx main rx monitor conversion code */
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
	li_can_slv_errorcode_t (*rx_mon[CAN_CONFIG_NR_OF_MODULE_OBJS])(byte_t *can, void *app, uint16_t dlc); /*!< pointer to monitor rx conversion code */
	li_can_slv_errorcode_t (*tx_mon[CAN_CONFIG_NR_OF_MODULE_OBJS])(byte_t *can, void *app, uint16_t dlc); /*!< pointer to monitor tx conversion code */
#endif // #ifdef LI_CAN_SLV_MON

	uint16_t rx_msg_obj_main[CAN_CONFIG_NR_OF_MODULE_OBJS]; /*!< */

#ifdef LI_CAN_SLV_MAIN_MON
	uint16_t tx_main_rx_msg_obj_mon[CAN_CONFIG_NR_OF_MODULE_OBJS]; /**< tx main rx msg obj monitor */
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
	uint16_t rx_msg_obj_mon[CAN_CONFIG_NR_OF_MODULE_OBJS]; /**< rx msg obj monitor */
	uint16_t tx_msg_obj_mon[CAN_CONFIG_NR_OF_MODULE_OBJS]; /**< tx msg obj monitor */
#endif // #ifdef LI_CAN_SLV_MON

	byte_t rx_obj_sync; /*!< number of receive objects of logical module */
	byte_t tx_obj_sync; /*!< number of transmit objects of logical module */
	byte_t rx_dlc_sync[CAN_CONFIG_NR_OF_MODULE_OBJS]; /*!< data length codes of receive objects */
	byte_t tx_dlc_sync[CAN_CONFIG_NR_OF_MODULE_OBJS]; /*!< data length codes of transmit objects */
	li_can_slv_errorcode_t (*rx_main_sync[CAN_CONFIG_NR_OF_MODULE_OBJS])(byte_t *can, void *app, uint16_t dlc); /*!< pointer to main rx conversion code */
	li_can_slv_errorcode_t (*tx_main_sync[CAN_CONFIG_NR_OF_MODULE_OBJS])(byte_t *can, void *app, uint16_t dlc); /*!< pointer to main tx conversion code */

#ifdef LI_CAN_SLV_MAIN_MON
	li_can_slv_errorcode_t (*tx_main_rx_mon_sync[CAN_CONFIG_NR_OF_MODULE_OBJS])(byte_t *can, void *app, uint16_t dlc); /*!< pointer to tx main rx monitor conversion code */
#endif // #ifdef LI_CAN_SLV_MAIN_MON

#ifdef LI_CAN_SLV_MON
	li_can_slv_errorcode_t (*rx_mon_sync[CAN_CONFIG_NR_OF_MODULE_OBJS])(byte_t *can, void *app, uint16_t dlc); /*!< pointer to monitor rx conversion code */
	li_can_slv_errorcode_t (*tx_mon_sync[CAN_CONFIG_NR_OF_MODULE_OBJS])(byte_t *can, void *app, uint16_t dlc); /*!< pointer to monitor tx conversion code */
#endif // #ifdef LI_CAN_SLV_MON

#endif // #ifndef LI_CAN_SLV_BOOT
} li_can_slv_config_module_t;

/**
 * @brief can_config_module_silent type definition
 * @enum can_config_module_silent_tag
 * @typedef enum can_config_module_silent_tag can_config_module_silent_t
 */
typedef enum can_config_module_silent_tag
{
	LI_CAN_SLV_CONFIG_MODULE_STATE_SILENT = 0, /**< SILENT -> 0 */
	LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE = 1 /**< AWAKE -> 1*/
} can_config_module_silent_t;

#ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR
/**
 * @brief return type for the module change callback voter
 * @typedef  lcsa_module_change_voter_state_t
 */
typedef enum
{
	LCSA_MODULE_CHANGE_VALID = 0, /**< change will proceed and change the identifiers for rx and tx messages accordingly */
	LCSA_MODULE_CHANGE_ERROR, /**< change will be skipped and an error send */
	LCSA_MODULE_CHANGE_FORBIDDEN,  /**< change will be skipped and an error send */
	LCSA_MODULE_CHANGE_VALID_BUT_SET_SILENT /**< change will be proceeded the identifiers changed but the module will not respond to any message until an reboot or an AWAKE command */
} lcsa_module_change_voter_state_t;
#endif // #ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR

#ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_TYPE
/**
 * @brief define the can module configuration type
 * @struct can_config_module_tag
 * @typedef struct can_config_module_tag can_config_module_t
 */
typedef struct can_config_change_module_type_tag
{
	const li_can_slv_config_module_t *can_config_module;
	void *rx[CAN_CONFIG_NR_OF_MODULE_OBJS]; /**< data pointer to rx data of modules */
	void *tx[CAN_CONFIG_NR_OF_MODULE_OBJS]; /**< data pointer to tx data of modules */
	void (*callback)(char_t *module_type);
} can_config_change_module_type_t;
#endif // #ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_TYPE

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_CAN_CONFIG_TYPES_H_
/** @} */

