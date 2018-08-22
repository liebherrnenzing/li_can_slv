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
 * @file io_can_config_mock.h
 * @brief header to io_can_config_mock.h module
 * @addtogroup lcs_mock
 * @{
 */

#ifndef IO_CAN_CONFIG_MOCK_H_
#define IO_CAN_CONFIG_MOCK_H_

#ifdef __cplusplus
extern "C" {
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

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/
uint8_t can_config_set_critical_error;

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t can_config_mock_init(void);
li_can_slv_errorcode_t can_config_mock_force_channel(char_t *type, li_can_slv_module_nr_t mod_nr, uint16_t channel, void *value);
li_can_slv_errorcode_t can_config_mock_read_channel(char_t *type, li_can_slv_module_nr_t mod_nr, uint16_t channel, void *value);

char_t *can_config_mock_get_type(void);
//crc16_t can_config_mock_get_crc(void);
uint32_t can_config_mock_get_date(void);
void can_config_set_ioconfig_set_error(li_can_slv_errorcode_t err);

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_CAN_CONFIG_MOCK_H_
/** @} */

