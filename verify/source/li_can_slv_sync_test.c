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
#include <li_can_slv/sync/io_can_sync_handler.h>

#include "io_app_module_change.h"

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
static lcsa_errorcode_t err = LCSA_ERROR_OK;

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/
static void whole_process_image_valid_cbk(void);
static void first_process_request_cbk(void);

static int doesFileExist(const char *filename);
static void get_expected_file_path(const char *filename, char *filepath);

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
// setUp will be called before each test
void setUp(void)
{
	static uint8_t init_once = 0;

	lcsa_module_number_t module_nr;

	if (init_once != 1 || reinit == TRUE)
	{
		init_once = 1;
		reinit = FALSE;

		app_frc2_image_valid_cnt = 0;
		app_incx_image_valid_cnt = 0;
		app_inxy_image_valid_cnt = 0;

		app_ma_w_image_valid_cnt = 0;
		app_ma_w_image_not_valid_cnt = 0;

		/* clear module table */
		memset(can_config_module_tab, 0x00, sizeof(can_config_module_tab));
		/* clean sync structure */
		memset(&can_sync, 0x00, sizeof(can_sync));

		li_can_slv_sync_main_rx_msg_obj_used = 0;

		/* after initialization no call is set */
		err = lcsa_init(LCSA_BAUD_RATE_DEFAULT);
		XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

		// table 0
		module_nr = APP_FRC2_MODULE_NR_DEF;
		err = app_frc2_init(module_nr);
		XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

		// table 1
		module_nr = APP_INCX_MODULE_NR_DEF;
		err = app_incx_init(module_nr);
		XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

		// table 2
		module_nr = APP_INXY_MODULE_NR_DEF;
		err = app_inxy_init(module_nr);
		XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

		// table 3
		module_nr = APP_MA_W_MODULE_NR_DEF;
		err = app_ma_w_init(0,  module_nr);
		XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

		err = lcsa_start();
	}
}

// tearDown will be called after each test
void tearDown(void)
{
	// for next test
	reinit = TRUE;
}

/**
 * @test test_process_req
 * @brief test the process request handling
 */
void test_process_req(void)
{
	char file_path[_MAX_PATH];
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int ret;

	can_main_hw_set_log_file_name("tc_process_req.log");
	ret = can_main_hw_log_open();
	if (ret == EXIT_FAILURE)
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

	uint16_t msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS;
	uint8_t dlc = 0;
	uint16_t canid = 0x001;

	ret = can_sync_handler_rx(msg_obj, dlc, canid, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

	XTFW_ASSERT_EQUAL_INT(1, first_process_request_cnt);

	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);

	can_main_hw_log_close();

	ret = doesFileExist("tc_process_req.log");
	XTFW_ASSERT_EQUAL_INT(1, ret);

	/* compare file content */
	get_expected_file_path("tc_process_req_expected.log", file_path);
	TEST_ASSERT_BINARY_FILE(file_path, "tc_process_req.log");

	// use a second process request to check that the callback did not happen.
	first_process_request_cnt = 0;

	ret = can_sync_handler_rx(msg_obj, dlc, canid, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

	XTFW_ASSERT_EQUAL_INT(0, first_process_request_cnt);

}

/**
 * @test test_sync_check_image_not_valid
 */
void test_sync_check_image_not_valid(void)
{
	char file_path[_MAX_PATH];
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int ret;

	can_main_hw_set_log_file_name("test_sync_check_image_not_valid.log");
	ret = can_main_hw_log_open();
	if (ret == EXIT_FAILURE)
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

	uint16_t msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS;
	uint8_t dlc = 0;
	uint16_t canid = 0x001;

	ret = can_sync_handler_rx(msg_obj, dlc, canid, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);


	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);

	XTFW_ASSERT_EQUAL_INT(0, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_inxy_image_valid_cnt);

	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_not_valid_cnt);


	// now check for valid images with next call. ma_w has no rx data
	// so the check should failed for that module.

	ret = can_sync_handler_rx(msg_obj, dlc, canid, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);

	can_main_hw_log_close();

	XTFW_ASSERT_EQUAL_INT(1, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_inxy_image_valid_cnt);

	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_ma_w_image_not_valid_cnt);

	ret = doesFileExist("test_sync_check_image_not_valid.log");
	XTFW_ASSERT_EQUAL_INT(1, ret);

	/* compare file content */
	get_expected_file_path("test_sync_check_image_not_valid_expected.log", file_path);
	TEST_ASSERT_BINARY_FILE(file_path, "test_sync_check_image_not_valid.log");
}

/**
 * @test test_sync_check_image_valid
 */
void test_sync_check_image_valid(void)
{
	char file_path[_MAX_PATH];
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int ret;


	can_main_hw_set_log_file_name("test_sync_check_image_valid.log");
	ret = can_main_hw_log_open();
	if (ret == EXIT_FAILURE)
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

	uint16_t msg_obj = CAN_CONFIG_MSG_MAIN_OBJ_RX_PROCESS;
	uint8_t dlc = 0;
	uint16_t canid = 0x001;

	// process request vom master zum module
	ret = can_sync_handler_rx(msg_obj, dlc, canid, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

	// rx processdaten module zum master
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);

	// tx prozessdaten

	// now simulate some process input data for ma_w module number 113
	// can main def msg obj: 5, id:960, acc mask: 2047, dlc: 8, dir: 0
	// can main def msg obj: 8, id:961, acc mask: 2047, dlc: 8, dir: 0
	// can main def msg obj: 10, id:962, acc mask: 2047, dlc: 8, dir: 0
	// can main def msg obj: 11, id:963, acc mask: 2047, dlc: 8, dir: 0

	// none_blocking: msg_obj = 4, can_id = 0x3C0, dlc = 8,  00 00 00 00 00 00 00 00
	// none_blocking: msg_obj = 1, can_id = 0x3C1, dlc = 8,  00 00 00 00 00 00 00 00
	// none_blocking: msg_obj = 4, can_id = 0x3C2, dlc = 8,  00 00 00 00 00 00 00 00
	// none_blocking: msg_obj = 1, can_id = 0x3C3, dlc = 8,  00 00 00 00 00 00 00 00

	ret = can_sync_handler_rx(11, 8, 0x3c0, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

	ret = can_sync_handler_rx(12, 8, 0x3c1, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

	ret = can_sync_handler_rx(13, 8, 0x3c2, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

	ret = can_sync_handler_rx(14, 8, 0x3c3, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

	XTFW_ASSERT_EQUAL_INT(0, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_inxy_image_valid_cnt);

	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_not_valid_cnt);

	// now check for valid images with next call. ma_w has no rx data
	// so the check should failed for that module.

	ret = can_sync_handler_rx(msg_obj, dlc, canid, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);
	can_main_handler_tx(1);

	can_main_hw_log_close();

	XTFW_ASSERT_EQUAL_INT(1, app_frc2_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_incx_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_inxy_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(1, app_ma_w_image_valid_cnt);
	XTFW_ASSERT_EQUAL_INT(0, app_ma_w_image_not_valid_cnt);

	ret = doesFileExist("test_sync_check_image_valid.log");
	XTFW_ASSERT_EQUAL_INT(1, ret);

	/* compare file content */
	get_expected_file_path("test_sync_check_image_valid_expected.log", file_path);
	TEST_ASSERT_BINARY_FILE(file_path, "test_sync_check_image_valid.log");
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
