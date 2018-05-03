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
 * @file io_app_ma_w.h
 * @brief header to io_app_ma_w module
 * @addtogroup app_ma_w
 * @{
 */

#ifndef IO_APP_MA_W_H_
#define IO_APP_MA_W_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include "io_app_port.h"

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
#ifndef APP_MA_W_MODULE_NR_DEF
#define APP_MA_W_MODULE_NR_DEF (113)
#endif

#ifndef APP_MA_W_MODULE_NR_MIN
#define APP_MA_W_MODULE_NR_MIN (113)
#endif

#ifndef APP_MA_W_MODULE_NR_MAX
#define APP_MA_W_MODULE_NR_MAX (117)
#endif

#ifndef APP_MA_W_NUM_OF_MODULES
#define APP_MA_W_NUM_OF_MODULES (1)
#endif

#define APP_MA_W_MODULE_TYPE	"MA_W\0"

#define TUPLE_ID_CNFG_DEFAULT	0xFFFF

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/
/**
 * @brief defines main or monitor types
 */
typedef enum
{
	APP_MA_W_TYPE_UDEF = 0, /**< undefined type */
	APP_MA_W_TYPE_MAIN = 1, /**< main module type */
	APP_MA_W_TYPE_MON = 2 /**< monitor module type */
} app_ma_w_type_t;

/* set to byte alignment */

#pragma pack(1)


/* rx objects on MA_W CAN module */
typedef struct app_ma_w_rx_object1_tag
{
	uint16_t word0;
	uint16_t word1;
	uint16_t word2;
	uint16_t word3;
} app_ma_w_rx_object1_t;

typedef struct app_ma_w_rx_object2_tag
{
	uint16_t word0;
	uint16_t word1;
	uint16_t word2;
	uint16_t word3;
} app_ma_w_rx_object2_t;

typedef struct app_ma_w_rx_object3_tag
{
	uint16_t word0;
	uint16_t word1;
	uint16_t word2;
	uint16_t word3;
} app_ma_w_rx_object3_t;

typedef struct app_ma_w_rx_object4_tag
{
	uint16_t word0;
	uint16_t word1;
	uint16_t word2;
	uint16_t word3;
} app_ma_w_rx_object4_t;

/* tx objects on MA_W CAN module */
typedef struct app_ma_w_tx_object1_tag
{
	uint16_t word0;
	uint16_t word1;
	uint16_t word2;
	uint16_t word3;
} app_ma_w_tx_object1_t;

typedef struct app_ma_w_tx_object2_tag
{
	uint16_t word0;
	uint16_t word1;
	uint16_t word2;
	uint16_t word3;
} app_ma_w_tx_object2_t;

typedef struct app_ma_w_tx_object3_tag
{
	uint16_t word0;
	uint16_t word1;
	uint16_t word2;
	uint16_t word3;
} app_ma_w_tx_object3_t;

typedef struct app_ma_w_tx_object4_tag
{
	uint16_t word0;
	uint16_t word1;
	uint16_t word2;
	uint16_t word3;
} app_ma_w_tx_object4_t;


/* rx data structure for one MA_W Module (Master to Module) */
typedef struct app_ma_w_can_rx_data_tag
{
	app_ma_w_rx_object1_t rx_obj1;
	app_ma_w_rx_object2_t rx_obj2;
	app_ma_w_rx_object3_t rx_obj3;
	app_ma_w_rx_object4_t rx_obj4;
} app_ma_w_can_rx_data_t;

/* tx data structure for one MA_W Module (Module to Master) */
typedef struct app_ma_w_can_tx_data_tag
{
	app_ma_w_tx_object1_t tx_obj1;
	app_ma_w_tx_object2_t tx_obj2;
	app_ma_w_tx_object3_t tx_obj3;
	app_ma_w_tx_object4_t tx_obj4;
} app_ma_w_can_tx_data_t;


#pragma pack() /* reset to default alignment */

typedef struct
{
	app_ma_w_can_rx_data_t can_rx_data;
	app_ma_w_can_tx_data_t can_tx_data;
} app_ma_w_data_t;


/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/
extern app_ma_w_can_rx_data_t app_ma_w_can_rx_data[APP_MA_W_NUM_OF_MODULES];
extern app_ma_w_can_tx_data_t app_ma_w_can_tx_data[APP_MA_W_NUM_OF_MODULES];

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
/**
 * @brief initialization of the MA_W module
 * @param modnr used module number
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_ma_w_init(uint16_t module_idx, li_can_slv_module_nr_t modnr);

/**
 * @brief process output function for the MA_W module
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_ma_w_process_input(void);

/**
 * @brief process output function for the MA_W module
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_ma_w_process_output(void);

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
/**
* @brief set word0 of tx-object 1
* @param module_idx index of module
* @param value word0
*/
extern void app_ma_w_tx1_set_word0(uint16_t module_idx, uint16_t value);

/**
* @brief set word1 of tx-object 1
* @param module_idx index of module
* @param value word1
*/
extern void app_ma_w_tx1_set_word1(uint16_t module_idx, uint16_t value);

/**
* @brief set word2 of tx-object 1
* @param module_idx index of module
* @param value word2
*/
extern void app_ma_w_tx1_set_word2(uint16_t module_idx, uint16_t value);

/**
* @brief set word3 of tx-object 1
* @param module_idx index of module
* @param value word3
*/
extern void app_ma_w_tx1_set_word3(uint16_t module_idx, uint16_t value);

/**
* @brief set word0 of tx-object 2
* @param module_idx index of module
* @param value word0
*/
extern void app_ma_w_tx2_set_word0(uint16_t module_idx, uint16_t value);

/**
* @brief set word1 of tx-object 2
* @param module_idx index of module
* @param value word1
*/
extern void app_ma_w_tx2_set_word1(uint16_t module_idx, uint16_t value);

/**
* @brief set word2 of tx-object 2
* @param module_idx index of module
* @param value word2
*/
extern void app_ma_w_tx2_set_word2(uint16_t module_idx, uint16_t value);

/**
* @brief set word3 of tx-object 2
* @param module_idx index of module
* @param value word3
*/
extern void app_ma_w_tx2_set_word3(uint16_t module_idx, uint16_t value);

/**
* @brief set word0 of tx-object 3
* @param module_idx index of module
* @param value word0
*/
extern void app_ma_w_tx3_set_word0(uint16_t module_idx, uint16_t value);

/**
* @brief set word1 of tx-object 3
* @param module_idx index of module
* @param value word1
*/
extern void app_ma_w_tx3_set_word1(uint16_t module_idx, uint16_t value);

/**
* @brief set word2 of tx-object 3
* @param module_idx index of module
* @param value word2
*/
extern void app_ma_w_tx3_set_word2(uint16_t module_idx, uint16_t value);

/**
* @brief set word3 of tx-object 3
* @param module_idx index of module
* @param value word3
*/
extern void app_ma_w_tx3_set_word3(uint16_t module_idx, uint16_t value);

/**
* @brief set word0 of tx-object 4
* @param module_idx index of module
* @param value word0
*/
extern void app_ma_w_tx4_set_word0(uint16_t module_idx, uint16_t value);

/**
* @brief set word1 of tx-object 4
* @param module_idx index of module
* @param value word1
*/
extern void app_ma_w_tx4_set_word1(uint16_t module_idx, uint16_t value);

/**
* @brief set word2 of tx-object 4
* @param module_idx index of module
* @param value word2
*/
extern void app_ma_w_tx4_set_word2(uint16_t module_idx, uint16_t value);

/**
* @brief set word3 of tx-object 4
* @param module_idx index of module
* @param value word3
*/
extern void app_ma_w_tx4_set_word3(uint16_t module_idx, uint16_t value);


/**
* @brief get value of word0 of rx-object 1
* @param module_idx index of module
* @return word0 of rx-object 1
*/
extern uint16_t app_ma_w_rx1_get_word0(uint16_t module_idx);

/**
* @brief get value of word1 of rx-object 1
* @param module_idx index of module
* @return word1 of rx-object 1
*/
extern uint16_t app_ma_w_rx1_get_word1(uint16_t module_idx);

/**
* @brief get value of word2 of rx-object 1
* @param module_idx index of module
* @return word2 of rx-object 1
*/
extern uint16_t app_ma_w_rx1_get_word2(uint16_t module_idx);

/**
* @brief get value of word3 of rx-object 1
* @param module_idx index of module
* @return word3 of rx-object 1
*/
extern uint16_t app_ma_w_rx1_get_word3(uint16_t module_idx);

/**
* @brief get value of word0 of rx-object 2
* @param module_idx index of module
* @return word0 of rx-object 2
*/
extern uint16_t app_ma_w_rx2_get_word0(uint16_t module_idx);

/**
* @brief get value of word1 of rx-object 2
* @param module_idx index of module
* @return word1 of rx-object 2
*/
extern uint16_t app_ma_w_rx2_get_word1(uint16_t module_idx);

/**
* @brief get value of word2 of rx-object 2
* @param module_idx index of module
* @return word2 of rx-object 2
*/
extern uint16_t app_ma_w_rx2_get_word2(uint16_t module_idx);

/**
* @brief get value of word3 of rx-object 2
* @param module_idx index of module
* @return word3 of rx-object 2
*/
extern uint16_t app_ma_w_rx2_get_word3(uint16_t module_idx);

/**
* @brief get value of word0 of rx-object 3
* @param module_idx index of module
* @return word0 of rx-object 3
*/
extern uint16_t app_ma_w_rx3_get_word0(uint16_t module_idx);

/**
* @brief get value of word1 of rx-object 3
* @param module_idx index of module
* @return word1 of rx-object 3
*/
extern uint16_t app_ma_w_rx3_get_word1(uint16_t module_idx);

/**
* @brief get value of word2 of rx-object 3
* @param module_idx index of module
* @return word2 of rx-object 3
*/
extern uint16_t app_ma_w_rx3_get_word2(uint16_t module_idx);

/**
* @brief get value of word3 of rx-object 3
* @param module_idx index of module
* @return word3 of rx-object 3
*/
extern uint16_t app_ma_w_rx3_get_word3(uint16_t module_idx);

/**
* @brief get value of word0 of rx-object 4
* @param module_idx index of module
* @return word0 of rx-object 4
*/
extern uint16_t app_ma_w_rx4_get_word0(uint16_t module_idx);

/**
* @brief get value of word1 of rx-object 4
* @param module_idx index of module
* @return word1 of rx-object 4
*/
extern uint16_t app_ma_w_rx4_get_word1(uint16_t module_idx);

/**
* @brief get value of word2 of rx-object 4
* @param module_idx index of module
* @return word2 of rx-object 4
*/
extern uint16_t app_ma_w_rx4_get_word2(uint16_t module_idx);

/**
* @brief get value of word3 of rx-object 4
* @param module_idx index of module
* @return word3 of rx-object 4
*/
extern uint16_t app_ma_w_rx4_get_word3(uint16_t module_idx);

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_APP_MA_W_H_

/** @} */
