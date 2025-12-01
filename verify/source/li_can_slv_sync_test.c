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
 * @file li_can_slv_core_test.c
 * @addtogroup unittest
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "unity_config.h"
#include "unity.h"
#include "xtfw.h"

#include <li_can_slv/api/li_can_slv_api.h>

// add some logical modules here as c include for test only
#include "io_app_frc2.c"
#include "io_app_incx.c"
#include "io_app_inxy.c"
#include "io_app_ma_w.c"

// used for the logging of the can output
#include "io_can_hw.h"
#include "io_can_main_hw.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <windows.h>

#include <string.h>

#include <li_can_slv/core/io_can_main.h>
#include <li_can_slv/core/io_can_main_handler.h>
#include <li_can_slv/core/io_can_mon.h>
#include <li_can_slv/sync/io_can_sync_handler.h>


#include "io_app_module_change.h"

#include "unittest_canpie-fd.h"

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
static uint16_t whole_image_valid_cnt = 0;
static uint16_t first_process_request_cnt = 0;
static uint8_t reinit = FALSE;

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/
static void whole_process_image_valid_cbk(void);
static void first_process_request_cbk(void);

static int doesFileExist(const char *filename);
static void get_expected_file_path(const char *filename, char *filepath);

extern void receive_main_tx_on_mon_rx(void);
extern uint32_t modify_pending_frame_main_tx_to_mon_rx(uint16_t can_id, uint8_t dlc, uint8_t* data, uint8_t clear_pending);
extern uint32_t send_to_main_rx_handler(uint16_t can_id, uint8_t dlc, uint8_t* data);
extern uint32_t send_to_mon_rx_handler(uint16_t can_id, uint8_t dlc, uint8_t* data);

static void receive_master_output_data(void);

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
// setUp will be called before each test
void setUp(void)
{
	static uint8_t init_once = 0;

	if (init_once != 1 || reinit == TRUE)
	{
		init_once = 1;
		reinit = FALSE;

		app_frc2_image_valid_cnt = 0;
		app_incx_image_valid_cnt = 0;
		app_inxy_image_valid_cnt = 0;

		app_ma_w_image_valid_cnt = 0;
		app_ma_w_image_not_valid_cnt = 0;
		app_ma_w_image_not_valid_err = LI_CAN_SLV_SYNC_ERR_FLAG_NO_ERR;

		/* clear module table */
		memset(can_config_module_tab, 0x00, sizeof(can_config_module_tab));
		/* clean sync structure */
		memset(&can_sync, 0x00, sizeof(can_sync));

		li_can_slv_sync_main_rx_msg_obj_used = 0;
		can_mon_rx_msg_obj_used = 0;
		can_mon_tx_msg_obj_used = 0;

		/* after initialization no call is set */
		XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, lcsa_init(LCSA_BAUD_RATE_DEFAULT));

		// table 0
		XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, app_frc2_init(APP_FRC2_MODULE_NR_DEF));

		// table 1
		XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, app_incx_init(APP_INCX_MODULE_NR_DEF));

		// table 2
		XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, app_inxy_init(APP_INXY_MODULE_NR_DEF));

		// table 3
		XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, app_ma_w_init(0, APP_MA_W_MODULE_NR_DEF));

		XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, lcsa_start());
	}
}

// tearDown will be called after each test
void tearDown(void)
{
	// for next test
	reinit = TRUE;
	li_can_slv_deinit();
	can_main_hw_log_close();
}

/**
 * @test test_process_req
 * @brief test the process request handling
 */
