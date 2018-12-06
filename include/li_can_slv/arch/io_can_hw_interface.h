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
 * @file io_can_hw_interface.h
 * @brief LCS HW interface
 * @addtogroup lcs_arch
 * @{
 */

#ifndef IO_CAN_HW_INTERFACE_H_
#define IO_CAN_HW_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>

#include <li_can_slv/core/io_can_types.h>
#ifdef LI_CAN_SLV_MAIN_MON
#include <li_can_slv/core/io_can_main_types.h>
#endif // #ifdef LI_CAN_SLV_MAIN_MON
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
/*!
 * \brief can Hardware init routine
 * \return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_hw_init(void);

/*!
 * \brief can Hardware enable routine
 * \return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_hw_transceiver_enable(void);

/*!
 * \brief can Hardware disable routine
 * \return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_hw_transceiver_disable(void);

#ifdef LI_CAN_SLV_MAIN_MON
can_mainmon_type_t can_hw_get_mainmon_type(void);
#endif // #ifdef LI_CAN_SLV_MAIN_MON

#if defined(OUTER) || defined(OUTER_APP)
/*!
 * \brief combine two message objects to FIFO
 * \param msg_obj first message object
 * \param msg_obj2 second message object
 * \return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
extern li_can_slv_errorcode_t can_hw_combine_msg_obj_to_two_stage_fifo(uint16_t msg_obj, uint16_t msg_obj2);
#endif // #if defined(OUTER) || defined(OUTER_APP)

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_CAN_HW_INTERFACE_H_

/** @} */
