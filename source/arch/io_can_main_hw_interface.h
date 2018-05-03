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
 * @file io_can_main_hw_interface.h
 * @brief LCS main HW interface
 * @addtogroup lcs_arch
 * @{
 */

#ifndef IO_CAN_MAIN_HW_INTERFACE_H_
#define IO_CAN_MAIN_HW_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include "li_can_slv.h"
#include "io_can_types.h"
#include "io_can_main_types.h"
#include "li_can_slv_error_types.h"
#include "io_can_config_types.h"

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
/**
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
extern li_can_slv_errorcode_t can_main_hw_init(void);

/**
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
extern li_can_slv_errorcode_t can_main_hw_deinit(void);

/**
 * @brief message object init function
 * @param msg_obj number of message object
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
extern li_can_slv_errorcode_t can_main_hw_msg_obj_init(uint16_t msg_obj);

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
extern li_can_slv_errorcode_t can_main_hw_free_tx_objs(void);
#endif // #if defined(OUTER) || defined(OUTER_APP)

/**
 * @brief get the next free message object number
 * @param[out] msg_obj number of message object
 * @return #LI_CAN_SLV_ERR_OK or #ERR_MSG_CAN_NO_MSG_OBJ_FREE
 */
extern li_can_slv_errorcode_t can_main_hw_get_next_free_msg_obj(uint16_t *msg_obj);

/**
 * @brief enable main CAN-Controller
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
extern li_can_slv_errorcode_t can_main_hw_enable(void);

/**
 * @brief disable main CAN-Controller
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
extern li_can_slv_errorcode_t can_main_hw_disable(void);

#ifdef LI_CAN_SLV_RECONNECT
/**
 * @return
 */
extern li_can_slv_errorcode_t can_main_hw_enable_listen_only(void);
#endif // #ifdef LI_CAN_SLV_RECONNECT

/**
 * @param msg_obj
 * @param can_id
 * @param acceptance_mask
 * @param dlc
 * @param dir
 * @param service_id
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
extern li_can_slv_errorcode_t can_main_hw_define_msg_obj(uint16_t msg_obj, uint16_t can_id, uint16_t acceptance_mask, byte_t dlc, byte_t dir, can_main_service_id_t service_id);

/**
 * @param bdr_tab_entry baud rate table entry
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
extern li_can_slv_errorcode_t can_main_hw_set_baudrate(can_config_bdr_tab_t *bdr_tab_entry);

/**
 * @param bdr_tab_entry baud rate table entry
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
extern li_can_slv_errorcode_t can_main_hw_set_baudrate_listen_only(can_config_bdr_tab_t *bdr_tab_entry);

/**
 * @param msg_obj
 * @param can_id
 * @param dlc
 * @param src
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
extern li_can_slv_errorcode_t can_main_hw_send_msg_obj_blocking(uint16_t msg_obj, uint16_t can_id, uint16_t dlc, const volatile byte_t *src);

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @param msg_obj
 * @param can_id
 * @param dlc
 * @param src
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
extern li_can_slv_errorcode_t can_main_hw_send_msg_obj_none_blocking(uint16_t msg_obj, uint16_t can_id, uint16_t dlc, const volatile byte_t *src);

#endif // #if defined(OUTER) || defined(OUTER_APP)

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_CAN_MAIN_HW_INTERFACE_H_

/** @} */
