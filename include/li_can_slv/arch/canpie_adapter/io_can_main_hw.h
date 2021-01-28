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
 * @file canpie_adapter/io_can_main_hw.h
 * @brief header to io_can_main_hw.h module
 * @addtogroup lcs_arch_canpie_adapter
 * @{
 */

#ifndef IO_CAN_MAIN_HW_H_
#define IO_CAN_MAIN_HW_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/arch/io_can_main_hw_interface.h>
#include <li_can_slv/config/io_can_config_types.h>
#include <li_can_slv/config/li_can_slv_config_defines.h>
#include "canpie.h"
/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
#define CANPIE_MODE_START	(eCP_MODE_OPERATION)
#define CANPIE_MODE_STOP	(eCP_MODE_INIT)
#define CANPIE_MODE_LISTEN_ONLY	(eCP_MODE_LISTEN_ONLY)
#define CANPIE_BUFFER_DIR_TX	(eCP_BUFFER_DIR_TRM)
#define CANPIE_BUFFER_DIR_RX	(eCP_BUFFER_DIR_RCV)
/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/
extern const can_config_bdr_tab_t can_main_hw_default_baudrate_table[CAN_CONFIG_SIZE_OF_BDR_TAB];
extern CpPort_ts can_port_main;

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_CAN_MAIN_HW_H_

/** @} */
