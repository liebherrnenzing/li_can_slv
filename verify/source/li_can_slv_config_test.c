
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

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
// setUp will be called before each test
void setUp(void)
{

}

// tearDown will be called after each test
void tearDown(void)
{

}


/**
 * @test test_process_req
 * @brief test the process request handling
 */
void test_lcsa_init_deinit(void)
{
	uint16_t msg_obj_table_nr = 0;

	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, lcsa_init(LCSA_BAUD_RATE_DEFAULT));

	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, app_frc2_init(APP_FRC2_MODULE_NR_DEF));
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, app_incx_init(APP_INCX_MODULE_NR_DEF));
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, app_inxy_init(APP_INXY_MODULE_NR_DEF));
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, app_ma_w_init(0, APP_MA_W_MODULE_NR_DEF));
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, lcsa_start());

	XTFW_ASSERT_EQUAL_UINT(LI_CAN_SLV_ERR_OK, can_main_get_next_free_msg_obj(&msg_obj_table_nr));
	XTFW_ASSERT_EQUAL_UINT(13, msg_obj_table_nr);

	XTFW_ASSERT_EQUAL_UINT(LI_CAN_SLV_ERR_OK, can_mon_get_next_free_msg_obj(&msg_obj_table_nr));
	XTFW_ASSERT_EQUAL_UINT(13, msg_obj_table_nr);

	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, lcsa_deinit());
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, lcsa_init(LCSA_BAUD_RATE_DEFAULT));

	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, app_frc2_init(APP_FRC2_MODULE_NR_DEF));
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, app_incx_init(APP_INCX_MODULE_NR_DEF));
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, app_inxy_init(APP_INXY_MODULE_NR_DEF));
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, app_ma_w_init(0, APP_MA_W_MODULE_NR_DEF));
	XTFW_ASSERT_EQUAL_UINT(LCSA_ERROR_OK, lcsa_start());

	XTFW_ASSERT_EQUAL_UINT(LI_CAN_SLV_ERR_OK, can_main_get_next_free_msg_obj(&msg_obj_table_nr));
	XTFW_ASSERT_EQUAL_UINT(13, msg_obj_table_nr);

	XTFW_ASSERT_EQUAL_UINT(LI_CAN_SLV_ERR_OK, can_mon_get_next_free_msg_obj(&msg_obj_table_nr));
	XTFW_ASSERT_EQUAL_UINT(13, msg_obj_table_nr);
}
