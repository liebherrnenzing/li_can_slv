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
 * @file io_can_main_types.h
 * @brief header to io_can_main_types.h module
 * @addtogroup lcs_core_main_types
 * @{
 */

#ifndef IO_CAN_MAIN_TYPES_H_
#define IO_CAN_MAIN_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
/**
 * @brief defines the can main or monitor target
 * @enum can_mainmon_type_tag
 * @typedef enum #can_mainmon_type_tag can_mainmon_type_t
 */
typedef enum can_mainmon_type_tag
{
	CAN_MAINMON_TYPE_UNDEF = 0, /**< target not defined */
	CAN_MAINMON_TYPE_MAIN = 1, /**< main target */
	CAN_MAINMON_TYPE_MON = 2 /**< monitor target */
} can_mainmon_type_t;

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/
/*!
 * \brief define the id of can interrupt service routines
 * \enum can_main_service_id_t
 */
typedef enum
{
	CAN_MAIN_SERVICE_ID_TX = 0,
	CAN_MAIN_SERVICE_ID_RX = 1,
	CAN_MAIN_ASYNC_SERVICE_ID_TX = 2,
	CAN_MAIN_ASYNC_SERVICE_ID_RX = 3,
	CAN_MAIN_ASYNC_CTRL_SERVICE_ID_RX = 4
} can_main_service_id_t;

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_CAN_MAIN_TYPES_H_
/** @} */

