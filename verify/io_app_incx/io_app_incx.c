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
 * @defgroup io_app_incx.c
 * @file io_app_incx.c
 * @addtogroup app_incx
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "io_app_incx.h"
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
app_incx_can_tx_data_t app_incx_can_tx_data;

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/
static uint16_t app_incx_can_config_table_pos;
static uint32_t (*app_incx_can_sync_get_pr_time_valid_module)(void);

static const lcsa_module_config_t app_incx_can_module =
{
	APP_INCX_MODULE_TYPE,
	APP_INCX_MODULE_NR_DEF,
#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
	{TUPLE_ID_CNFG_DEFAULT, DLOAD_CRC_DEFAULT, DLOAD_DATE_DEFAULT, NULL},
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG
	LCSA_CONFIG_OBJ_USED_NOT,
	LCSA_CONFIG_OBJ_USED_1,
	{LCSA_CONFIG_DLC_USED_0, LCSA_CONFIG_DLC_USED_0, LCSA_CONFIG_DLC_USED_0, LCSA_CONFIG_DLC_USED_0},
	{LCSA_CONFIG_DLC_USED_2, LCSA_CONFIG_DLC_USED_0, LCSA_CONFIG_DLC_USED_0, LCSA_CONFIG_DLC_USED_0},
	{NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL},
	{&app_convc_tx_uint16, NULL, NULL, NULL},
#ifdef APP_MAIN_MON
	{&app_convc_mon_rx_uint16, NULL, NULL, NULL},
#endif // #ifdef APP_MAIN_MON
#ifdef CAN_MON
	{NULL, NULL, NULL, NULL},
	{&app_convc_mon_rx_uint16, NULL, NULL, NULL},
#endif // #ifdef CAN_MON
	{0, 0, 0, 0},
#ifdef APP_MAIN_MON
	{0, 0, 0, 0},
#endif // #ifdef APP_MAIN_MON
#ifdef CAN_MON
	{0, 0, 0, 0},
	{0, 0, 0, 0},
#endif // #ifdef CAN_MON
	LCSA_CONFIG_OBJ_USED_NOT,
	LCSA_CONFIG_OBJ_USED_1,
	{LCSA_CONFIG_DLC_USED_0, LCSA_CONFIG_DLC_USED_0, LCSA_CONFIG_DLC_USED_0, LCSA_CONFIG_DLC_USED_0},
	{LCSA_CONFIG_DLC_USED_2, LCSA_CONFIG_DLC_USED_0, LCSA_CONFIG_DLC_USED_0, LCSA_CONFIG_DLC_USED_0},
	{NULL, NULL, NULL, NULL},
	{&app_convc_tx_uint16, NULL, NULL, NULL},
#ifdef APP_MAIN_MON
	{&app_convc_mon_rx_uint16, NULL, NULL, NULL},
#endif // #ifdef APP_MAIN_MON
#ifdef CAN_MON
	{NULL, NULL, NULL, NULL},
	{&app_convc_mon_rx_uint16, NULL, NULL, NULL},
#endif // #ifdef CAN_MON
};

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
/**
 * @brief initialization of the INCX module
 * @param modnr used module number
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_incx_init(li_can_slv_module_nr_t modnr)
{
	lcsa_errorcode_t err = LCSA_ERROR_OK;

	/* call hw_init */
#ifdef APP_INCX_HW
	app_incx_hw_init();
#endif // #ifdef APP_INCX_HW
	app_port_memory_set(&app_incx_can_tx_data, 0, sizeof(app_incx_can_tx_data_t));

	/*----------------------------------------------------------------------*/
	/* add logical module "INCX" to CAN										*/
	/*----------------------------------------------------------------------*/
	err = lcsa_add_module(&app_incx_can_module, modnr,
	                      NULL, NULL, NULL, NULL,
	                      &app_incx_can_tx_data, NULL, NULL, NULL);

	if (err == LCSA_ERROR_OK)
	{
		err = lcsa_sync_set_process_time_valid_fnc(APP_INCX_MODULE_TYPE, &app_incx_can_sync_get_pr_time_valid_module);
	}

	if (err == LCSA_ERROR_OK)
	{
		err = lcsa_is_module_valid(APP_INCX_MODULE_TYPE, modnr, &app_incx_can_config_table_pos);
	}

	return (err);
}

/**
 * @param act_speed set the actual_speed
 */
void app_incx_set_incx(uint16_t incx)
{
	app_incx_can_tx_data = incx;
}

/**
 * @brief process output function for the INCX module
 * @return #lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_incx_process_output(void)
{
	return (LCSA_ERROR_OK);
}

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/** @} */

