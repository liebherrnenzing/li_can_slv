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
 * @file windows/unittest/io_can_mon_hw.c
 * @addtogroup lcs_arch_windows
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "io_can_mon_hw.h"
#include <li_can_slv/error/io_can_errno.h>
#include <li_can_slv/config/io_can_config_types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/
static FILE *can_mon_hw_log;
static char can_mon_hw_log_file_path[FILENAME_MAX];
static char default_name[] = "can_mon_hw.log";
static uint8_t can_mon_msg_obj_used[LI_CAN_SLV_MON_NODE_MAX_NOF_MSG_OBJ];

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t can_mon_hw_init(void)
{
	(void) strncpy(can_mon_hw_log_file_path, default_name, sizeof(default_name));

	int ret = can_mon_hw_log_open();

	if (ret == EXIT_FAILURE)
	{
		return ret;
	}

	fprintf(can_mon_hw_log, "\ncan_mon_hw_init");

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_mon_hw_deinit(void)
{
	can_mon_hw_log_close();
	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_mon_hw_msg_obj_init(uint16_t msg_obj)
{
	if (can_mon_hw_log != NULL)
	{
		fprintf(can_mon_hw_log, "\ncan_mon_hw_msg_obj_init: msg_obj = %d", msg_obj);
	}

	can_mon_msg_obj_used[msg_obj] = FALSE;

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_mon_hw_get_next_free_msg_obj(uint16_t *msg_obj)
{
	uint16_t i;

	if (can_mon_hw_log != NULL)
	{
		fprintf(can_mon_hw_log, "\ncan_mon_hw_get_next_free_msg_obj");
		fprintf(can_mon_hw_log, "msg_obj = %d", *msg_obj);
	}

	for (i = 0; i < LI_CAN_SLV_MON_NODE_MAX_NOF_MSG_OBJ; i++)
	{
		if (can_mon_msg_obj_used[i] == FALSE)
		{
			// the object is free;
			*msg_obj = i;
			return (LI_CAN_SLV_ERR_OK);
		}
	}

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_mon_hw_enable(void)
{
	if (can_mon_hw_log != NULL)
	{
		fprintf(can_mon_hw_log, "\ncan_mon_hw_enable");
	}

	return LI_CAN_SLV_ERR_OK;
}

li_can_slv_errorcode_t can_mon_hw_disable(void)
{
	if (can_mon_hw_log != NULL)
	{
		fprintf(can_mon_hw_log, "\ncan_mon_hw_disable");
	}

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_mon_hw_define_msg_obj(uint16_t msg_obj, uint16_t can_id, uint16_t acceptance_mask, byte_t dlc, byte_t dir, can_mon_service_id_t service_id)
{
	if (can_mon_hw_log != NULL)
	{
		fprintf(can_mon_hw_log, "\ncan_mon_hw_define_msg_obj: msg_obj = %d, can_id = %d, ", msg_obj, can_id);
		fprintf(can_mon_hw_log, "acceptance_mask = %d, dlc = %d, dir = %d, can_main_service_id = %d", acceptance_mask, dlc, dir, service_id);
	}

	// set message object to used state
	can_mon_msg_obj_used[msg_obj] = TRUE;

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_mon_hw_set_baudrate(can_config_bdr_tab_t *bdr_tab_entry)
{
	uint16_t baudrate_register;

	if (can_mon_hw_log != NULL)
	{
		fprintf(can_mon_hw_log, "\ncan_mon_hw_set_baudrate");
		fprintf(can_mon_hw_log, "\nbdr:%d, prescaler:%d", bdr_tab_entry->baudrate, bdr_tab_entry->baudrate_prescaler);
		fprintf(can_mon_hw_log, "\ntime1:%d, time2:%d", bdr_tab_entry->phase_time_segment_one, bdr_tab_entry->phase_time_segment_two);
		fprintf(can_mon_hw_log, "\nprop:%d, rjw:%d, specific:%d", bdr_tab_entry->propagation_segment, bdr_tab_entry->resync_jump_width, bdr_tab_entry->plattform_specific);
	}

	/* on the XC164, XC167 and XE167 device the plattform_specific byte is used for DIV8 */
	baudrate_register = (((uint16_t) bdr_tab_entry->plattform_specific) << 15) | (((uint16_t) bdr_tab_entry->phase_time_segment_two) << 12) | (((uint16_t) bdr_tab_entry->phase_time_segment_one) << 8)
	                    | (((uint16_t) bdr_tab_entry->resync_jump_width) << 6) | (((uint16_t) bdr_tab_entry->baudrate_prescaler) << 0);

	if (can_mon_hw_log != NULL)
	{
		fprintf(can_mon_hw_log, "\nbaudraet_regiser:%04x", baudrate_register);
	}

	return (LI_CAN_SLV_ERR_OK);
}

void can_mon_hw_set_log_file_name(char *file_name)
{
	(void) strcpy(can_mon_hw_log_file_path, file_name);
}

int can_mon_hw_log_open(void)
{
	can_mon_hw_log = fopen(can_mon_hw_log_file_path, "w+");

	if (NULL == can_mon_hw_log)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void can_mon_hw_log_close(void)
{
	(void) fclose(can_mon_hw_log);
}

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/** @} */
