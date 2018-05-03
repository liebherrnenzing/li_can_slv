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
 * @file io_app_frc2.h
 * @brief header to io_app_frc2.c module
 * @addtogroup app_frc2
 * @{
 */

#ifndef IO_APP_FRC2_H_
#define IO_APP_FRC2_H_

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
#ifndef APP_FRC2_MODULE_NR_DEF
#define APP_FRC2_MODULE_NR_DEF 65
#endif

#ifndef APP_FRC2_MODULE_NR_MIN
#define APP_FRC2_MODULE_NR_MIN 65
#endif

#ifndef APP_FRC2_MODULE_NR_MAX
#define APP_FRC2_MODULE_NR_MAX 70
#endif

#define APP_FRC2_MODULE_TYPE	"FRC2\0"

#define ERR_APP_FRC2_MODULE_ADD_FAILED	(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_APPLICATION) |0x0fff)
#define ERR_APP_FRC2_MON_NOT_FOUND		(MSG_SG_SHIFT(MSG_SUBGROUP_CAN_APPLICATION) |0x0ffe)

#define TUPLE_ID_CNFG_DEFAULT	0xFFFF

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/
typedef int16_t app_frc2_can_tx_data_t;

extern app_frc2_can_tx_data_t app_frc2_can_tx_data;

/*!
 * \brief defines main or monitor types
 */
typedef enum
{
	APP_FRC2_TYPE_UDEF = 0, /*!<undefined type */
	APP_FRC2_TYPE_MAIN = 1, /*!<main module type */
	APP_FRC2_TYPE_MON = 2, /*!<monitor module type */
} app_frc2_type_t;

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/
extern app_frc2_can_tx_data_t app_frc2_can_tx_data;
extern volatile uint32_t app_frc2_image_valid_cnt;
extern volatile uint32_t app_frc2_process_req_cnt;
extern volatile uint32_t app_frc2_process_time_valid;

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
lcsa_errorcode_t app_frc2_init(li_can_slv_module_nr_t modnr);
lcsa_errorcode_t app_frc2_process_output(void);

void app_frc2_set_force(uint16_t force);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_APP_FRC2_H_

/** @} */