void test_process_req(void)
{
	char exp_log_file[] = "tc_process_req_exp.log";
	char act_log_file[] = "_tc_process_req.log";
	char exp_log_file_path[_MAX_PATH];

	uint16_t msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS;
	uint16_t msg_obj_mon = CAN_CONFIG_MSG_MON_OBJ_RX_PROCESS;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	get_expected_file_path(exp_log_file, exp_log_file_path);
	can_main_hw_set_log_file_name(act_log_file);
	if (can_main_hw_log_open() == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	XTFW_ASSERT_EQUAL_INT(0, first_process_request_cnt);

	lcsa_sync_set_first_process_request_cbk(&first_process_request_cbk);

	app_incx_set_incx(500);
	app_inxy_set_incx(50);
	app_inxy_set_incy(44);
	app_frc2_set_force(102);

	app_ma_w_tx1_set_word0(0, 283);
	app_ma_w_tx3_set_word0(0, 283);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_INT(1, first_process_request_cnt);

	// use a second process request to check that the callback did not happen.
	first_process_request_cnt = 0;

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_INT(0, first_process_request_cnt);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	/* compare file content */
	XTFW_ASSERT_EQUAL_INT(1, doesFileExist(act_log_file));
	TEST_ASSERT_BINARY_FILE(exp_log_file_path, act_log_file);
}

/**
 * @test test_sync_check_image_not_valid
 */
void test_sync_check_image_not_valid(void)
{
	char exp_log_file[] = "tc_sync_check_image_not_valid_exp.log";
	char act_log_file[] = "_tc_sync_check_image_not_valid.log";
	char exp_log_file_path[_MAX_PATH];

	uint16_t msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS;
	uint16_t msg_obj_mon = CAN_CONFIG_MSG_MON_OBJ_RX_PROCESS;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	get_expected_file_path(exp_log_file, exp_log_file_path);
	can_main_hw_set_log_file_name(act_log_file);
	if (can_main_hw_log_open() == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	lcsa_sync_set_whole_process_image_valid_cbk(&whole_process_image_valid_cbk);

	app_incx_set_incx(500);
	app_inxy_set_incx(50);
	app_inxy_set_incy(44);
	app_frc2_set_force(102);

	app_ma_w_tx1_set_word0(0, 283);
	app_ma_w_tx3_set_word0(0, 283);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	/* do not send rx data from master to modules */

	XTFW_ASSERT_EQUAL_INT(0, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_inxy_image_valid_cnt);

	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_not_valid_cnt);

	// now check for valid images with next call. ma_w has no rx data
	// so the check should failed for that module.
	// additionally the failed module will send an Error Message: ERR_MSG_CAN_MAIN_NR_OF_RX_DATA

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();

	XTFW_ASSERT_EQUAL_INT(1, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_inxy_image_valid_cnt);

	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_ma_w_image_not_valid_cnt);

	/* compare file content */
	XTFW_ASSERT_EQUAL_INT(1, doesFileExist(act_log_file));
	TEST_ASSERT_BINARY_FILE(exp_log_file_path, act_log_file);
}

/**
 * @test test_sync_check_image_valid
 */
