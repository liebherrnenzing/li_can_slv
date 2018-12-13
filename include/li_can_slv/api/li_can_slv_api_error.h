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
 * @file li_can_slv_api_error.h
 * @brief header to li_can_slv_api_error.h module
 * @addtogroup lcs_api
 * @{
 */

#ifndef LI_CAN_SLV_API_ERROR_H_
#define LI_CAN_SLV_API_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/error/io_can_errno.h>
#include <li_can_slv/error/li_can_slv_error_types.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
/**
 * @defgroup li_can_slv_api_error_msg_group Error Message Group Defines
 * @addtogroup li_can_slv_api_error_msg_group
 * @{
 */
#define LCSA_ERROR_GROUP_CAN			(0x7) /**< group definition: CAN bus */
#define LCSA_ERROR_GROUP_SMALLSYSTEM	(0xB) /**< group definition: small system */
/**
 * @}
 */

/**
 * @defgroup li_can_slv_api_error_msg_subgroup Error Message Subgroup Defines
 * @addtogroup li_can_slv_api_error_msg_subgroup
 * @{
 */
//#define LCSA_ERROR_SUBGROUP_UNDEFINED		(0x0L) /**< undefined/unknown/unused */
//#define LCSA_ERROR_SUBGROUP_CAN_SYSTEM		(0x1L) /**< system */
#define LCSA_ERROR_SUBGROUP_CAN_APPLICATION	(0x2L) /**< application */
//#define LCSA_ERROR_SUBGROUP_CAN_BOOT		(0x3L) /**< boot */
//#define LCSA_ERROR_SUBGROUP_CAN_OS			(0x4L) /**< OS */
//#define LCSA_ERROR_SUBGROUP_CAN_CAN			(0x5L) /**< CAN */
//#define LCSA_ERROR_SUBGROUP_CAN_DRIVERS		(0x6L) /**< drivers */
//#define LCSA_ERROR_SUBGROUP_CAN_SYSCHECK	(0x7L) /**< system checks */
//#define LCSA_ERROR_SUBGROUP_CAN_UTILITIES	(0x8L) /**< utilities */
//#define LCSA_ERROR_SUBGROUP_CAN_RECORDER	(0x9L) /**< recorder */
#define LCSA_ERROR_SUBGROUP_CAN_SENSOR		(0xAL) /**< sensor */
#define LCSA_ERROR_SUBGROUP_SMALLSYSTEM_FREQUENCYCONVERTER (0x0B) /**< freqeuncyconverter */
/**
 * @}
 */

/**
 * @defgroup li_can_slv_api_error_msg_prio Error Message Priority Defines
 * @addtogroup li_can_slv_api_error_msg_prio
 * @{
 */
#define LCSA_ERROR_MSG_PRIO_INFO		ERR_LVL_INFO /**< message priority info */
#define LCSA_ERROR_MSG_PRIO_WARNING		ERR_LVL_WARNING /**< message priority warning */
#define LCSA_ERROR_MSG_PRIO_ERROR		ERR_LVL_ERROR /**< message priority error */
#define LCSA_ERROR_MSG_PRIO_CRITICAL	ERR_LVL_CRITICAL /**< message priority critical */
#define LCSA_ERROR_MSG_PRIO_FATAL		ERR_LVL_CRITICAL /**< message priority fatal */

/**
 * @}
 */

/**
 * @defgroup li_can_slv_api_error_code_defines Error Message Defines
 * @addtogroup li_can_slv_api_error_code_defines
 * @{
 */
#define LCSA_ERROR_OK								LI_CAN_SLV_ERR_OK
#define LCSA_ERROR_CONFIG_MODULE_TYPE_NOT_FOUND		ERR_MSG_CAN_CONFIG_MODULE_TYPE_NOT_FOUND
#define LCSA_ERROR_CONFIG_MODULE_NOT_FOUND			ERR_MSG_CAN_CONFIG_MODULE_NOT_FOUND
#define LCSA_ERROR_XLOAD_INFO_BAD_CRC				ERR_MSG_CAN_XLOAD_INFO_BAD_CRC
#define LCSA_ERROR_CONIFG_REDECLARED_MODULE_NR		ERR_MSG_CAN_CONFIG_REDECLARED_MODULE_NR
#define LCSA_ERROR_DLOAD_ADD_HANDLE					ERR_MSG_CAN_DLOAD_ADD_HANDLE
#define LCSA_ERROR_DLOAD_UNKNOWN_COMPONENT			ERR_MSG_CAN_DLOAD_UNKNOWN_COMPONENT
#define LCSA_ERROR_ULOAD_ADD_HANDLE					ERR_MSG_CAN_DLOAD_ADD_HANDLE
#define LCSA_ERROR_ULOAD_UNKNOWN_COMPONENT			ERR_MSG_CAN_DLOAD_UNKNOWN_COMPONENT
/**
 * @}
 */

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

#endif // #ifndef LI_CAN_SLV_API_ERROR_H_

/** @} */
