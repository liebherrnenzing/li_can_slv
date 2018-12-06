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
 * @file io_can_error.h
 * @brief header to io_can_error.h module
 * @addtogroup lcs_error_core
 * @{
 */

#ifndef IO_CAN_ERROR_H_
#define IO_CAN_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/error/li_can_slv_error_types.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
#ifndef ERROR_SYSERR_QUEUE_NUMENT
#define ERROR_SYSERR_QUEUE_NUMENT	16 /*!<number of possible application error queue entries*/
#endif // #ifndef ERROR_SYSERR_QUEUE_NUMENT

#ifndef ERROR_APPERR_QUEUE_NUMENT
#define ERROR_APPERR_QUEUE_NUMENT	16 /*!<number of possible application error queue entries*/
#endif // #ifndef ERROR_APPERR_QUEUE_NUMENT

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR

#ifdef SYSERR_QUEUE
li_can_slv_errorcode_t error_init(void);
li_can_slv_errorcode_t error_syserr_get_diag(uint16_t idx, msg_code_t *pmsg_code);
#endif // #ifdef SYSERR_QUEUE

#if defined(ERROR_SYSERR_ADDITIONAL_INFO_BYTE) || defined(LI_CAN_SLV_SYS_MODULE_ERROR)
li_can_slv_errorcode_t error_syserr_send_with_info_byte(li_can_slv_errorcode_t errnum, err_prio_t priority, li_can_slv_module_nr_t module_nr, err_prio_t priority_redundant, byte_t info);
#endif // #if defined(ERROR_SYSERR_ADDITIONAL_INFO_BYTE) || defined(LI_CAN_SLV_SYS_MODULE_ERROR)

li_can_slv_errorcode_t error_syserr_send_full(msg_code_t msg_code, byte_t add_info, li_can_slv_msg_priority_t priority, byte_t group, byte_t subgroup, li_can_slv_module_nr_t module_nr);
li_can_slv_errorcode_t error_syserr_send(li_can_slv_errorcode_t errnum, err_prio_t priority, li_can_slv_module_nr_t module_nr, err_prio_t priority_redundant);
li_can_slv_errorcode_t error_syserr_get_full(msg_code_t *pmsg_code, byte_t *padd_info, err_prio_t *ppriority, byte_t *pgroup_subgroup, li_can_slv_module_nr_t *pmodule_nr);
li_can_slv_errorcode_t error_syserr_get_with_info_byte(li_can_slv_module_nr_t *pmodule_nr, li_can_slv_errorcode_t *perrnum, err_prio_t *ppriority, byte_t *pinfo);
li_can_slv_errorcode_t error_syserr_get(li_can_slv_module_nr_t *pmodule_nr, li_can_slv_errorcode_t *perrnum, err_prio_t *ppriority);
uint16_t error_syserr_num(void);

#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR

#if defined(ERROR_APPLICATION) && defined(SYSERR_QUEUE)
li_can_slv_errorcode_t error_apperr_get_diag(uint16_t idx, msg_code_t *perrnum, err_apperr_state_t *perrstate);
uint16_t error_apperr_num(void);
li_can_slv_errorcode_t error_apperr_send(li_can_slv_errorcode_t errnum, err_apperr_state_t errstate, li_can_slv_module_nr_t module_nr);
li_can_slv_errorcode_t error_apperr_get(msg_code_t *perrnum, err_apperr_state_t *perrstate);
#endif // #if defined(ERROR_APPLICATION) && defined(SYSERR_QUEUE)

#ifdef ERROR_SUBGROUP_TEXT
const char_t *error_get_subgroup_text(uint8_t subgroup);
#endif // #ifdef ERROR_SUBGROUP_TEXT

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_CAN_ERROR_H_

/** @} */
