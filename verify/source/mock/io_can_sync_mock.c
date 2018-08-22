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
 * @file io_can_sync_mock.c
 * @brief
 * @addtogroup lcs_mock
 * @{
 */
/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/sync/io_can_sync.h>
#include <stdio.h>
#include <li_can_slv/config/io_can_config.h>

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_MON
//static can_sync_data_t can_sync_data_main_tx[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES] = {0}; /**< */
//static can_sync_data_t can_sync_data_mon_rx[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES] = {0}; /**< */
#endif // #ifdef LI_CAN_SLV_MON

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* public (exported) functions                                              */
/*--------------------------------------------------------------------------*/
/**
 * @brief mock function: set process time by type
 * @param type module type
 * @param pr_time time to set
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_sync_mock_set_pr_time_by_type(char_t *type, uint32_t pr_time)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t table_pos;

	err = can_config_module_type_valid(type, &table_pos);
	if (err != LI_CAN_SLV_ERR_OK)
	{
		return (err);
	}

	can_sync.pr_time_valid_module[table_pos] = pr_time;

	return (err);
}
/** @} */

