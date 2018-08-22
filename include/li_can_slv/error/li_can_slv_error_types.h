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
 * @file li_can_slv_error_types.h
 * @brief header to li_can_slv_error_types.h module
 * @addtogroup lcs_error_types
 * @{
 */

#ifndef LI_CAN_SLV_ERROR_TYPES_H_
#define LI_CAN_SLV_ERROR_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/core/li_can_slv.h>


/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR_16_BIT
typedef	uint16_t errorcode_t; /*!< data type error code(16 Bit)*/
typedef	errorcode_t li_can_slv_errorcode_t; /*!< data type error code(16 Bit)*/
typedef	uint32_t msg_code_t; /*!< data type for message number (32 Bit)*/
typedef	msg_code_t li_can_slv_msg_code_t; /*!< data type for message number (32 Bit)*/
typedef byte_t li_can_slv_msg_add_info_t;
typedef byte_t li_can_slv_msg_group_t;
typedef byte_t li_can_slv_msg_subgroup_t;
#else // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR_16_BIT
typedef	uint16_t errorcode_t; /*!< data type error code(16 Bit)*/
typedef	errorcode_t li_can_slv_errorcode_t; /*!< data type error code(16 Bit)*/
typedef	uint32_t msg_code_t; /*!< data type for message number (32 Bit)*/
typedef	msg_code_t li_can_slv_msg_code_t; /*!< data type for message number (32 Bit)*/
typedef byte_t li_can_slv_msg_add_info_t;
typedef byte_t li_can_slv_msg_group_t;
typedef byte_t li_can_slv_msg_subgroup_t;
#endif // LI_CAN_SLV_SYS_MODULE_ERROR_16_BIT

/*!
 * \brief defines the possible error priorities
 * \remarks The error levels define the reaction of the error handler system (if any) beside of entering the error to the error queue\n
 * \pre	The error levels have to match the PME1 MASTER message system levels
 * \test The error levels have to be checked against PME1 MASTER message levels to consistency.
*/
typedef enum
{
	ERR_LVL_UNDEFINED = 0, /*!< undefined error level */
	ERR_LVL_UNAWARE = 1, /*!< error level for unaware information*/
	ERR_LVL_DEBUG_MORE = 2, /*!< error level for more debug information*/
	ERR_LVL_DEBUG = 3, /*!< error level for normal debug information*/
	ERR_LVL_INFO = 4, /*!< error level for information messages*/
	ERR_LVL_NOTICE = 6, /*!< error level notice */
	ERR_LVL_WARNING = 8, /*!< error level for warning messages */
	ERR_LVL_ERROR = 10, /*!< error level for normal error messages */
	ERR_LVL_SYSTEM = 12, /*!< error level for system error messages */
	ERR_LVL_CRITICAL = 13, /*!< error level for critical error messages (application shutdown)*/
	ERR_LVL_ALERT = 14, /*!< error level for alerts (no transmission via CAN )*/
	ERR_LVL_FATAL = 15 /*!< error level for fatal system errors (no transmission via CAN) */
} err_prio_t;

typedef err_prio_t li_can_slv_msg_priority_t;

/*!
 * \brief defines the possible states of application errors
*/
typedef enum
{
	ERR_STATE_GOING = 0, /*!< application error has gone now (falling edge) */
	ERR_STATE_COMING = 1, /*!< application error has been detected (rising edge)*/
	ERR_STATE_ACK = 2 /*!< application error has been acknowledged*/
} err_apperr_state_t;

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

#endif // #ifndef LI_CAN_SLV_ERROR_TYPES_H_
/** @} */

