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
 * @file io_can_mon_hw_inline.h
 * @brief LCS mon inline functions
 * @addtogroup lcs_arch
 * @{
 */

#ifndef IO_CAN_MON_HW_INLINE_H_
#define IO_CAN_MON_HW_INLINE_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include "io_can_port.h"

#include <li_can_slv/core/io_can_mon.h>
#include <li_can_slv/error/io_can_errno.h>

#include <li_can_slv/error/li_can_slv_error_types.h>

#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_debug.h"
#endif // #ifdef LI_CAN_SLV_DEBUG

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
#ifndef CAN_NODE_B_USED_FOR_RECONNECT_ONLY
/*!
 * \brief tries to find the table entry and message object number for the given mon sync rx slots
 * \param[in] rxi bit to check for correspondence
 * \param[out] msg_obj message object number of the searched slot
 * \param[out] table_pos table position of the searched slot
 * \return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
CAN_PORT_STATIC_INLINE li_can_slv_errorcode_t can_mon_synchron_rx(uint32_t rxi, uint16_t *msg_obj, uint16_t *table_pos)
{
	uint16_t i;

	for (i = 0; i < can_mon_rx_msg_obj_used; i++)
	{
		if ((rxi & can_mon_rx_msg_obj[i].msg_obj_mask) != 0)
		{
			*table_pos = can_mon_rx_msg_obj[i].table_pos;
			*msg_obj = can_mon_rx_msg_obj[i].msg_obj;
			return (LI_CAN_SLV_ERR_OK);
		}
	}
	*table_pos = 0;
	*msg_obj = 0;
	return (ERR_MSG_CAN_MON_RX_WRONG_SLOT);
}

/*!
 * \brief tries to find the table entry and message object number for the given mon sync tx slots
 * \param[in] txi bit to check for correspondence
 * \param[out] msg_obj message object number of the searched slot
 * \param[out] table_pos table position of the searched slot
 * \return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
CAN_PORT_STATIC_INLINE li_can_slv_errorcode_t can_mon_synchron_tx(uint32_t txi, uint16_t *msg_obj, uint16_t *table_pos)
{
	uint16_t i;

	for (i = 0; i < can_mon_tx_msg_obj_used; i++)
	{
		if ((txi & can_mon_tx_msg_obj[i].msg_obj_mask) != 0)
		{
			*table_pos = can_mon_tx_msg_obj[i].table_pos;
			*msg_obj = can_mon_tx_msg_obj[i].msg_obj;
			return (LI_CAN_SLV_ERR_OK);
		}
	}
	*table_pos = 0;
	*msg_obj = 0;
	return (ERR_MSG_CAN_MON_TX_WRONG_SLOT);
}
#endif // #ifndef CAN_NODE_B_USED_FOR_RECONNECT_ONLY

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_CAN_MON_HW_INLINE_H_

/** @} */
