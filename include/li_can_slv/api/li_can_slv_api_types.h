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
 * @typedef lcsa_state_t
 * @brief defines the state of the stack
 */
typedef enum
{
	LI_CAN_SLV_STATE_RUNNING = 0,
	LI_CAN_SLV_STATE_DOWNLOAD,
	LI_CAN_SLV_STATE_RECONNECT,
	LI_CAN_SLV_STATE_INIT
} lcsa_state_t;

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
