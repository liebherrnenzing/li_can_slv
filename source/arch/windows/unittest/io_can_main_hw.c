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
 * @file windows/unittest/io_can_main_hw.c
 * @addtogroup lcs_arch_windows
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "io_can_main_hw.h"
#include "io_can_hw.h"

#include <li_can_slv/core/io_can_main.h>
#include <li_can_slv/sys/io_can_sys.h>
#include <li_can_slv/sync/io_can_sync.h>
#include <li_can_slv/async/io_can_async.h>

#include <li_can_slv/error/io_can_error.h>
#include <li_can_slv/error/io_can_errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_debug.h"
#endif // #ifdef LI_CAN_SLV_DEBUG

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/
/* CAN warning level */
#define CAN_MAIN_WARNING_LEVEL					96 /**< warning level */

const can_config_bdr_tab_t can_main_hw_default_baudrate_table[CAN_CONFIG_SIZE_OF_BDR_TAB] = /**< baud rate table */
{
	{CAN_CONFIG_DEF_BDR,	0, 0, 0, 0, 0, 0},
	{125,					0, 0, 0, 0, 0, 0},
	{500,   				0, 0, 0, 0, 0, 0},
	{1000,					0, 0, 0, 0, 0, 0}
};

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/
typedef struct
{
	uint8_t msg_type;
	uint8_t module_type[4];
	struct
	{
		uint8_t serial_byte_0;
		uint8_t serial_byte_1;
		uint8_t serial_byte_2;
	} serial;
} status_ack_t;

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/
static FILE *can_main_hw_log;
static char can_main_hw_log_file_path[FILENAME_MAX];
static char default_name[] = "can_main_hw.log";
static uint8_t msg_obj_used[LI_CAN_SLV_MAIN_NODE_MAX_NOF_MSG_OBJ];

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t can_main_hw_init(void)
{
	(void) strncpy(can_main_hw_log_file_path, default_name, sizeof(can_main_hw_log_file_path));

	int ret = can_main_hw_log_open();

	if (ret == EXIT_FAILURE)
	{
		return ret;
	}

	fprintf(can_main_hw_log, "\ncan_main_hw_init");

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_main_hw_deinit(void)
{
	can_main_hw_log_close();
	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_main_hw_msg_obj_init(uint16_t msg_obj)
{
	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, "\ncan_main_hw_msg_obj_init: msg_obj = %d", msg_obj);
	}

	msg_obj_used[msg_obj] = FALSE;

	return (LI_CAN_SLV_ERR_OK);
}

#if defined(OUTER) || defined(OUTER_APP)
li_can_slv_errorcode_t can_main_hw_free_tx_objs(void)
{
	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, "\ncan_main_hw_free_tx_objs");
	}
	return LI_CAN_SLV_ERR_OK;
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

li_can_slv_errorcode_t can_main_hw_get_next_free_msg_obj(uint16_t *msg_obj)
{
	uint16_t i;

	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, "\ncan_main_hw_get_next_free_msg_obj");
		fprintf(can_main_hw_log, "msg_obj = %d", *msg_obj);
	}

	for (i = 0; i < LI_CAN_SLV_MAIN_NODE_MAX_NOF_MSG_OBJ; i++)
	{
		if (msg_obj_used[i] == FALSE)
		{
			// the object is free;
			*msg_obj = i;
			return (LI_CAN_SLV_ERR_OK);
		}
	}

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_main_hw_enable(void)
{
	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, "\ncan_main_hw_enable");
	}

	return (LI_CAN_SLV_ERR_OK);
}

#if defined(OUTER) || defined(OUTER_APP)
li_can_slv_errorcode_t can_main_hw_disable(void)
{
	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, "\ncan_main_hw_disable");
	}

	return (LI_CAN_SLV_ERR_OK);
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

