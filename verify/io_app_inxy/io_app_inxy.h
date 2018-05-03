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
 * @file io_app_inxy.h
 * @brief header to io_app_inxy.c module
 * @addtogroup app_inxy
 * @{
 */

#ifndef IO_APP_INXY_H_
#define IO_APP_INXY_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include "io_app_port.h"

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
#ifndef APP_INXY_MODULE_NR_DEF
#define APP_INXY_MODULE_NR_DEF 79
#endif

#ifndef APP_INXY_MODULE_NR_MIN
#define APP_INXY_MODULE_NR_MIN 79
#endif

#ifndef APP_INXY_MODULE_NR_MAX
#define APP_INXY_MODULE_NR_MAX 80
#endif

#define APP_INXY_MODULE_TYPE	"INXY\0"

#define ERR_APP_INXY_MODULE_ADD_FAILED	(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_APPLICATION) |0x0ffb)
#define ERR_APP_INXY_MON_NOT_FOUND		(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_APPLICATION) |0x0ffa)

#define TUPLE_ID_CNFG_DEFAULT	0xFFFF

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/
typedef int16_t app_inxy_can_tx_data_t;

/*!
 * \brief defines main or monitor types
 */
typedef enum
{
	APP_INXY_TYPE_UDEF = 0, /*!<undefined type */
	APP_INXY_TYPE_MAIN = 1, /*!<main module type */
	APP_INXY_TYPE_MON = 2, /*!<monitor module type */
} app_inxy_type_t;

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/
extern app_inxy_can_tx_data_t app_inxy_can_tx_data[2];

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
lcsa_errorcode_t app_inxy_init(lcsa_module_number_t modnr);
lcsa_errorcode_t app_inxy_process_output(void);
void app_inxy_set_incx(uint16_t incx);
void app_inxy_set_incy(uint16_t incy);

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_APP_INXY_H_

/** @} */
