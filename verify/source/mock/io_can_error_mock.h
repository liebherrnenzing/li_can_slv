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
 * @file io_can_error_mock.h
 * @brief header to io_error_mock.h module
 * @addtogroup lcs_mock
 * @{
 */

#ifndef IO_ERROR_MOCK_H_
#define IO_ERROR_MOCK_H_

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

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
uint16_t error_syserr_get_queue_mock_idx(void);
li_can_slv_errorcode_t error_syserr_get_from_idx(uint16_t idx, li_can_slv_module_nr_t *pmodule_nr, msg_code_t *perrnum, err_prio_t *ppriority);
#ifdef ERROR_SYSERR_ADDITIONAL_INFO_BYTE
li_can_slv_errorcode_t error_syserr_get_with_info_byte_from_idx(uint16_t idx, li_can_slv_module_nr_t *pmodule_nr, msg_code_t *perrnum, err_prio_t *ppriority, byte_t *pinfo);
#endif // #ifdef ERROR_SYSERR_ADDITIONAL_INFO_BYTE


#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_ERROR_MOCK_H_
/** @} */