li_can_slv_errorcode_t can_main_hw_define_msg_obj(uint16_t msg_obj, uint16_t can_id, uint16_t acceptance_mask, byte_t dlc, byte_t dir, can_main_service_id_t service_id)
{
	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, "\ncan_main_hw_define_msg_obj: msg_obj = %d, can_id = %d, ", msg_obj, can_id);
		fprintf(can_main_hw_log, "acceptance_mask = %d, dlc = %d, dir = %d, can_main_service_id = %d", acceptance_mask, dlc, dir, service_id);
	}

	// set message object to used state
	msg_obj_used[msg_obj] = TRUE;

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_main_hw_set_baudrate(can_config_bdr_tab_t *bdr_tab_entry)
{
	uint16_t baudrate_register;

	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, "\ncan_main_hw_set_baudrate");
		fprintf(can_main_hw_log, "\nbdr:%d, prescaler:%d", bdr_tab_entry->baudrate, bdr_tab_entry->baudrate_prescaler);
		fprintf(can_main_hw_log, "\ntime1:%d, time2:%d", bdr_tab_entry->phase_time_segment_one, bdr_tab_entry->phase_time_segment_two);
		fprintf(can_main_hw_log, "\nprop:%d, rjw:%d, specific:%d", bdr_tab_entry->propagation_segment, bdr_tab_entry->resync_jump_width, bdr_tab_entry->plattform_specific);
	}

	/* on the XC164, XC167 and XE167 device the plattform_specific byte is used for DIV8 */
	baudrate_register = (((uint16_t) bdr_tab_entry->plattform_specific) << 15) | (((uint16_t) bdr_tab_entry->phase_time_segment_two) << 12) | (((uint16_t) bdr_tab_entry->phase_time_segment_one) << 8)
	                    | (((uint16_t) bdr_tab_entry->resync_jump_width) << 6) | (((uint16_t) bdr_tab_entry->baudrate_prescaler) << 0);

	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, "\nbaudraet_regiser:%04x", baudrate_register);
	}

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_main_hw_set_baudrate_listen_only(can_config_bdr_tab_t *bdr_tab_entry)
{
	uint16_t baudrate_register;

	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, "\ncan_main_hw_set_baudrate listen only");
		fprintf(can_main_hw_log, "\nbdr:%d, prescaler:%d", bdr_tab_entry->baudrate, bdr_tab_entry->baudrate_prescaler);
		fprintf(can_main_hw_log, "\ntime1:%d, time2:%d", bdr_tab_entry->phase_time_segment_one, bdr_tab_entry->phase_time_segment_two);
		fprintf(can_main_hw_log, "\nprop:%d, rjw:%d, specific:%d", bdr_tab_entry->propagation_segment, bdr_tab_entry->resync_jump_width, bdr_tab_entry->plattform_specific);
	}

	/* on the XC164, XC167 and XE167 device the plattform_specific byte is used for DIV8 */
	baudrate_register = (((uint16_t) bdr_tab_entry->plattform_specific) << 15) | (((uint16_t) bdr_tab_entry->phase_time_segment_two) << 12) | (((uint16_t) bdr_tab_entry->phase_time_segment_one) << 8)
	                    | (((uint16_t) bdr_tab_entry->resync_jump_width) << 6) | (((uint16_t) bdr_tab_entry->baudrate_prescaler) << 0);

	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, "\nbaudraet_regiser:%04x", baudrate_register);
	}

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_main_hw_send_msg_obj_blocking(uint16_t msg_obj, uint16_t can_id, uint16_t dlc, const volatile byte_t *src)
{
	uint16_t i;
	status_ack_t *p_status_ack;
	char module_type[5];

	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, "blocking: msg_obj = %d, can_id = 0x%X, dlc = %d, ", msg_obj, can_id, dlc);
	}

	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, " ");

		for (i = 0; i < dlc; i++)
		{
			fprintf(can_main_hw_log, "%02X ", src[i]);
		}

		p_status_ack = (status_ack_t *) src;

		if (p_status_ack->msg_type == CAN_SYS_S2M_STATUS_AKNOWLEDGE)
		{
			strncpy(module_type, (char *)(p_status_ack->module_type), 4);
			module_type[4] = '\0';
			int serial_number = (p_status_ack->serial.serial_byte_0 << 16) + (p_status_ack->serial.serial_byte_1 << 8) + (p_status_ack->serial.serial_byte_2);

			fprintf(can_main_hw_log, "\nblocking: ");
			fprintf(can_main_hw_log, "module_type: %s, serial_number: %d", module_type, serial_number);
		}

		fprintf(can_main_hw_log, "\n");
	}

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_main_hw_send_msg_obj_none_blocking(uint16_t msg_obj, uint16_t can_id, uint16_t dlc, const volatile byte_t *src)
{
	uint16_t i;

	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, "none_blocking: msg_obj = %d, can_id = 0x%X, dlc = %d, ", msg_obj, can_id, dlc);
	}

	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, " ");

		for (i = 0; i < dlc; i++)
		{
			fprintf(can_main_hw_log, "%02X ", src[i]);
		}
		fprintf(can_main_hw_log, "\n");
	}

	return (LI_CAN_SLV_ERR_OK);
}

void can_main_hw_set_log_file_name(char *file_name)
{
	(void) strcpy(can_main_hw_log_file_path, file_name);
}

int can_main_hw_log_open(void)
{
	can_main_hw_log = fopen(can_main_hw_log_file_path, "w+");

	if (NULL == can_main_hw_log)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void can_main_hw_log_close(void)
{
	(void) fclose(can_main_hw_log);
}

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/** @} */
