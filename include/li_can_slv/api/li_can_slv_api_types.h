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
 * @file li_can_slv_api_types.h
 * @brief header to li_can_slv_api_types.h module
 * @addtogroup lcs_api
 * @{
 */

#ifndef LI_CAN_SLV_API_TYPES_H_
#define LI_CAN_SLV_API_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/config/io_can_config_types.h>
#include <li_can_slv/error/li_can_slv_error_types.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/
/**
 * @brief defines the state of the stack
 * @enum lcsa_state_t
 */
typedef enum
{
	LI_CAN_SLV_STATE_RUNNING = 0, /**< stack is in running mode */
	LI_CAN_SLV_STATE_DOWNLOAD, /**< stack is in download or after download mode */
	LI_CAN_SLV_STATE_RECONNECT, /**< stack is in reconnect mode */
	LI_CAN_SLV_STATE_INIT, /**< stack is in init mode */
	LI_CAN_SLV_STATE_STOPPED /**< stack is stopped */
} lcsa_state_t;

/**
 * @brief The error flags for the process_image_not_valid_cbk
 * @enum lcsa_can_sync_err_flag_t
 * @addtogroup li_can_slv_api_types
 * @{
 */
typedef enum
{
	LI_CAN_SLV_SYNC_ERR_FLAG_NO_ERR		 			= 0x00000000,	/**< Everything is fine */
	LI_CAN_SLV_SYNC_ERR_FLAG_MAIN_RX_DLC			= 0x00000001,	/**< Wrong DLC for RX sync object on MAIN controller (ERR_MSG_CAN_MAIN_RX_WRONG_DLC) */
	LI_CAN_SLV_SYNC_ERR_FLAG_MON_RX_DLC 			= 0x00000004,	/**< Wrong DLC for RX sync object on MON controller (ERR_MSG_CAN_MON_RX_WRONG_DLC) */
	LI_CAN_SLV_SYNC_ERR_FLAG_MON_TX_DLC 			= 0x00000008,	/**< Wrong DLC for TX sync object on MON controller (ERR_MSG_CAN_MON_TX_WRONG_DLC) */
	LI_CAN_SLV_SYNC_ERR_FLAG_MAIN_MON_RX_DATA		= 0x00000010,	/**< Received data on MAIN and MON not identically for RX sync object (ERR_MSG_CAN_MAIN_MON_DATA_RX) */
	LI_CAN_SLV_SYNC_ERR_FLAG_MAIN_MON_TX_DATA		= 0x00000020,	/**< Transmitted data on MAIN and MON not identically for TX sync object (ERR_MSG_CAN_MAIN_MON_DATA_TX) */
	LI_CAN_SLV_SYNC_ERR_FLAG_MAIN_RX_MISSING_OBJ 	= 0x00000100,	/**< Missing RX sync data object on MAIN (ERR_MSG_CAN_MAIN_NR_OF_RX_DATA) */
	LI_CAN_SLV_SYNC_ERR_FLAG_MON_RX_MISSING_OBJ  	= 0x00000400,	/**< Missing RX sync data object on MON (ERR_MSG_CAN_MON_NR_OF_RX_DATA) */
	LI_CAN_SLV_SYNC_ERR_FLAG_MON_TX_MISSING_OBJ  	= 0x00000800	/**< Missing TX sync data object on MON (ERR_MSG_CAN_MON_NR_OF_TX_DATA) */
} lcsa_can_sync_err_flag_t;
/**
 * @}
 */

/**
 * @defgroup li_can_slv_api_types Typedefs
 * @addtogroup li_can_slv_api_types
 * @{
 */
typedef li_can_slv_config_module_t lcsa_module_config_t; /**< module configuration type */
typedef li_can_slv_module_nr_t lcsa_module_number_t;  /**< module number type */
typedef li_can_slv_module_type_t lcsa_module_type_t;  /**< module type type */
typedef li_can_slv_config_bdr_t lcsa_bdr_t; /**< baud rate type */
typedef can_config_module_silent_t lcsa_module_response_sate_t; /**< state if an module is responsive or silent on CAN messages*/
/**
 * @}
 */

/**
 * @defgroup li_can_slv_api_types_error Typedefs
 * @addtogroup li_can_slv_api_types_error
 * @{
 */
typedef li_can_slv_errorcode_t lcsa_errorcode_t;  /**< error code type */
typedef li_can_slv_msg_code_t lcsa_msg_code_t;  /**< message code type */
typedef li_can_slv_msg_add_info_t lcsa_msg_add_info_t; /**< message additional info type */
typedef li_can_slv_msg_priority_t lcsa_msg_priority_t; /**< message priority type */
typedef li_can_slv_msg_group_t lcsa_msg_group_t; /**< message group type */
typedef li_can_slv_msg_subgroup_t lcsa_msg_subgroup_t; /**< message subgroup type */
/**
 * @}
 */

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef LI_CAN_SLV_API_TYPES_H_

/** @} */
