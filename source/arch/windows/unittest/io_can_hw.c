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
 * @file windows/unittest/io_can_hw.c
 * @addtogroup lcs_arch_windows
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "io_can_hw.h"
#include <stdlib.h>
#include <stdio.h>
#include "io_can_errno.h"

#include "io_can_main_hw.h"
#include "io_can_mon_hw.h"

#ifdef LI_CAN_SLV_MAIN_MON
#include "io_mainmon.h"
#endif // #ifdef LI_CAN_SLV_MAIN_MON

#include <string.h>

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/
static FILE *can_hw_log;
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

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/
static char can_hw_log_file_path[FILENAME_MAX];
static char default_name[] = "can_hw.log";

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t can_hw_init(void)
{
	strncpy(can_hw_log_file_path, default_name, sizeof(default_name));

	int ret = can_hw_log_open();

	if (ret == EXIT_FAILURE)
	{
		return ret;
	}

	if (can_hw_log != NULL)
	{
		fprintf(can_hw_log, "\ncan_hw_init");
	}

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_hw_transceiver_enable(void)
{
	if (can_hw_log != NULL)
	{
		fprintf(can_hw_log, "\ncan_hw_enable");
	}

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_hw_transceiver_disable(void)
{
	if (can_hw_log != NULL)
	{
		fprintf(can_hw_log, "\ncan_hw_disable");
	}

	return (LI_CAN_SLV_ERR_OK);
}

#if defined(OUTER) || defined(OUTER_APP)
li_can_slv_errorcode_t can_hw_combine_msg_obj_to_two_stage_fifo(uint16_t msg_obj, uint16_t msg_obj2)
{
	msg_obj2 = msg_obj2;

	if (can_hw_log != NULL)
	{
		fprintf(can_hw_log, "\ncan_hw_combine_msg_obj_to_two_stage_fifo: msg_obj = %d", msg_obj);
	}

	return (LI_CAN_SLV_ERR_OK);
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

#ifdef LI_CAN_SLV_MAIN_MON
can_mainmon_type_t can_hw_get_mainmon_type(void)
{
	return (can_mainmon_type_t) mainmon_get_type();
}
#endif // #ifdef LI_CAN_SLV_MAIN_MON

void can_hw_set_log_file_name(char *file_name)
{
	strcpy(can_hw_log_file_path, file_name);
}

int can_hw_log_open(void)
{
	can_hw_log = fopen(can_hw_log_file_path, "w");

	if (NULL == can_hw_log)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void can_hw_log_close(void)
{
	fclose(can_hw_log);
}
/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/


/** @} */
