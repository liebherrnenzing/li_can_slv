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
#include <li_can_slv/core/io_can_mon.h>
#include <li_can_slv/core/io_can_main_handler.h>
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
static uint16_t first_status_request_cnt = 0;
static uint8_t reinit = FALSE;
static lcsa_errorcode_t err = LCSA_ERROR_OK;
static uint16_t reset_reason_actual = 0;
static li_can_slv_system_time_t received_system_time;

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/
static void first_status_request_cbk(void);
static void system_time_cbk(li_can_slv_system_time_t system_time);
static void factory_reset_cbk(uint8_t reset_reason);

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

		/* clear module table */
		memset(can_config_module_tab, 0x00, sizeof(can_config_module_tab));

		li_can_slv_sync_main_rx_msg_obj_used = 0;
		can_mon_rx_msg_obj_used = 0;
		can_mon_tx_msg_obj_used = 0;

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
	//li_can_slv_deinit();
}

/**
 * @test test_sys_msg_first_status_req
 * @brief test the first status request handling
 */
void test_sys_msg_first_status_req(void)
{
	char file_path[_MAX_PATH];
	lcsa_errorcode_t err = LCSA_ERROR_OK;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	lcsa_module_number_t module_nr = 1;
	uint16_t dlc = 8;
	int ret;

	/* set first status request callback */
	err = lcsa_set_first_status_request_cbk(&first_status_request_cbk);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

	first_status_request_cnt = 0;
	XTFW_ASSERT_EQUAL_UINT(0, first_status_request_cnt);

	can_main_hw_set_log_file_name("_tc_first_status_req.log");
	ret = can_main_hw_log_open();
	if (ret == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	/* simulate a status request message */
	rx_data[0] = CAN_SYS_M2S_STATUS_REQUEST;

	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	can_main_hw_log_close();

	ret = doesFileExist("_tc_first_status_req.log");
	XTFW_ASSERT_EQUAL_INT(1, ret);

	/* check if the callback was called */
	XTFW_ASSERT_EQUAL_UINT16(1, first_status_request_cnt);

	/* compare file content */
	get_expected_file_path("tc_first_status_req_exp.log", file_path);
	TEST_ASSERT_BINARY_FILE(file_path, "_tc_first_status_req.log");
}

/**
 * @test test_sys_msg_status_req
 * @brief test the status request handling
 */
void test_sys_msg_status_req(void)
{
	char file_path[_MAX_PATH];
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	lcsa_module_number_t module_nr = 1;
	uint16_t dlc = 8;

	can_main_hw_set_log_file_name("_tc_status_req.log");
	int ret = can_main_hw_log_open();
	if (ret == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	/* simulate a status request message */
	rx_data[0] = CAN_SYS_M2S_STATUS_REQUEST;

	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	can_main_hw_log_close();

	ret = doesFileExist("_tc_status_req.log");
	XTFW_ASSERT_EQUAL_INT(1, ret);

	/* compare file content */
	get_expected_file_path("tc_status_req_exp.log", file_path);
	TEST_ASSERT_BINARY_FILE(file_path, "_tc_status_req.log");
}

/**
 * @test test_sys_msg_status_req_broadcast
 * @brief test the status request handling
 */
void test_sys_msg_status_req_broadcast(void)
{
	char file_path[_MAX_PATH];
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	lcsa_module_number_t module_nr;
	uint16_t dlc = 8;

	can_main_hw_set_log_file_name("_tc_status_req_broadcast_ma_w.log");
	int ret = can_main_hw_log_open();
	if (ret == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	// get module number of the ma_w module
	can_config_get_module_nr_by_type(&module_nr, APP_MA_W_MODULE_TYPE);

	/* simulate a status request message */
	rx_data[0] = CAN_SYS_M2S_STATUS_REQUEST;

	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	can_main_hw_log_close();

	ret = doesFileExist("_tc_status_req_broadcast_ma_w.log");
	XTFW_ASSERT_EQUAL_INT(1, ret);

	/* compare file content */
	get_expected_file_path("tc_status_req_broadcast_ma_w_exp.log", file_path);
	TEST_ASSERT_BINARY_FILE(file_path, "_tc_status_req_broadcast_ma_w.log");
}

/**
 * @test test_sys_msg_version_req
 * @brief test the version request handling
 */
void test_sys_msg_version_req(void)
{
	char file_path[_MAX_PATH];
	lcsa_errorcode_t err = LCSA_ERROR_OK;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	lcsa_module_number_t module_nr = 1;
	uint16_t dlc = 8;
	int ret;

	can_main_hw_set_log_file_name("_tc_version_req.log");
	ret = can_main_hw_log_open();
	if (ret == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	/* simulate a version request message */
	rx_data[0] = CAN_SYS_M2S_VERSION_REQUEST;

	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	can_main_hw_log_close();

	ret = doesFileExist("_tc_version_req.log");
	XTFW_ASSERT_EQUAL_INT(1, ret);

	/* compare file content */
	get_expected_file_path("tc_version_req_exp.log", file_path);
	TEST_ASSERT_BINARY_FILE(file_path, "_tc_version_req.log");
}

/**
 * @test test_sys_msg_version_req_broadcast
 * @brief test the version request handling
 */
void test_sys_msg_version_req_broadcast(void)
{
	char file_path[_MAX_PATH];
	lcsa_errorcode_t err = LCSA_ERROR_OK;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	lcsa_module_number_t module_nr;
	uint16_t dlc = 8;
	int ret;

	can_main_hw_set_log_file_name("_tc_version_req_broadcast_frc2.log");
	ret = can_main_hw_log_open();
	if (ret == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	// get module number of the ma_w module
	can_config_get_module_nr_by_type(&module_nr, APP_FRC2_MODULE_TYPE);

	/* simulate a version request message */
	rx_data[0] = CAN_SYS_M2S_VERSION_REQUEST;

	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	can_main_hw_log_close();

	ret = doesFileExist("_tc_version_req_broadcast_frc2.log");
	XTFW_ASSERT_EQUAL_INT(1, ret);

	/* compare file content */
	get_expected_file_path("tc_version_req_broadcast_frc2_exp.log", file_path);
	TEST_ASSERT_BINARY_FILE(file_path, "_tc_version_req_broadcast_frc2.log");
}

/**
 * @test test_sys_msg_invalid_dlc
 * @brief This test checks the invalid dlc handling
 */
void test_sys_msg_invalid_dlc(void)
{
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	lcsa_module_number_t module_nr = 1;
	uint16_t dlc = 9;

	/* simulate a status request message */
	rx_data[0] = CAN_SYS_M2S_STATUS_REQUEST;

	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(ERR_MSG_CAN_INVALID_SYS_MSG_DLC, err);
}

/**
 * @test test_sys_msg_broadcast_no_module
 * @brief This test checks the handling if no module is available
 */
void test_sys_msg_broadcast_no_module(void)
{
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	lcsa_module_number_t module_nr = 1;
	uint16_t dlc = 8;

	/* clear module table */
	can_config_nr_of_modules = 0;
	// del for the test
	memset(can_config_module_tab, 0x00, sizeof(can_config_module_tab));
	// reinit for next test
	reinit = TRUE;

	/* simulate a status request message */
	rx_data[0] = CAN_SYS_M2S_STATUS_REQUEST;

	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(ERR_MSG_CAN_CONFIG_MODULE_NOT_FOUND, err);
}

/**
 * @test test_sys_msg_invalid_module_number
 * @brief This test checks the invalid dlc handling
 */
void test_sys_msg_invalid_module_number(void)
{
	char file_path[_MAX_PATH];
	lcsa_module_number_t module_nr = 200;
	uint16_t dlc = 8;

	can_main_hw_set_log_file_name("_tc_status_req_invalid_module_number.log");
	int ret = can_main_hw_log_open();
	if (ret == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	byte_t rx_data[8] = { 0, 1, 2, 3, 4, 5, 255, 254 };

	/* simulate a status request message */
	rx_data[0] = CAN_SYS_M2S_STATUS_REQUEST;

	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

	// send a dummy msg
	can_main_hw_send_msg(0x002,  8, &rx_data[0]);

	can_main_hw_log_close();

	ret = doesFileExist("_tc_status_req_invalid_module_number.log");
	XTFW_ASSERT_EQUAL_INT(1, ret);

	/* compare file content */
	get_expected_file_path("tc_status_req_invalid_module_number_exp.log", file_path);
	TEST_ASSERT_BINARY_FILE(file_path, "_tc_status_req_invalid_module_number.log");
}

/**
 * @test test_sys_msg_broadcast_not_allowed
 * @brief This test checks the handling of not allowed broadcast messages.
 */
void test_sys_msg_broadcast_not_allowed(void)
{
	lcsa_errorcode_t err = LCSA_ERROR_OK;
	/* use a broadcast message */
	byte_t rx_data[8] = { 0, 0, 114, 0, 0, 0x1e, 0x74, 0xde };
	lcsa_module_number_t module_nr;
	uint16_t dlc = 8;

	module_nr = 1; /* broadcast */

	/* the following messages are not allowed as broadcast */
	rx_data[0] = CAN_SYS_M2S_CHANGE_MODULE_NUMBER;
	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(ERR_MSG_CAN_SYSTEM_MSG_NOT_ALLOWED_BROADCAST, err);

	rx_data[0] = CAN_SYS_M2S_STAY_SILENT;
	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(ERR_MSG_CAN_SYSTEM_MSG_NOT_ALLOWED_BROADCAST, err);

	rx_data[0] = CAN_SYS_M2S_AWAKE;
	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(ERR_MSG_CAN_SYSTEM_MSG_NOT_ALLOWED_BROADCAST, err);

#if defined(LI_CAN_SLV_SYS_CHANGE_MODULE_TYPE)
	rx_data[0] = CAN_SYS_M2S_CHANGE_MODULE_TYPE;
	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(ERR_MSG_CAN_SYSTEM_MSG_NOT_ALLOWED_BROADCAST, err);
#endif // #if defined(LI_CAN_SLV_SYS_CHANGE_MODULE_TYPE)

}

/**
 * @test test_sys_msg_broadcast_allowed
 * @brief This test checks the handling of allowed broadcast messages.
 */
void test_sys_msg_broadcast_allowed(void)
{
	lcsa_errorcode_t err = LCSA_ERROR_OK;
	byte_t rx_data[8] = { 0x02, 0x03, 0xE8, 0, 0, 0, 0};
	lcsa_module_number_t module_nr;
	uint16_t dlc = 8;
	lcsa_bdr_t bdr = 0;

	module_nr = 1; /* broadcast */

	/* read actual baud rate */
	bdr = lcsa_get_baudrate();
	XTFW_ASSERT_EQUAL_UINT(LCSA_BAUD_RATE_DEFAULT, bdr);

	/* simulate a change baud rate via broadcast number */
	rx_data[0] = CAN_SYS_M2S_CHANGE_CAN_BAUDRATE;
	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LI_CAN_SLV_ERR_OK, err);

	/* now we should have 1MBaud */
	bdr = lcsa_get_baudrate();
	XTFW_ASSERT_EQUAL_UINT(LCSA_BAUD_RATE_1M, bdr);

	rx_data[0] = CAN_SYS_M2S_STATUS_REQUEST;
	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LI_CAN_SLV_ERR_OK, err);

}

/**
 * @test test_sys_msg_stay_silent_sys
 *   - Step 1 Status and Version Request -> all modules must answer
 *   - Step 2 Set MA_W module silent
 *   - Step 3 Status and Version Request -> all modules except MA_W must answer
 *   - Step 4 Awake MA_W module
 *   - Step 5 Status and Version Request -> all modules must answer
 * @brief This tests if the stay silent command also works with system commands.
 */
void test_sys_msg_stay_silent_sys(void)
{
	char file_path[_MAX_PATH];
	lcsa_errorcode_t err = LCSA_ERROR_OK;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	lcsa_module_number_t module_nr = 1;
	uint16_t dlc = 8;
	int ret;

	/* Test Status Request CAN_SYS_M2S_STATUS_REQUEST */
	/* All Modules must answer */
	rx_data[0] = CAN_SYS_M2S_STATUS_REQUEST;

	can_main_hw_set_log_file_name("_tc_stay_s_sys_stat_norm.log");
	ret = can_main_hw_log_open();
	if (ret == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	can_main_hw_log_close();

	ret = doesFileExist("_tc_stay_s_sys_stat_norm.log");
	XTFW_ASSERT_EQUAL_INT(1, ret);

	/* compare file content */
	get_expected_file_path("tc_status_req_exp.log", file_path);
	TEST_ASSERT_BINARY_FILE(file_path, "_tc_stay_s_sys_stat_norm.log");

	/* -------------------------------------- */
	/* Test Version Request CAN_SYS_M2S_VERSION_REQUEST */
	/* All Modules must answer */
	rx_data[0] = CAN_SYS_M2S_VERSION_REQUEST;

	can_main_hw_set_log_file_name("_tc_stay_s_sys_ver_norm.log");
	ret = can_main_hw_log_open();
	if (ret == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	can_main_hw_log_close();

	ret = doesFileExist("_tc_stay_s_sys_ver_norm.log");
	XTFW_ASSERT_EQUAL_INT(1, ret);

	/* compare file content */
	get_expected_file_path("tc_version_req_exp.log", file_path);
	TEST_ASSERT_BINARY_FILE(file_path, "_tc_stay_s_sys_ver_norm.log");

	/* -------------------------------------- */
	/* Stay Silent MA_W 113 SN:1995998 */
	rx_data[0] = CAN_SYS_M2S_STAY_SILENT;
	rx_data[1] = 0x4D; // 'M'
	rx_data[2] = 0x41; // 'A'
	rx_data[3] = 0x5F; // '_'
	rx_data[4] = 0x57; // 'W'
	rx_data[5] = 0x1E; // serial 1995998 -> hex:0x1e74de
	rx_data[6] = 0x74;
	rx_data[7] = 0xDE;

	dlc = 8;
	module_nr = 113;

	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

	/* -------------------------------------- */
	/* Test Status Request CAN_SYS_M2S_STATUS_REQUEST */
	/* All Modules except MA_W must answer */
	memset(rx_data, 0x00, 8);
	rx_data[0] = CAN_SYS_M2S_STATUS_REQUEST;
	dlc = 8;
	module_nr = 1;

	can_main_hw_set_log_file_name("_tc_stay_s_sys_stat_no_ma_w.log");
	ret = can_main_hw_log_open();
	if (ret == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	can_main_hw_log_close();

	ret = doesFileExist("_tc_stay_s_sys_stat_no_ma_w.log");
	XTFW_ASSERT_EQUAL_INT(1, ret);

	/* compare file content */
	get_expected_file_path("tc_status_req_no_ma_w_exp.log", file_path);
	TEST_ASSERT_BINARY_FILE(file_path, "_tc_stay_s_sys_stat_no_ma_w.log");

	/* -------------------------------------- */
	/* Test Version Request CAN_SYS_M2S_VERSION_REQUEST */
	/* All Modules except MA_W must answer */
	rx_data[0] = CAN_SYS_M2S_VERSION_REQUEST;

	can_main_hw_set_log_file_name("_tc_stay_s_sys_ver_no_ma_w.log");
	ret = can_main_hw_log_open();
	if (ret == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	can_main_hw_log_close();

	ret = doesFileExist("_tc_stay_s_sys_ver_no_ma_w.log");
	XTFW_ASSERT_EQUAL_INT(1, ret);

	/* compare file content */
	get_expected_file_path("tc_version_req_no_ma_w_exp.log", file_path);
	TEST_ASSERT_BINARY_FILE(file_path, "_tc_stay_s_sys_ver_no_ma_w.log");

	/* -------------------------------------- */
	/* Awake MA_W 113 SN:1995998 */
	rx_data[0] = CAN_SYS_M2S_AWAKE;
	rx_data[1] = 0x4D; // 'M'
	rx_data[2] = 0x41; // 'A'
	rx_data[3] = 0x5F; // '_'
	rx_data[4] = 0x57; // 'W'
	rx_data[5] = 0x1E; // serial 1995998 -> hex:0x1e74de
	rx_data[6] = 0x74;
	rx_data[7] = 0xDE;

	dlc = 8;
	module_nr = 113;

	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

	/* -------------------------------------- */
	/* Test Status Request CAN_SYS_M2S_STATUS_REQUEST */
	/* All Modules must answer */
	memset(rx_data, 0x00, 8);
	rx_data[0] = CAN_SYS_M2S_STATUS_REQUEST;
	dlc = 8;
	module_nr = 1;

	can_main_hw_set_log_file_name("_tc_stay_s_sys_stat_norm_2.log");
	ret = can_main_hw_log_open();
	if (ret == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	can_main_hw_log_close();

	ret = doesFileExist("_tc_stay_s_sys_stat_norm_2.log");
	XTFW_ASSERT_EQUAL_INT(1, ret);

	/* compare file content */
	get_expected_file_path("tc_status_req_exp.log", file_path);
	TEST_ASSERT_BINARY_FILE(file_path, "_tc_stay_s_sys_stat_norm_2.log");

	/* -------------------------------------- */
	/* Test Version Request CAN_SYS_M2S_VERSION_REQUEST */
	/* All Modules must answer */
	rx_data[0] = CAN_SYS_M2S_VERSION_REQUEST;

	can_main_hw_set_log_file_name("_tc_stay_s_sys_ver_norm_2.log");
	ret = can_main_hw_log_open();
	if (ret == EXIT_FAILURE)
	{
		TEST_FAIL_MESSAGE("log open fails");
	}

	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	can_main_hw_log_close();

	ret = doesFileExist("_tc_stay_s_sys_ver_norm_2.log");
	XTFW_ASSERT_EQUAL_INT(1, ret);

	/* compare file content */
	get_expected_file_path("tc_version_req_exp.log", file_path);
	TEST_ASSERT_BINARY_FILE(file_path, "_tc_stay_s_sys_ver_norm_2.log");
}

/**
 * @test test_sys_system_time
 *   - Step 1 Do not set a system time callback
 *   - Step 2 Receive a current system time command -> no error must occur.
 *   - Step 3 Set a system time callback
 *   - Step 4 Receive a current system time command -> no error must occur and the callback must be called.
 *   - Step 5 Compare the received system time with the expected system time
 * @brief This tests the current system time command callback.
 */
void test_sys_system_time(void)
{
	li_can_slv_system_time_t _empty_sys_time = {0};
	li_can_slv_system_time_t _expected_sys_time = {.system_time = 1234567, .time_zone_offset = -7, .dst_active = 1};
	lcsa_errorcode_t err = LCSA_ERROR_OK;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	lcsa_module_number_t module_nr = 1;
	uint16_t dlc = 8;

	rx_data[0] = CAN_SYS_M2S_CURRENT_SYSTEM_TIME;
	rx_data[1] = 0x00;
	rx_data[2] = 0x00; // -
	rx_data[3] = 0x12; // |_ system time 1234567
	rx_data[4] = 0xD6; // |
	rx_data[5] = 0x87; // -
	rx_data[6] = 0xF9; // time zone -7
	rx_data[7] = 0x01; // dst active

	memset(&received_system_time, 0x00, sizeof(li_can_slv_system_time_t));

	/* Test without callback */
	/* Receive current system time command */
	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	XTFW_ASSERT(_empty_sys_time.system_time == received_system_time.system_time);
	XTFW_ASSERT(_empty_sys_time.time_zone_offset == received_system_time.time_zone_offset);
	XTFW_ASSERT(_empty_sys_time.dst_active == received_system_time.dst_active);

	/* Test with callback */
	/* set system time callback */
	lcsa_set_system_time_cbk(&system_time_cbk);

	/* Receive current system time command */
	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	XTFW_ASSERT(_expected_sys_time.system_time == received_system_time.system_time);
	XTFW_ASSERT(_expected_sys_time.time_zone_offset == received_system_time.time_zone_offset);
	XTFW_ASSERT(_expected_sys_time.dst_active == received_system_time.dst_active);
}

/**
 * @test test_sys_msg_change_module_nr
 * @brief This test checks the change module number of a MA_W Module from module
 * number 113 to 114
 */
void test_sys_msg_change_module_nr(void)
{
	lcsa_errorcode_t err = LCSA_ERROR_OK;
	byte_t rx_data[8] = { 0, 0, 114, 0, 0, 0x1e, 0x74, 0xde };
	lcsa_module_number_t module_nr = 113;
	uint16_t dlc = 8;
	uint16_t module_found = 0;
	uint16_t table_pos = UINT16_MAX;
	can_config_module_silent_t module_silent;

	// first check if the module available, and not silent
	module_nr = 113;
	rx_data[2] = 114;
	err = can_config_module_nr_valid(module_nr, &table_pos, &module_silent, &module_found);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	XTFW_ASSERT_EQUAL_INT16(1, module_found);
	XTFW_ASSERT_EQUAL_INT16(3, table_pos); // should be on table position 3
	XTFW_ASSERT_EQUAL_UINT(LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE, module_silent);

	/* simulate a change module number request */
	rx_data[0] = CAN_SYS_M2S_CHANGE_MODULE_NUMBER;
	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

	// should stay awake but on module number 114
	module_nr = 114;
	err = can_config_module_nr_valid(module_nr, &table_pos, &module_silent, &module_found);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	XTFW_ASSERT_EQUAL_INT16(1, module_found);
	XTFW_ASSERT_EQUAL_INT16(3, table_pos); // should be on table position 3
	XTFW_ASSERT_EQUAL_UINT(LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE, module_silent);

	/* simulate a change module number request twice with same parameter
	 * this is possible and should not throw any error
	 */
	rx_data[0] = CAN_SYS_M2S_CHANGE_MODULE_NUMBER;
	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

	err = can_config_module_nr_valid(module_nr, &table_pos, &module_silent, &module_found);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	XTFW_ASSERT_EQUAL_INT16(1, module_found);
	XTFW_ASSERT_EQUAL_INT16(3, table_pos); // should be on table position 3
	XTFW_ASSERT_EQUAL_UINT(LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE, module_silent);

	/* add a change callback */
	lsca_set_module_nr_change_cbk_voter(&app_module_change_vote_module_nr_change_valid);

	/* change back to 113 so change sender and receiver module number */
	module_nr = 114;
	rx_data[2] = 113;
	/* simulate a change module number request */
	rx_data[0] = CAN_SYS_M2S_CHANGE_MODULE_NUMBER;
	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

	err = can_config_module_nr_valid(module_nr, &table_pos, &module_silent, &module_found);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	XTFW_ASSERT_EQUAL_INT16(0, module_found);

	// but it should be available on module number 113 now
	module_nr = 113;
	err = can_config_module_nr_valid(module_nr, &table_pos, &module_silent, &module_found);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	XTFW_ASSERT_EQUAL_INT16(1, module_found);
	XTFW_ASSERT_EQUAL_INT16(3, table_pos); // should be on table position 3
	XTFW_ASSERT_EQUAL_UINT(LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE, module_silent);

	// now change but with the same number so module should be silent
	module_nr = 113;
	rx_data[2] = 113;
	/* simulate a change module number request */
	rx_data[0] = CAN_SYS_M2S_CHANGE_MODULE_NUMBER;
	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);

	err = can_config_module_nr_valid(module_nr, &table_pos, &module_silent, &module_found);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	XTFW_ASSERT_EQUAL_INT16(1, module_found);
	XTFW_ASSERT_EQUAL_INT16(3, table_pos); // should be on table position 3
	XTFW_ASSERT_EQUAL_UINT(LI_CAN_SLV_CONFIG_MODULE_STATE_SILENT, module_silent);

	// set back to awake
	lcsa_set_module_silent_awake_from_type_and_nr(APP_MA_W_MODULE_TYPE, module_nr, LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE);
	// now module should be awake
	err = can_config_module_nr_valid(module_nr, &table_pos, &module_silent, &module_found);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	XTFW_ASSERT_EQUAL_INT16(1, module_found);
	XTFW_ASSERT_EQUAL_INT16(3, table_pos); // should be on table position 3
	XTFW_ASSERT_EQUAL_UINT(LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE, module_silent);

	// now change to a invalid number
	module_nr = 113;
	rx_data[2] = APP_MA_W_MODULE_NR_MAX + 1;
	/* simulate a change module number request */
	rx_data[0] = CAN_SYS_M2S_CHANGE_MODULE_NUMBER;
	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(ERR_MSG_CAN_CHANGING_TO_INVALID_MODULE_NR, err);

	err = can_config_module_nr_valid(module_nr, &table_pos, &module_silent, &module_found);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	XTFW_ASSERT_EQUAL_INT16(1, module_found);
	XTFW_ASSERT_EQUAL_INT16(3, table_pos); // should be on table position 3
	XTFW_ASSERT_EQUAL_UINT(LI_CAN_SLV_CONFIG_MODULE_STATE_AWAKE, module_silent);
}

/**
 * @test test_sys_msg_factory_reset
 * @brief test the factory reset callback
 */
void test_sys_msg_factory_reset(void)
{
	lcsa_errorcode_t err = LCSA_ERROR_OK;
	byte_t rx_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	lcsa_module_number_t module_nr = 1;
	uint16_t dlc = 8;

	// full reset
	uint8_t reset_reason_expected = 0x02;

	/* set first status request callback */
	lcsa_set_factory_reset_cbk(&factory_reset_cbk);

	/* simulate a status request message */
	rx_data[0] = CAN_SYS_M2S_FACTORY_RESET;

	// set reset action to full reset
	rx_data[1] = reset_reason_expected;

	reset_reason_actual = 0x00;

	err = can_sys_msg_rx(module_nr, dlc, rx_data);
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, err);
	XTFW_ASSERT_EQUAL_UINT(reset_reason_expected, reset_reason_actual);
}


/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/
static void first_status_request_cbk(void)
{
	first_status_request_cnt++;
}

static void system_time_cbk(li_can_slv_system_time_t system_time)
{
	received_system_time = system_time;
}

static void factory_reset_cbk(uint8_t reset_reason)
{
	reset_reason_actual = reset_reason;
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
