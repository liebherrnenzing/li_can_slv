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
 * @defgroup app_ma_w unit of MA_W digital output module
 * @file io_app_ma_w.c
 * @addtogroup app_ma_w
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "io_app_ma_w.h"
#include "io_app_convc.h"

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/
app_ma_w_can_rx_data_t app_ma_w_can_rx_data[APP_MA_W_NUM_OF_MODULES];
app_ma_w_can_tx_data_t app_ma_w_can_tx_data[APP_MA_W_NUM_OF_MODULES];

volatile uint32_t app_ma_w_image_valid_cnt = 0;
volatile uint32_t app_ma_w_process_req_cnt = 0;
volatile uint32_t app_ma_w_process_time_valid = 0;
volatile uint32_t app_ma_w_image_not_valid_cnt = 0;

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/
static void app_ma_w_process_request_cbk(void);
static void app_ma_w_process_image_valid_cbk(void);
static void app_ma_w_process_image_not_valid_cbk(void);

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/
static uint16_t app_ma_w_can_config_table_pos;
static uint32_t (*app_ma_w_process_time_valid_fnc)(void);

static const lcsa_module_config_t app_ma_w_config =
{
	APP_MA_W_MODULE_TYPE,
	APP_MA_W_MODULE_NR_DEF,
#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
	{TUPLE_ID_CNFG_DEFAULT, DLOAD_CRC_DEFAULT, DLOAD_DATE_DEFAULT, NULL},
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG
	LCSA_CONFIG_OBJ_USED_4,
	LCSA_CONFIG_OBJ_USED_4,
	{LCSA_CONFIG_DLC_USED_8, LCSA_CONFIG_DLC_USED_8, LCSA_CONFIG_DLC_USED_8, LCSA_CONFIG_DLC_USED_8},
	{LCSA_CONFIG_DLC_USED_8, LCSA_CONFIG_DLC_USED_8, LCSA_CONFIG_DLC_USED_8, LCSA_CONFIG_DLC_USED_8},
	{NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL},
	{&app_convc_main_rx_uint16, &app_convc_main_rx_uint16, &app_convc_main_rx_uint16, &app_convc_main_rx_uint16},
	{&app_convc_tx_uint16, &app_convc_tx_uint16, &app_convc_tx_uint16, &app_convc_tx_uint16},
#ifdef LI_CAN_SLV_MAIN_MON
	{&app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
	{&app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16},
	{&app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16},
#endif // #ifdef LI_CAN_SLV_MON
	{0, 0, 0, 0},
#ifdef LI_CAN_SLV_MAIN_MON
	{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
	{0, 0, 0, 0},
	{0, 0, 0, 0},
#endif // #ifdef LI_CAN_SLV_MON
	LCSA_CONFIG_OBJ_USED_4,
	LCSA_CONFIG_OBJ_USED_4,
	{LCSA_CONFIG_DLC_USED_8, LCSA_CONFIG_DLC_USED_8, LCSA_CONFIG_DLC_USED_8, LCSA_CONFIG_DLC_USED_8},
	{LCSA_CONFIG_DLC_USED_8, LCSA_CONFIG_DLC_USED_8, LCSA_CONFIG_DLC_USED_8, LCSA_CONFIG_DLC_USED_8},
	{&app_convc_main_rx_uint16, &app_convc_main_rx_uint16, &app_convc_main_rx_uint16, &app_convc_main_rx_uint16},
	{&app_convc_tx_uint16, &app_convc_tx_uint16, &app_convc_tx_uint16, &app_convc_tx_uint16},
#ifdef LI_CAN_SLV_MAIN_MON
	{&app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16},
#endif // #ifdef LI_CAN_SLV_MAIN_MON
#ifdef LI_CAN_SLV_MON
	{&app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16},
	{&app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16, &app_convc_mon_rx_uint16},
#endif // #ifdef LI_CAN_SLV_MON
};

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
lcsa_errorcode_t app_ma_w_init(uint16_t module_idx, li_can_slv_module_nr_t modnr)
{
	lcsa_errorcode_t err = LCSA_ERROR_OK;

	/* call hw_init */
#ifdef APP_MA_W_HW
	app_ma_w_hw_init();
#endif // #ifdef APP_MA_W_HW

#ifdef APP_MA_W_SIM
	app_ma_w_can_tx_data[0].tx_obj1.word0 = 0x10;
	app_ma_w_can_tx_data[0].tx_obj1.word1 = 0x11;
	app_ma_w_can_tx_data[0].tx_obj1.word2 = 0x12;
	app_ma_w_can_tx_data[0].tx_obj1.word3 = 0x13;

	app_ma_w_can_tx_data[0].tx_obj2.word0 = 0x20;
	app_ma_w_can_tx_data[0].tx_obj2.word1 = 0x21;
	app_ma_w_can_tx_data[0].tx_obj2.word2 = 0x22;
	app_ma_w_can_tx_data[0].tx_obj2.word3 = 0x23;
	app_ma_w_can_tx_data[0].tx_obj3.word0 = 30;
	app_ma_w_can_tx_data[0].tx_obj3.word1 = 31;
	app_ma_w_can_tx_data[0].tx_obj3.word2 = 32;
	app_ma_w_can_tx_data[0].tx_obj3.word3 = 33;
	app_ma_w_can_tx_data[0].tx_obj4.word0 = 40;
	app_ma_w_can_tx_data[0].tx_obj4.word1 = 41;
	app_ma_w_can_tx_data[0].tx_obj4.word2 = 42;
	app_ma_w_can_tx_data[0].tx_obj4.word3 = 43;
#else
	app_port_memory_set(&app_ma_w_can_rx_data, 0, sizeof(app_ma_w_can_rx_data_t) * APP_MA_W_NUM_OF_MODULES);
	app_port_memory_set(&app_ma_w_can_tx_data, 0, sizeof(app_ma_w_can_tx_data_t) * APP_MA_W_NUM_OF_MODULES);
#endif // #ifdef APP_MA_W_SIM

	/*----------------------------------------------------------------------*/
	/* add logical module "MA_W" to CAN										*/
	/*----------------------------------------------------------------------*/
	if (module_idx < APP_MA_W_NUM_OF_MODULES)
	{
		if (err == LCSA_ERROR_OK)
		{
			err = lcsa_add_module(&app_ma_w_config, modnr, &app_ma_w_can_rx_data[module_idx].rx_obj1, &app_ma_w_can_rx_data[module_idx].rx_obj2, &app_ma_w_can_rx_data[module_idx].rx_obj3, &app_ma_w_can_rx_data[module_idx].rx_obj4,
			                      &app_ma_w_can_tx_data[module_idx].tx_obj1, &app_ma_w_can_tx_data[module_idx].tx_obj2, &app_ma_w_can_tx_data[module_idx].tx_obj3, &app_ma_w_can_tx_data[module_idx].tx_obj4);

		}
	}

	if (err == LCSA_ERROR_OK)
	{
		err = lcsa_sync_set_process_time_valid_fnc(APP_MA_W_MODULE_TYPE, &app_ma_w_process_time_valid_fnc);
	}

	if (err == LCSA_ERROR_OK)
	{
		err = lcsa_sync_set_process_request_cbk(APP_MA_W_MODULE_TYPE, modnr, &app_ma_w_process_request_cbk);
	}

	if (err == LCSA_ERROR_OK)
	{
		err = lcsa_sync_set_process_image_valid_cbk(APP_MA_W_MODULE_TYPE, modnr, &app_ma_w_process_image_valid_cbk);
	}

	if (err == LCSA_ERROR_OK)
	{
		err = lcsa_sync_set_process_image_not_valid_cbk(APP_MA_W_MODULE_TYPE, modnr, &app_ma_w_process_image_not_valid_cbk);
	}

	if (err == LCSA_ERROR_OK)
	{
		err = lcsa_is_module_valid(APP_MA_W_MODULE_TYPE, modnr, &app_ma_w_can_config_table_pos);
	}

	return (err);
}

void app_ma_w_tx1_set_word0(uint16_t module_idx, uint16_t value)
{
	app_ma_w_can_tx_data[module_idx].tx_obj1.word0 = value;
}

void app_ma_w_tx1_set_word1(uint16_t module_idx, uint16_t value)
{
	app_ma_w_can_tx_data[module_idx].tx_obj1.word1 = value;
}

void app_ma_w_tx1_set_word2(uint16_t module_idx, uint16_t value)
{
	app_ma_w_can_tx_data[module_idx].tx_obj1.word2 = value;
}

void app_ma_w_tx1_set_word3(uint16_t module_idx, uint16_t value)
{
	app_ma_w_can_tx_data[module_idx].tx_obj1.word3 = value;
}

void app_ma_w_tx2_set_word0(uint16_t module_idx, uint16_t value)
{
	app_ma_w_can_tx_data[module_idx].tx_obj2.word0 = value;
}

void app_ma_w_tx2_set_word1(uint16_t module_idx, uint16_t value)
{
	app_ma_w_can_tx_data[module_idx].tx_obj2.word1 = value;
}

void app_ma_w_tx2_set_word2(uint16_t module_idx, uint16_t value)
{
	app_ma_w_can_tx_data[module_idx].tx_obj2.word2 = value;
}

void app_ma_w_tx2_set_word3(uint16_t module_idx, uint16_t value)
{
	app_ma_w_can_tx_data[module_idx].tx_obj2.word3 = value;
}

void app_ma_w_tx3_set_word0(uint16_t module_idx, uint16_t value)
{
	app_ma_w_can_tx_data[module_idx].tx_obj3.word0 = value;
}

void app_ma_w_tx3_set_word1(uint16_t module_idx, uint16_t value)
{
	app_ma_w_can_tx_data[module_idx].tx_obj3.word1 = value;
}

void app_ma_w_tx3_set_word2(uint16_t module_idx, uint16_t value)
{
	app_ma_w_can_tx_data[module_idx].tx_obj3.word2 = value;
}

void app_ma_w_tx3_set_word3(uint16_t module_idx, uint16_t value)
{
	app_ma_w_can_tx_data[module_idx].tx_obj3.word3 = value;
}

void app_ma_w_tx4_set_word0(uint16_t module_idx, uint16_t value)
{
	app_ma_w_can_tx_data[module_idx].tx_obj4.word0 = value;
}

void app_ma_w_tx4_set_word1(uint16_t module_idx, uint16_t value)
{
	app_ma_w_can_tx_data[module_idx].tx_obj4.word1 = value;
}

void app_ma_w_tx4_set_word2(uint16_t module_idx, uint16_t value)
{
	app_ma_w_can_tx_data[module_idx].tx_obj4.word2 = value;
}

void app_ma_w_tx4_set_word3(uint16_t module_idx, uint16_t value)
{
	app_ma_w_can_tx_data[module_idx].tx_obj4.word3 = value;
}

uint16_t app_ma_w_rx1_get_word0(uint16_t module_idx)
{
	return app_ma_w_can_rx_data[module_idx].rx_obj1.word0;
}

uint16_t app_ma_w_rx1_get_word1(uint16_t module_idx)
{
	return app_ma_w_can_rx_data[module_idx].rx_obj1.word1;
}

uint16_t app_ma_w_rx1_get_word2(uint16_t module_idx)
{
	return app_ma_w_can_rx_data[module_idx].rx_obj1.word2;
}

uint16_t app_ma_w_rx1_get_word3(uint16_t module_idx)
{
	return app_ma_w_can_rx_data[module_idx].rx_obj1.word3;
}

uint16_t app_ma_w_rx2_get_word0(uint16_t module_idx)
{
	return app_ma_w_can_rx_data[module_idx].rx_obj2.word0;
}

uint16_t app_ma_w_rx2_get_word1(uint16_t module_idx)
{
	return app_ma_w_can_rx_data[module_idx].rx_obj2.word1;
}

uint16_t app_ma_w_rx2_get_word2(uint16_t module_idx)
{
	return app_ma_w_can_rx_data[module_idx].rx_obj2.word2;
}

uint16_t app_ma_w_rx2_get_word3(uint16_t module_idx)
{
	return app_ma_w_can_rx_data[module_idx].rx_obj2.word3;
}

uint16_t app_ma_w_rx3_get_word0(uint16_t module_idx)
{
	return app_ma_w_can_rx_data[module_idx].rx_obj3.word0;
}

uint16_t app_ma_w_rx3_get_word1(uint16_t module_idx)
{
	return app_ma_w_can_rx_data[module_idx].rx_obj3.word1;
}

uint16_t app_ma_w_rx3_get_word2(uint16_t module_idx)
{
	return app_ma_w_can_rx_data[module_idx].rx_obj3.word2;
}

uint16_t app_ma_w_rx3_get_word3(uint16_t module_idx)
{
	return app_ma_w_can_rx_data[module_idx].rx_obj3.word3;
}

uint16_t app_ma_w_rx4_get_word0(uint16_t module_idx)
{
	return app_ma_w_can_rx_data[module_idx].rx_obj4.word0;
}

uint16_t app_ma_w_rx4_get_word1(uint16_t module_idx)
{
	return app_ma_w_can_rx_data[module_idx].rx_obj4.word1;
}

uint16_t app_ma_w_rx4_get_word2(uint16_t module_idx)
{
	return app_ma_w_can_rx_data[module_idx].rx_obj4.word2;
}

uint16_t app_ma_w_rx4_get_word3(uint16_t module_idx)
{
	return app_ma_w_can_rx_data[module_idx].rx_obj4.word3;
}

lcsa_errorcode_t app_ma_w_process_input(void)
{
	return (LCSA_ERROR_OK);
}

lcsa_errorcode_t app_ma_w_process_output(void)
{
	return (LCSA_ERROR_OK);
}

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/
static void app_ma_w_process_request_cbk(void)
{
	app_ma_w_process_req_cnt++;
	app_ma_w_process_time_valid = app_port_get_system_tick() - app_ma_w_process_time_valid_fnc();
}

static void app_ma_w_process_image_valid_cbk(void)
{
	app_ma_w_image_valid_cnt++;
}
static void app_ma_w_process_image_not_valid_cbk(void)
{
	app_ma_w_image_not_valid_cnt++;
}


/** @} */