void test_sync_check_image_valid(void)
{
	char exp_log_file[] = "tc_sync_check_image_valid_exp.log";
	char act_log_file[] = "_tc_sync_check_image_valid.log";
	char exp_log_file_path[_MAX_PATH];

	uint16_t msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS;
	uint16_t msg_obj_mon = CAN_CONFIG_MSG_MON_OBJ_RX_PROCESS;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	get_expected_file_path(exp_log_file, exp_log_file_path);
	can_main_hw_set_log_file_name(act_log_file);
	if (can_main_hw_log_open() == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	lcsa_sync_set_whole_process_image_valid_cbk(&whole_process_image_valid_cbk);

	app_incx_set_incx(500);
	app_inxy_set_incx(50);
	app_inxy_set_incy(44);
	app_frc2_set_force(102);

	app_ma_w_tx1_set_word0(0, 283);
	app_ma_w_tx3_set_word0(0, 283);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();

	// tx prozessdaten

	// now simulate some process input data for ma_w module number 113
	// can main def msg obj: 7, id:960, acc mask: 2047, dlc: 8, dir: 0
	// can main def msg obj: 8, id:961, acc mask: 2047, dlc: 8, dir: 0
	// can main def msg obj: 9, id:962, acc mask: 2047, dlc: 8, dir: 0
	// can main def msg obj: 11, id:963, acc mask: 2047, dlc: 8, dir: 0

	// MAIN
	// none_blocking: msg_obj = 7, can_id = 0x3C0, dlc = 8,  00 00 00 00 00 00 00 00
	// none_blocking: msg_obj = 8, can_id = 0x3C1, dlc = 8,  00 00 00 00 00 00 00 00
	// none_blocking: msg_obj = 9, can_id = 0x3C2, dlc = 8,  00 00 00 00 00 00 00 00
	// msg_obj = 10 is reserved for process image
	// none_blocking: msg_obj = 11, can_id = 0x3C3, dlc = 8,  00 00 00 00 00 00 00 00

	// MON
	// none_blocking: msg_obj = 9, can_id = 0x3C0, dlc = 8,  00 00 00 00 00 00 00 00
	// none_blocking: msg_obj = 10, can_id = 0x3C1, dlc = 8,  00 00 00 00 00 00 00 00
	// none_blocking: msg_obj = 11, can_id = 0x3C2, dlc = 8,  00 00 00 00 00 00 00 00
	// none_blocking: msg_obj = 12, can_id = 0x3C3, dlc = 8,  00 00 00 00 00 00 00 00

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(7, 8, 0x3c0, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(9, 8, 0x3c0, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(8, 8, 0x3c1, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(10, 8, 0x3c1, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(9, 8, 0x3c2, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(11, 8, 0x3c2, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(11, 8, 0x3c3, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(12, 8, 0x3c3, rx_data));

	XTFW_ASSERT_EQUAL_INT(0, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_inxy_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_not_valid_cnt);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();

	can_main_hw_log_close();

	XTFW_ASSERT_EQUAL_INT(1, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_inxy_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_not_valid_cnt);

	/* compare file content */
	XTFW_ASSERT_EQUAL_INT(1, doesFileExist(act_log_file));
	TEST_ASSERT_BINARY_FILE(exp_log_file_path, act_log_file);
}

/**
 * @test test_sync_image_ERR_MSG_CAN_MAIN_RX_WRONG_DLC
 * @brief test ERR_MSG_CAN_MAIN_RX_WRONG_DLC and callback with LI_CAN_SLV_SYNC_ERR_FLAG_MAIN_RX_DLC
 */
void test_sync_image_ERR_MSG_CAN_MAIN_RX_WRONG_DLC(void)
{
	char exp_log_file[] = "tc_sync_ERR_MSG_CAN_MAIN_RX_WRONG_DLC_exp.log";
	char act_log_file[] = "_tc_sync_ERR_MSG_CAN_MAIN_RX_WRONG_DLC.log";
	char exp_log_file_path[_MAX_PATH];

	uint16_t msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS;
	uint16_t msg_obj_mon = CAN_CONFIG_MSG_MON_OBJ_RX_PROCESS;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	get_expected_file_path(exp_log_file, exp_log_file_path);
	can_main_hw_set_log_file_name(act_log_file);
	if (can_main_hw_log_open() == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	lcsa_sync_set_whole_process_image_valid_cbk(&whole_process_image_valid_cbk);

	app_incx_set_incx(500);
	app_inxy_set_incx(50);
	app_inxy_set_incy(44);
	app_frc2_set_force(102);

	app_ma_w_tx1_set_word0(0, 283);
	app_ma_w_tx3_set_word0(0, 283);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(7, 8, 0x3c0, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(9, 8, 0x3c0, rx_data));

	/* MAIN wrong DLC */
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(8, 7, 0x3c1, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(10, 8, 0x3c1, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(9, 8, 0x3c2, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(11, 8, 0x3c2, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(11, 8, 0x3c3, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(12, 8, 0x3c3, rx_data));

	XTFW_ASSERT_EQUAL_INT(0, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_inxy_image_valid_cnt);

	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_not_valid_cnt);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_HEX32(LI_CAN_SLV_SYNC_ERR_FLAG_MAIN_RX_DLC | LI_CAN_SLV_SYNC_ERR_FLAG_MAIN_RX_MISSING_OBJ, app_ma_w_image_not_valid_err); /* check error code */

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	can_main_hw_log_close();

	XTFW_ASSERT_EQUAL_INT(3, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(3, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(3, app_inxy_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(2, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_ma_w_image_not_valid_cnt);

	/* compare file content */
	XTFW_ASSERT_EQUAL_INT(1, doesFileExist(act_log_file));
	TEST_ASSERT_BINARY_FILE(exp_log_file_path, act_log_file);
}

/**
 * @test test_sync_image_ERR_MSG_CAN_MON_RX_WRONG_DLC
 * @brief test ERR_MSG_CAN_MON_RX_WRONG_DLC and callback with LI_CAN_SLV_SYNC_ERR_FLAG_MON_RX_DLC
 */
void test_sync_image_ERR_MSG_CAN_MON_RX_WRONG_DLC(void)
{
	char exp_log_file[] = "tc_sync_ERR_MSG_CAN_MON_RX_WRONG_DLC_exp.log";
	char act_log_file[] = "_tc_sync_ERR_MSG_CAN_MON_RX_WRONG_DLC.log";
	char exp_log_file_path[_MAX_PATH];

	uint16_t msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS;
	uint16_t msg_obj_mon = CAN_CONFIG_MSG_MON_OBJ_RX_PROCESS;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	get_expected_file_path(exp_log_file, exp_log_file_path);
	can_main_hw_set_log_file_name(act_log_file);
	if (can_main_hw_log_open() == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	lcsa_sync_set_whole_process_image_valid_cbk(&whole_process_image_valid_cbk);

	app_incx_set_incx(500);
	app_inxy_set_incx(50);
	app_inxy_set_incy(44);
	app_frc2_set_force(102);

	app_ma_w_tx1_set_word0(0, 283);
	app_ma_w_tx3_set_word0(0, 283);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(7, 8, 0x3c0, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(9, 8, 0x3c0, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(8, 8, 0x3c1, rx_data));
	/* MON wrong DLC */
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(10, 7, 0x3c1, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(9, 8, 0x3c2, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(11, 8, 0x3c2, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(11, 8, 0x3c3, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(12, 8, 0x3c3, rx_data));

	XTFW_ASSERT_EQUAL_INT(0, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_inxy_image_valid_cnt);

	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_not_valid_cnt);

	// now check for valid images with next call. ma_w has no rx data
	// so the check should failed for that module.

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_HEX32(LI_CAN_SLV_SYNC_ERR_FLAG_MON_RX_DLC | LI_CAN_SLV_SYNC_ERR_FLAG_MON_RX_MISSING_OBJ, app_ma_w_image_not_valid_err); /* check error code */

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	can_main_hw_log_close();

	XTFW_ASSERT_EQUAL_INT(3, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(3, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(3, app_inxy_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(2, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_ma_w_image_not_valid_cnt);

	/* compare file content */
	XTFW_ASSERT_EQUAL_INT(1, doesFileExist(act_log_file));
	TEST_ASSERT_BINARY_FILE(exp_log_file_path, act_log_file);
}

/**
 * @test test_sync_image_ERR_MSG_CAN_MON_TX_WRONG_DLC
 * @brief test ERR_MSG_CAN_MON_TX_WRONG_DLC and callback with LI_CAN_SLV_SYNC_ERR_FLAG_MON_TX_DLC
 */
void test_sync_image_ERR_MSG_CAN_MON_TX_WRONG_DLC(void)
{
	char exp_log_file[] = "tc_sync_ERR_MSG_CAN_MON_TX_WRONG_DLC_exp.log";
	char act_log_file[] = "_tc_sync_ERR_MSG_CAN_MON_TX_WRONG_DLC.log";
	char exp_log_file_path[_MAX_PATH];

	uint16_t msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS;
	uint16_t msg_obj_mon = CAN_CONFIG_MSG_MON_OBJ_RX_PROCESS;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	byte_t tx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	get_expected_file_path(exp_log_file, exp_log_file_path);
	can_main_hw_set_log_file_name(act_log_file);
	if (can_main_hw_log_open() == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	lcsa_sync_set_whole_process_image_valid_cbk(&whole_process_image_valid_cbk);

	app_incx_set_incx(500);
	app_inxy_set_incx(50);
	app_inxy_set_incy(44);
	app_frc2_set_force(102);

	app_ma_w_tx1_set_word0(0, 283);
	app_ma_w_tx3_set_word0(0, 283);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	/* modify frame DLC main_tx to mon_rx */
	XTFW_ASSERT_EQUAL_UINT(0, modify_pending_frame_main_tx_to_mon_rx(0x5C1, 7, tx_data, 0));
	receive_main_tx_on_mon_rx();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(7, 8, 0x3c0, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(9, 8, 0x3c0, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(8, 8, 0x3c1, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(10, 8, 0x3c1, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(9, 8, 0x3c2, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(11, 8, 0x3c2, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(11, 8, 0x3c3, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(12, 8, 0x3c3, rx_data));

	XTFW_ASSERT_EQUAL_INT(0, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_inxy_image_valid_cnt);

	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_not_valid_cnt);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_HEX32(LI_CAN_SLV_SYNC_ERR_FLAG_MON_TX_DLC | LI_CAN_SLV_SYNC_ERR_FLAG_MON_TX_MISSING_OBJ, app_ma_w_image_not_valid_err); /* check error code */

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	can_main_hw_log_close();

	XTFW_ASSERT_EQUAL_INT(3, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(3, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(3, app_inxy_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(2, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_ma_w_image_not_valid_cnt);

	/* compare file content */
	XTFW_ASSERT_EQUAL_INT(1, doesFileExist(act_log_file));
	TEST_ASSERT_BINARY_FILE(exp_log_file_path, act_log_file);
}

/**
 * @test test_sync_image_ERR_MSG_CAN_MAIN_MON_DATA_RX
 * @brief test ERR_MSG_CAN_MAIN_MON_DATA_RX and callback with LI_CAN_SLV_SYNC_ERR_FLAG_MAIN_MON_RX_DATA
 */
void test_sync_image_ERR_MSG_CAN_MAIN_MON_DATA_RX(void)
{
	char exp_log_file[] = "tc_sync_ERR_MSG_CAN_MAIN_MON_DATA_RX_exp.log";
	char act_log_file[] = "_tc_sync_ERR_MSG_CAN_MAIN_MON_DATA_RX.log";
	char exp_log_file_path[_MAX_PATH];

	uint16_t msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS;
	uint16_t msg_obj_mon = CAN_CONFIG_MSG_MON_OBJ_RX_PROCESS;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	byte_t modified_rx_data[8] = { 0, 0, 0, 0, 1, 0, 0, 0 };

	get_expected_file_path(exp_log_file, exp_log_file_path);
	can_main_hw_set_log_file_name(act_log_file);
	if (can_main_hw_log_open() == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	lcsa_sync_set_whole_process_image_valid_cbk(&whole_process_image_valid_cbk);

	app_incx_set_incx(500);
	app_inxy_set_incx(50);
	app_inxy_set_incy(44);
	app_frc2_set_force(102);

	app_ma_w_tx1_set_word0(0, 283);
	app_ma_w_tx3_set_word0(0, 283);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(7, 8, 0x3c0, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(9, 8, 0x3c0, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(8, 8, 0x3c1, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(10, 8, 0x3c1, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(9, 8, 0x3c2, rx_data));
	/* Modify MON RX Data */
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(11, 8, 0x3c2, modified_rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(11, 8, 0x3c3, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(12, 8, 0x3c3, rx_data));

	XTFW_ASSERT_EQUAL_INT(0, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_inxy_image_valid_cnt);

	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_not_valid_cnt);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_HEX32(LI_CAN_SLV_SYNC_ERR_FLAG_MAIN_MON_RX_DATA, app_ma_w_image_not_valid_err); /* check error code */
	app_ma_w_image_not_valid_err = 0;

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	XTFW_ASSERT_EQUAL_HEX32(LI_CAN_SLV_SYNC_ERR_FLAG_NO_ERR, app_ma_w_image_not_valid_err);

	receive_main_tx_on_mon_rx();

	XTFW_ASSERT_EQUAL_INT(3, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(3, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(3, app_inxy_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(2, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_ma_w_image_not_valid_cnt);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(7, 8, 0x3c0, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(9, 8, 0x3c0, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(8, 8, 0x3c1, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(10, 8, 0x3c1, rx_data));

	/* Modify MAIN RX Data */
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(9, 8, 0x3c2, modified_rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(11, 8, 0x3c2, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(11, 8, 0x3c3, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(12, 8, 0x3c3, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_INT(4, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(4, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(4, app_inxy_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(2, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(2, app_ma_w_image_not_valid_cnt);

	XTFW_ASSERT_EQUAL_HEX32(LI_CAN_SLV_SYNC_ERR_FLAG_MAIN_MON_RX_DATA, app_ma_w_image_not_valid_err); /* check error code */

	can_main_hw_log_close();

	/* compare file content */
	XTFW_ASSERT_EQUAL_INT(1, doesFileExist(act_log_file));
	TEST_ASSERT_BINARY_FILE(exp_log_file_path, act_log_file);
}

/**
 * @test test_sync_image_ERR_MSG_CAN_MAIN_MON_DATA_TX
 * @brief test ERR_MSG_CAN_MAIN_MON_DATA_TX and callback with LI_CAN_SLV_SYNC_ERR_FLAG_MAIN_MON_TX_DATA
 */
void test_sync_image_ERR_MSG_CAN_MAIN_MON_DATA_TX(void)
{
	char exp_log_file[] = "tc_sync_ERR_MSG_CAN_MAIN_MON_DATA_TX_exp.log";
	char act_log_file[] = "_tc_sync_ERR_MSG_CAN_MAIN_MON_DATA_TX.log";
	char exp_log_file_path[_MAX_PATH];

	uint16_t msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS;
	uint16_t msg_obj_mon = CAN_CONFIG_MSG_MON_OBJ_RX_PROCESS;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	byte_t modified_rx_data[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };

	get_expected_file_path(exp_log_file, exp_log_file_path);
	can_main_hw_set_log_file_name(act_log_file);
	if (can_main_hw_log_open() == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	lcsa_sync_set_whole_process_image_valid_cbk(&whole_process_image_valid_cbk);

	app_incx_set_incx(500);
	app_inxy_set_incx(50);
	app_inxy_set_incy(44);
	app_frc2_set_force(102);

	app_ma_w_tx1_set_word0(0, 283);
	app_ma_w_tx3_set_word0(0, 283);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	/* modify frame DATA main_tx to mon_rx (will not be received by MON) */
	XTFW_ASSERT_EQUAL_UINT(0, modify_pending_frame_main_tx_to_mon_rx(0x5C1, 8, modified_rx_data, 0));
	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_INT(0, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_inxy_image_valid_cnt);

	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_not_valid_cnt);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_HEX32(LI_CAN_SLV_SYNC_ERR_FLAG_MAIN_MON_TX_DATA | LI_CAN_SLV_SYNC_ERR_FLAG_MON_TX_MISSING_OBJ, app_ma_w_image_not_valid_err); /* check error code */
	app_ma_w_image_not_valid_err = 0;

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	XTFW_ASSERT_EQUAL_HEX32(LI_CAN_SLV_SYNC_ERR_FLAG_NO_ERR, app_ma_w_image_not_valid_err);

	XTFW_ASSERT_EQUAL_INT(3, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(3, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(3, app_inxy_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(2, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_ma_w_image_not_valid_cnt);

	can_main_hw_log_close();

	/* compare file content */
	XTFW_ASSERT_EQUAL_INT(1, doesFileExist(act_log_file));
	TEST_ASSERT_BINARY_FILE(exp_log_file_path, act_log_file);
}

/**
 * @test test_sync_image_ERR_MSG_CAN_MAIN_NR_OF_RX_DATA
 * @brief test ERR_MSG_CAN_MAIN_NR_OF_RX_DATA and callback with LI_CAN_SLV_SYNC_ERR_FLAG_MAIN_RX_MISSING_OBJ
 */
void test_sync_image_ERR_MSG_CAN_MAIN_NR_OF_RX_DATA(void)
{
	char exp_log_file[] = "tc_sync_ERR_MSG_CAN_MAIN_NR_OF_RX_DATA_exp.log";
	char act_log_file[] = "_tc_sync_ERR_MSG_CAN_MAIN_NR_OF_RX_DATA.log";
	char exp_log_file_path[_MAX_PATH];

	uint16_t msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS;
	uint16_t msg_obj_mon = CAN_CONFIG_MSG_MON_OBJ_RX_PROCESS;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	get_expected_file_path(exp_log_file, exp_log_file_path);
	can_main_hw_set_log_file_name(act_log_file);
	if (can_main_hw_log_open() == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	lcsa_sync_set_whole_process_image_valid_cbk(&whole_process_image_valid_cbk);

	app_incx_set_incx(500);
	app_inxy_set_incx(50);
	app_inxy_set_incy(44);
	app_frc2_set_force(102);

	app_ma_w_tx1_set_word0(0, 283);
	app_ma_w_tx3_set_word0(0, 283);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(7, 8, 0x3c0, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(9, 8, 0x3c0, rx_data));

	/* No RX message for object 0x3c0 on MAIN */
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(10, 8, 0x3c1, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(9, 8, 0x3c2, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(11, 8, 0x3c2, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(11, 8, 0x3c3, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(12, 8, 0x3c3, rx_data));

	XTFW_ASSERT_EQUAL_INT(0, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_inxy_image_valid_cnt);

	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_not_valid_cnt);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_HEX32(LI_CAN_SLV_SYNC_ERR_FLAG_MAIN_RX_MISSING_OBJ, app_ma_w_image_not_valid_err); /* check error code */

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	can_main_hw_log_close();

	XTFW_ASSERT_EQUAL_INT(3, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(3, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(3, app_inxy_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(2, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_ma_w_image_not_valid_cnt);

	/* compare file content */
	XTFW_ASSERT_EQUAL_INT(1, doesFileExist(act_log_file));
	TEST_ASSERT_BINARY_FILE(exp_log_file_path, act_log_file);
}

/**
 * @test test_sync_image_ERR_MSG_CAN_MON_NR_OF_RX_DATA
 * @brief test ERR_MSG_CAN_MON_NR_OF_RX_DATA and callback with LI_CAN_SLV_SYNC_ERR_FLAG_MON_RX_MISSING_OBJ
 */
void test_sync_image_ERR_MSG_CAN_MON_NR_OF_RX_DATA(void)
{
	char exp_log_file[] = "tc_sync_ERR_MSG_CAN_MON_NR_OF_RX_DATA_exp.log";
	char act_log_file[] = "_tc_sync_ERR_MSG_CAN_MON_NR_OF_RX_DATA.log";
	char exp_log_file_path[_MAX_PATH];

	uint16_t msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS;
	uint16_t msg_obj_mon = CAN_CONFIG_MSG_MON_OBJ_RX_PROCESS;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	get_expected_file_path(exp_log_file, exp_log_file_path);
	can_main_hw_set_log_file_name(act_log_file);
	if (can_main_hw_log_open() == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	lcsa_sync_set_whole_process_image_valid_cbk(&whole_process_image_valid_cbk);

	app_incx_set_incx(500);
	app_inxy_set_incx(50);
	app_inxy_set_incy(44);
	app_frc2_set_force(102);

	app_ma_w_tx1_set_word0(0, 283);
	app_ma_w_tx3_set_word0(0, 283);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	XTFW_ASSERT_EQUAL_HEX32(LI_CAN_SLV_SYNC_ERR_FLAG_NO_ERR, app_ma_w_image_not_valid_err);

	receive_main_tx_on_mon_rx();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(7, 8, 0x3c0, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(9, 8, 0x3c0, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(8, 8, 0x3c1, rx_data));
	/* No RX message for object 0x3c1 on MON */

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(9, 8, 0x3c2, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(11, 8, 0x3c2, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(11, 8, 0x3c3, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(12, 8, 0x3c3, rx_data));

	XTFW_ASSERT_EQUAL_INT(0, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_inxy_image_valid_cnt);

	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_not_valid_cnt);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_HEX32(LI_CAN_SLV_SYNC_ERR_FLAG_MON_RX_MISSING_OBJ, app_ma_w_image_not_valid_err); /* check error code */

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	can_main_hw_log_close();

	XTFW_ASSERT_EQUAL_INT(3, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(3, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(3, app_inxy_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(2, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_ma_w_image_not_valid_cnt);

	/* compare file content */
	XTFW_ASSERT_EQUAL_INT(1, doesFileExist(act_log_file));
	TEST_ASSERT_BINARY_FILE(exp_log_file_path, act_log_file);
}

/**
 * @test test_sync_image_ERR_MSG_CAN_MON_NR_OF_TX_DATA
 * @brief test ERR_MSG_CAN_MON_NR_OF_TX_DATA and callback with LI_CAN_SLV_SYNC_ERR_FLAG_MON_TX_MISSING_OBJ
 */
void test_sync_image_ERR_MSG_CAN_MON_NR_OF_TX_DATA(void)
{
	char exp_log_file[] = "tc_sync_ERR_MSG_CAN_MON_NR_OF_TX_DATA_exp.log";
	char act_log_file[] = "_tc_sync_ERR_MSG_CAN_MON_NR_OF_TX_DATA.log";
	char exp_log_file_path[_MAX_PATH];

	uint16_t msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS;
	uint16_t msg_obj_mon = CAN_CONFIG_MSG_MON_OBJ_RX_PROCESS;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	get_expected_file_path(exp_log_file, exp_log_file_path);
	can_main_hw_set_log_file_name(act_log_file);
	if (can_main_hw_log_open() == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	lcsa_sync_set_whole_process_image_valid_cbk(&whole_process_image_valid_cbk);

	app_incx_set_incx(500);
	app_inxy_set_incx(50);
	app_inxy_set_incy(44);
	app_frc2_set_force(102);

	app_ma_w_tx1_set_word0(0, 283);
	app_ma_w_tx3_set_word0(0, 283);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	/* modify PENDING_FLAG for main_tx to mon_rx (will not be received by MON) */
	XTFW_ASSERT_EQUAL_UINT(0, modify_pending_frame_main_tx_to_mon_rx(0x5C1, 0, NULL, 1));
	receive_main_tx_on_mon_rx();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(7, 8, 0x3c0, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(9, 8, 0x3c0, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(8, 8, 0x3c1, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(10, 8, 0x3c1, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(9, 8, 0x3c2, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(11, 8, 0x3c2, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(11, 8, 0x3c3, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(12, 8, 0x3c3, rx_data));

	XTFW_ASSERT_EQUAL_INT(0, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_inxy_image_valid_cnt);

	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_not_valid_cnt);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_HEX32(LI_CAN_SLV_SYNC_ERR_FLAG_MON_TX_MISSING_OBJ, app_ma_w_image_not_valid_err); /* check error code */

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	can_main_hw_log_close();

	XTFW_ASSERT_EQUAL_INT(3, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(3, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(3, app_inxy_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(2, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_ma_w_image_not_valid_cnt);

	/* compare file content */
	XTFW_ASSERT_EQUAL_INT(1, doesFileExist(act_log_file));
	TEST_ASSERT_BINARY_FILE(exp_log_file_path, act_log_file);
}

/**
 * @test test_sync_sync_critical_module_no_error
 * @brief test if a critical module does not send any error messages
 */
void test_sync_sync_critical_module_no_error(void)
{
	char exp_log_file[] = "tc_sync_critical_module_no_error_exp.log";
	char act_log_file[] = "_tc_sync_critical_module_no_error_exp.log";
	char exp_log_file_path[_MAX_PATH];

	uint16_t msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS;
	uint16_t msg_obj_mon = CAN_CONFIG_MSG_MON_OBJ_RX_PROCESS;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	get_expected_file_path(exp_log_file, exp_log_file_path);
	can_main_hw_set_log_file_name(act_log_file);
	if (can_main_hw_log_open() == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	lcsa_sync_set_whole_process_image_valid_cbk(&whole_process_image_valid_cbk);

	app_incx_set_incx(500);
	app_inxy_set_incx(50);
	app_inxy_set_incy(44);
	app_frc2_set_force(102);

	app_ma_w_tx1_set_word0(0, 283);
	app_ma_w_tx3_set_word0(0, 283);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(7, 8, 0x3c0, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(9, 8, 0x3c0, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(8, 8, 0x3c1, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(10, 8, 0x3c1, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(9, 8, 0x3c2, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(11, 8, 0x3c2, rx_data));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(11, 8, 0x3c3, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(12, 8, 0x3c3, rx_data));

	XTFW_ASSERT_EQUAL_INT(0, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_inxy_image_valid_cnt);

	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_not_valid_cnt);

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	/* set module MA_W critical */
	XTFW_ASSERT_EQUAL_UINT(LI_CAN_SLV_ERR_OK, can_config_set_critical(APP_MA_W_MODULE_TYPE));

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	receive_main_tx_on_mon_rx();
	receive_master_output_data();

	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx(msg_obj, 0, 0x001, rx_data));
	XTFW_ASSERT_EQUAL_UINT(0, can_sync_handler_rx_mon(msg_obj_mon, 0, 0x001, rx_data));

	can_main_hw_log_close();

	XTFW_ASSERT_EQUAL_INT(5, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(5, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(5, app_inxy_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(5, app_ma_w_image_valid_cnt);

	/* compare file content */
	XTFW_ASSERT_EQUAL_INT(1, doesFileExist(act_log_file));
	TEST_ASSERT_BINARY_FILE(exp_log_file_path, act_log_file);
}

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/

static void whole_process_image_valid_cbk(void)
{
	whole_image_valid_cnt++;
}

static void first_process_request_cbk(void)
{
	first_process_request_cnt++;
}

static void receive_master_output_data(void)
{
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	uint16_t can_id = 0;
	uint8_t dlc = 8;

	for (uint32_t i = 0; i < 4; i++)
	{
		can_id = 0x3c0 + i;
		send_to_main_rx_handler(can_id, dlc, rx_data);
		send_to_mon_rx_handler(can_id, dlc, rx_data);
	}
}

static int doesFileExist(const char *filename)
{
	struct stat st;
	int result = stat(filename, &st);
	return result == 0;
}

static void get_expected_file_path(const char *filename, char *filepath)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];

	_splitpath(Unity.TestFile, drive, dir, NULL, NULL);
	strcpy(fname, drive);
	strcat(fname, dir);
	strcat(fname, "..\\expected\\file\\");
	strcat(fname, filename);
	strcpy(filepath, fname);
}

/** @} */
