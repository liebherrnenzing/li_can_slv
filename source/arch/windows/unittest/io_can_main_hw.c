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

#include "io_can_main.h"
#include "io_can_sys.h"
#include "io_can_sync.h"
#include "io_can_async.h"

#include "io_can_error.h"
#include "io_can_errno.h"

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

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t can_main_hw_init(void)
{
	(void) strncpy(can_main_hw_log_file_path, default_name, sizeof(default_name));

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

	return (LI_CAN_SLV_ERR_OK);
}

li_can_slv_errorcode_t can_main_hw_define_msg_obj(uint16_t msg_obj, uint16_t can_id, uint16_t acceptance_mask, byte_t dlc, byte_t dir, can_main_service_id_t service_id)
{
	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, "\ncan_main_hw_define_msg_obj: msg_obj = %d, can_id = %d, ", msg_obj, can_id);
		fprintf(can_main_hw_log, "acceptance_mask = %d, dlc = %d, dir = %d, can_main_service_id = %d", acceptance_mask, dlc, dir, service_id);
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

li_can_slv_errorcode_t can_main_hw_send_msg_obj_none_blocking_with_sync_tx_conv_code(uint16_t msg_obj, uint16_t table_pos, uint16_t obj)
{
	li_can_slv_errorcode_t err;
	uint16_t i;
	uint8_t dlc;
	uint8_t data[8] =
	{ 0, 0, 0, 0, 0, 0, 0, 0 };

	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, "can_main_hw_send_msg_obj_none_blocking_with_sync_tx_conv_code: ");
	}

	dlc = can_config_module_tab[table_pos].tx_dlc[obj];
	/* call synchrony transmit routine of main CAN controller to prepare data to send via can, also capture data for monitor controller */
	err = can_sync_tx_data_main(table_pos, obj, (uint16_t) dlc, data);

	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, "type = %s, ", can_config_module_tab[table_pos].type);
		fprintf(can_main_hw_log, "modnr = %d, ", can_config_module_tab[table_pos].module_nr);
		fprintf(can_main_hw_log, "msg_obj = %d, table_pos = %d, ", msg_obj, table_pos);
		fprintf(can_main_hw_log, "obj = %d, ", obj);
		fprintf(can_main_hw_log, "id = 0x%02X, ", can_main_sync_process_tx_data_ctrl.id[table_pos][obj]);
		fprintf(can_main_hw_log, "dlc = %d, ", can_main_sync_process_tx_data_ctrl.dlc[table_pos][obj]);

		fprintf(can_main_hw_log, " ");

		for (i = 0; i < 8; i++)
		{
			fprintf(can_main_hw_log, "%02X ", data[i]);
		}
		fprintf(can_main_hw_log, "\n");
	}

	return (err);
}

li_can_slv_errorcode_t can_main_hw_get_next_free_msg_obj(uint16_t *msg_obj)
{
	static uint16_t obj = 0;

	if (can_main_hw_log != NULL)
	{
		fprintf(can_main_hw_log, "\ncan_main_hw_get_next_free_msg_obj");
		fprintf(can_main_hw_log, "msg_obj = %d", *msg_obj);
	}
	*msg_obj = obj++;

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
