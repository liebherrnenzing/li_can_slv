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
 * @brief The can_sync module implements functions and data structures used for
 * the synchronous protocol handling of each logical CAN module.
 * \remarks For each module a maximum number of 4 transmit and 4 received objects
 * (CAN messages) with a DLC (data length code) from 0 to 8 is defined
 *
 * The CAN synchronous protocol is local proved on the module by using of two
 * CAN-controllers. On the main CAN-controller both directions (receive and transmit)
 * are connected to the CAN-transceiver.
 */

/**
 * @file io_can_sync.c
 * @addtogroup can_sync
 * @{
 */
/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/sync/io_can_sync.h>
#include <li_can_slv/core/io_can.h>
#include <li_can_slv/config/io_can_config.h>

#include "io_can_port.h"

#include <li_can_slv/error/io_can_errno.h>

#include <li_can_slv/core/io_can_main.h>
#include <li_can_slv/core/io_can_mon.h>

#ifdef SHOW_CAN_SYNC
#include "io_var.h"
#ifdef TESTSYS_SYSTEM
#include "io_testsys.h"
#endif // #ifdef TESTSYS_SYSTEM
#endif // #ifdef SHOW_CAN_SYNC

#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_debug.h"
#endif // #ifdef LI_CAN_SLV_DEBUG

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
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/
can_sync_t can_sync = { 0 }; /**< */
#ifdef LI_CAN_SLV_MON
static can_sync_data_t can_sync_data_main_tx[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES]; /**< */
static can_sync_data_t can_sync_data_mon_rx[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES]; /**< */
#endif // #ifdef LI_CAN_SLV_MON

static void (*can_sync_process_call)(void) = NULL;
static void (*can_sync_process_image_valid_cbk_table[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES])(void);
static void (*can_sync_process_image_not_valid_cbk_table[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES])(void);
static void (*can_sync_process_request_rx_cbk_table[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES])(void);
static uint16_t can_sync_first_process_detect = 0;
static void (*can_sync_first_process_request_call)(void) = NULL;

#ifdef SHOW_CAN_SYNC
/*--------------------------------------------------------------------------*/
/* system variable CAN synchronous process                                  */
/*--------------------------------------------------------------------------*/
static var_change_t can_sync_node_a_var_change[] = /**< */
{
	{VAR_STATE_FRC_SET,	"ms",	""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"ms",	""},

	{VAR_STATE_FRC_SET,	"ms",	""},
	{VAR_STATE_FRC_SET,	"ms",	""},
	{VAR_STATE_FRC_SET,	"ms",	""},
	{VAR_STATE_FRC_SET,	"ms",	""},

	{VAR_STATE_FRC_SET,	"ms",	""},
	{VAR_STATE_FRC_SET,	"ms",	""},
	{VAR_STATE_FRC_SET,	"ms",	""},
	{VAR_STATE_FRC_SET,	"ms",	""},

	{VAR_STATE_FRC_SET,	"ms",	""}
};

static const var_const_t can_sync_node_a_var_const[] = /**< */
{
	{"sync.ma.time",	&can_sync.main_pr_time,				VAR_UINT32,	0,		0xFFFFFFFFL,	1,		VAR_ID_NOT_USED,	NULL,	&can_sync_node_a_var_change[0]},
	{"sync.ma.cnt",		&can_sync.main_pr_cnt,				VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,	&can_sync_node_a_var_change[1]},
	{"sync.ma.dlc",		&can_sync.main_pr_dlc,				VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,	&can_sync_node_a_var_change[2]},
	{"sync.ptv",		&can_sync.pr_time_valid,			VAR_UINT32,	0,		0xFFFFFFFFL,	1,		VAR_ID_NOT_USED,	NULL,	&can_sync_node_a_var_change[3]},

	{"sync.ptv.01",		&can_sync.pr_time_valid_module[0],	VAR_UINT32,	0,		0xFFFFFFFFL,	1,		VAR_ID_NOT_USED,	NULL,	&can_sync_node_a_var_change[4]},
	{"sync.ptv.02",		&can_sync.pr_time_valid_module[1],	VAR_UINT32,	0,		0xFFFFFFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_node_a_var_change[5]},
	{"sync.ptv.03",		&can_sync.pr_time_valid_module[2],	VAR_UINT32,	0,		0xFFFFFFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_node_a_var_change[6]},
	{"sync.ptv.04",		&can_sync.pr_time_valid_module[3],	VAR_UINT32,	0,		0xFFFFFFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_node_a_var_change[7]},

	{"sync.ptv.05",		&can_sync.pr_time_valid_module[4],	VAR_UINT32,	0,		0xFFFFFFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_node_a_var_change[8]},
	{"sync.ptv.06",		&can_sync.pr_time_valid_module[5],	VAR_UINT32,	0,		0xFFFFFFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_node_a_var_change[9]},
	{"sync.ptv.07",		&can_sync.pr_time_valid_module[6],	VAR_UINT32,	0,		0xFFFFFFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_node_a_var_change[10]},
	{"sync.ptv.08",		&can_sync.pr_time_valid_module[7],	VAR_UINT32,	0,		0xFFFFFFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_node_a_var_change[11]},

	{"sync.pr.periode",	&can_sync.pr_periode,				VAR_UINT32,	0,		0xFFFFFFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_node_a_var_change[12]},
	{VAR_END_OF_TABLE}
};

#ifdef LI_CAN_SLV_MON
static var_change_t can_sync_node_b_var_change[] = /**< */
{
	{VAR_STATE_FRC_SET,	"ms",	""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""}
};

static const var_const_t can_sync_node_b_var_const[] = /**< */
{
	{"sync.mo.time",	&can_sync.mon_pr_time,		VAR_UINT32,	0,		0xFFFFFFFFL,	1,		VAR_ID_NOT_USED,	NULL,	&can_sync_node_b_var_change[0]},
	{"sync.mo.cnt",		&can_sync.mon_pr_cnt,		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,	&can_sync_node_b_var_change[1]},
	{"sync.mo.dlc",		&can_sync.mon_pr_dlc,		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,	&can_sync_node_b_var_change[2]},
	{VAR_END_OF_TABLE}
};
#endif // #ifdef LI_CAN_SLV_MON

static var_change_t can_sync_err_node_a_var_change[] = /**< */
{
	{VAR_STATE_FRC_SET,	"",	""}
};

static const var_const_t can_sync_err_node_a_var_const[] = /**< */
{
	{"sync.e.ma.cnt",	&can_sync.err.main_pr_cnt,		VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_a_var_change[0]},
	{VAR_END_OF_TABLE}
};

static var_change_t can_sync_err_rx_node_a_var_change[] = /**< */
{
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},

	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""}
};

static const var_const_t can_sync_err_rx_node_a_var_const[] = /**< */
{
	{"sync.e.ma.rx.01",	&can_sync.err.main_rx_cnt[0],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_sync_err_rx_node_a_var_change[0]},
	{"sync.e.ma.rx.02",	&can_sync.err.main_rx_cnt[1],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_sync_err_rx_node_a_var_change[1]},
	{"sync.e.ma.rx.03",	&can_sync.err.main_rx_cnt[2],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_sync_err_rx_node_a_var_change[2]},
	{"sync.e.ma.rx.04",	&can_sync.err.main_rx_cnt[3],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_sync_err_rx_node_a_var_change[3]},
	{"sync.e.ma.rx.05",	&can_sync.err.main_rx_cnt[4],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_sync_err_rx_node_a_var_change[4]},
	{"sync.e.ma.rx.06",	&can_sync.err.main_rx_cnt[5],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_sync_err_rx_node_a_var_change[5]},
	{"sync.e.ma.rx.07",	&can_sync.err.main_rx_cnt[6],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_sync_err_rx_node_a_var_change[6]},
	{"sync.e.ma.rx.08",	&can_sync.err.main_rx_cnt[7],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_sync_err_rx_node_a_var_change[7]},
	{VAR_END_OF_TABLE}
};

static var_change_t can_sync_err_tx_node_a_var_change[] = /**< */
{
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},

	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""}
};

static const var_const_t can_sync_err_tx_node_a_var_const[] = /**< */
{
	{"sync.e.ma.tx.01",	&can_sync.err.main_tx_cnt[0],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_sync_err_tx_node_a_var_change[0]},
	{"sync.e.ma.tx.02",	&can_sync.err.main_tx_cnt[1],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_sync_err_tx_node_a_var_change[1]},
	{"sync.e.ma.tx.03",	&can_sync.err.main_tx_cnt[2],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_sync_err_tx_node_a_var_change[2]},
	{"sync.e.ma.tx.04",	&can_sync.err.main_tx_cnt[3],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_sync_err_tx_node_a_var_change[3]},
	{"sync.e.ma.tx.05",	&can_sync.err.main_tx_cnt[4],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_sync_err_tx_node_a_var_change[4]},
	{"sync.e.ma.tx.06",	&can_sync.err.main_tx_cnt[5],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_sync_err_tx_node_a_var_change[5]},
	{"sync.e.ma.tx.07",	&can_sync.err.main_tx_cnt[6],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_sync_err_tx_node_a_var_change[6]},
	{"sync.e.ma.tx.08",	&can_sync.err.main_tx_cnt[7],	VAR_UINT16,	0,	0x0000FFFFL,	1,	VAR_ID_NOT_USED,	NULL,	&can_sync_err_tx_node_a_var_change[7]},
	{VAR_END_OF_TABLE}
};

static var_change_t can_sync_err_data_node_a_var_change[] = /**< */
{
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},

	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""}
};

static const var_const_t can_sync_err_data_node_a_var_const[] = /**< */
{
	{"sync.e.data.01",	&can_sync.err.data[0],	VAR_UINT16,	0,	0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_data_node_a_var_change[0]},
	{"sync.e.data.02",	&can_sync.err.data[1],	VAR_UINT16,	0,	0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_data_node_a_var_change[1]},
	{"sync.e.data.03",	&can_sync.err.data[2],	VAR_UINT16,	0,	0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_data_node_a_var_change[2]},
	{"sync.e.data.04",	&can_sync.err.data[3],	VAR_UINT16,	0,	0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_data_node_a_var_change[3]},

	{"sync.e.data.05",	&can_sync.err.data[4],	VAR_UINT16,	0,	0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_data_node_a_var_change[4]},
	{"sync.e.data.06",	&can_sync.err.data[5],	VAR_UINT16,	0,	0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_data_node_a_var_change[5]},
	{"sync.e.data.07",	&can_sync.err.data[6],	VAR_UINT16,	0,	0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_data_node_a_var_change[6]},
	{"sync.e.data.08",	&can_sync.err.data[7],	VAR_UINT16,	0,	0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_data_node_a_var_change[7]},
	{VAR_END_OF_TABLE}
};

#ifdef LI_CAN_SLV_MON
static var_change_t can_sync_err_node_b_var_change[] = /**< */
{
	{VAR_STATE_FRC_SET,	"",		""},

	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},

	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""},
	{VAR_STATE_FRC_SET,	"",		""}
};

static const var_const_t can_sync_err_node_b_var_const[] = /**< */
{
	{"sync.e.mo.cnt",	&can_sync.err.mon_pr_cnt,		VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_b_var_change[0]},

	{"sync.e.mo.rx.01",	&can_sync.err.mon_rx_cnt[0],	VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_b_var_change[1]},
	{"sync.e.mo.rx.02",	&can_sync.err.mon_rx_cnt[1],	VAR_UINT16,	0,		0x0000FFFFL,	1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_b_var_change[2]},
	{"sync.e.mo.rx.03",	&can_sync.err.mon_rx_cnt[2],	VAR_UINT16,	0,		0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_b_var_change[3]},
	{"sync.e.mo.rx.04",	&can_sync.err.mon_rx_cnt[3],	VAR_UINT16,	0,		0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_b_var_change[4]},
	{"sync.e.mo.rx.05",	&can_sync.err.mon_rx_cnt[4],	VAR_UINT16,	0,		0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_b_var_change[5]},
	{"sync.e.mo.rx.06",	&can_sync.err.mon_rx_cnt[5],	VAR_UINT16,	0,		0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_b_var_change[6]},
	{"sync.e.mo.rx.07",	&can_sync.err.mon_rx_cnt[6],	VAR_UINT16,	0,		0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_b_var_change[7]},
	{"sync.e.mo.rx.08",	&can_sync.err.mon_rx_cnt[7],	VAR_UINT16,	0,		0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_b_var_change[8]},

	{"sync.e.mo.tx.01",	&can_sync.err.mon_tx_cnt[0],	VAR_UINT16,	0,		0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_b_var_change[9]},
	{"sync.e.mo.tx.02",	&can_sync.err.mon_tx_cnt[1],	VAR_UINT16,	0,		0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_b_var_change[10]},
	{"sync.e.mo.tx.03",	&can_sync.err.mon_tx_cnt[2],	VAR_UINT16,	0,		0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_b_var_change[11]},
	{"sync.e.mo.tx.04",	&can_sync.err.mon_tx_cnt[3],	VAR_UINT16,	0,		0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_b_var_change[12]},
	{"sync.e.mo.tx.05",	&can_sync.err.mon_tx_cnt[4],	VAR_UINT16,	0,		0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_b_var_change[13]},
	{"sync.e.mo.tx.06",	&can_sync.err.mon_tx_cnt[5],	VAR_UINT16,	0,		0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_b_var_change[14]},
	{"sync.e.mo.tx.07",	&can_sync.err.mon_tx_cnt[6],	VAR_UINT16,	0,		0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_b_var_change[15]},
	{"sync.e.mo.tx.08",	&can_sync.err.mon_tx_cnt[7],	VAR_UINT16,	0,		0x0000FFFFL,    1,		VAR_ID_NOT_USED,	NULL,	&can_sync_err_node_b_var_change[16]},

	{VAR_END_OF_TABLE}
};
#endif // #ifdef LI_CAN_SLV_MON

/*--------------------------------------------------------------------------*/
/* IO-TESTSYS-PAGE system CAN synchronous process                           */
/*--------------------------------------------------------------------------*/
static const testsys_var_t can_sync_node_ab_testsys[] = /**< */
{
	{&can_sync_node_a_var_const[0], 	1, 1},
	{&can_sync_node_a_var_const[1], 	1, 2},
	{&can_sync_node_a_var_const[2], 	1, 3},
	{&can_sync_node_a_var_const[3], 	1, 5},
	{&can_sync_node_a_var_const[4], 	1, 6},
	{&can_sync_node_a_var_const[5], 	1, 7},
	{&can_sync_node_a_var_const[6], 	1, 8},
	{&can_sync_node_a_var_const[7], 	1, 9},
	{&can_sync_node_a_var_const[8], 	1, 10},
	{&can_sync_node_a_var_const[9], 	1, 11},
	{&can_sync_node_a_var_const[10],	1, 12},
	{&can_sync_node_a_var_const[11],	1, 13},
	{&can_sync_node_a_var_const[12],	1, 14},

#ifdef LI_CAN_SLV_MON
	{&can_sync_node_b_var_const[0],		48,	1},
	{&can_sync_node_b_var_const[1],		48,	2},
	{&can_sync_node_b_var_const[2],		48,	3},
#endif // #ifdef LI_CAN_SLV_MON
	{TESTSYS_VAR_END_OF_TABLE,			0,	0}
};

static const testsys_var_t can_sync_err_node_ab_testsys[] = /**< */
{
	{&can_sync_err_node_a_var_const[0],		1,	1},

	{&can_sync_err_rx_node_a_var_const[0],	1, 3},
	{&can_sync_err_rx_node_a_var_const[1],	1, 4},
	{&can_sync_err_rx_node_a_var_const[2],	1, 5},
	{&can_sync_err_rx_node_a_var_const[3],	1, 6},
	{&can_sync_err_rx_node_a_var_const[4],	1, 7},
	{&can_sync_err_rx_node_a_var_const[5],	1, 8},
	{&can_sync_err_rx_node_a_var_const[6],	1, 9},
	{&can_sync_err_rx_node_a_var_const[7],	1, 10},

	{&can_sync_err_tx_node_a_var_const[0],	1, 12},
	{&can_sync_err_tx_node_a_var_const[1],	1, 13},
	{&can_sync_err_tx_node_a_var_const[2],	1, 14},
	{&can_sync_err_tx_node_a_var_const[3],	1, 15},
	{&can_sync_err_tx_node_a_var_const[4],	1, 16},
	{&can_sync_err_tx_node_a_var_const[5],	1, 17},
	{&can_sync_err_tx_node_a_var_const[6],	1, 18},
	{&can_sync_err_tx_node_a_var_const[7],	1, 19},

	{&can_sync_err_data_node_a_var_const[0],	28,	3},
	{&can_sync_err_data_node_a_var_const[1],	28,	4},
	{&can_sync_err_data_node_a_var_const[2],	28,	5},
	{&can_sync_err_data_node_a_var_const[3],	28,	6},
	{&can_sync_err_data_node_a_var_const[4],	28,	7},
	{&can_sync_err_data_node_a_var_const[5],	28,	8},
	{&can_sync_err_data_node_a_var_const[6],	28,	9},
	{&can_sync_err_data_node_a_var_const[7],	28,	10},

	/** @todo remove variables because of overwrite if LI_CAN_SLV_MON is activated */
#ifdef LI_CAN_SLV_MON
	{&can_sync_err_node_b_var_const[0], 		55, 1},

	{&can_sync_err_node_b_var_const[1], 		55, 3},
	{&can_sync_err_node_b_var_const[2], 		55, 4},
	{&can_sync_err_node_b_var_const[3], 		55, 5},
	{&can_sync_err_node_b_var_const[4], 		55, 6},
	{&can_sync_err_node_b_var_const[5], 		55, 7},
	{&can_sync_err_node_b_var_const[6], 		55, 8},
	{&can_sync_err_node_b_var_const[7], 		55, 9},
	{&can_sync_err_node_b_var_const[8], 		55, 10},

#endif // #ifdef LI_CAN_SLV_MON
	{TESTSYS_VAR_END_OF_TABLE, 0, 0}
};
#endif // #ifdef SHOW_CAN_SYNC

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/
static void li_can_slv_sync_clear_process_image(void);
static li_can_slv_errorcode_t li_can_slv_sync_check_process_image_module(uint16_t table_pos);

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 1
static uint32_t can_sync_get_pr_time_valid_module1(void);
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 1

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 2
static uint32_t can_sync_get_pr_time_valid_module2(void);
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 2

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 3
static uint32_t can_sync_get_pr_time_valid_module3(void);
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 3

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 4
static uint32_t can_sync_get_pr_time_valid_module4(void);
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 4

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 5
static uint32_t can_sync_get_pr_time_valid_module5(void);
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 5

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 6
static uint32_t can_sync_get_pr_time_valid_module6(void);
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 6

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 7
static uint32_t can_sync_get_pr_time_valid_module7(void);
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 7

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 8
static uint32_t can_sync_get_pr_time_valid_module8(void);
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 8

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
/**
 * @brief initializes all internal variables
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_sync_init(void)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t i;

#ifdef LI_CAN_SLV_MON
	can_port_memory_set(can_sync_data_main_tx, 0, sizeof(can_sync_data_t));
	can_port_memory_set(can_sync_data_mon_rx, 0, sizeof(can_sync_data_t));
#endif // #ifdef LI_CAN_SLV_MON

	can_sync.pr_periode = LI_CAN_SLV_SYNC_PROCESS_PERIODE_MAX;
	for (i = 0; i < LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES; i++)
	{
		can_sync_process_image_valid_cbk_table[i] = NULL;
		can_sync_process_image_not_valid_cbk_table[i] = NULL;
		can_sync_process_request_rx_cbk_table[i] = NULL;
	}

#ifdef SHOW_CAN_SYNC
	/*----------------------------------------------------------------------*/
	/* add can sync IO's                                                    */
	/*----------------------------------------------------------------------*/
	err = var_add(&can_sync_node_a_var_const[0]);

	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = var_add(&can_sync_err_node_a_var_const[0]);
	}

	/*----------------------------------------------------------------------*/
	/* add can sync IO's to testsys                                         */
	/*----------------------------------------------------------------------*/
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = testsys_system_page_add(&can_sync_node_ab_testsys[0], "CAN synchronous process system");
	}
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = testsys_system_page_add(&can_sync_err_node_ab_testsys[0], "CAN synchronous process errors system");
	}
#endif // #ifdef SHOW_CAN_SYNC
	return (err);
}

/**
 * @brief checks process request from master via main CAN-controller
 * @remarks Incoming process request from master are checked
 * - of correct DLC (DLC = 0 or DLC = 1 are accepted)
 * - if DLC > 0 of correct process index
 * @param dlc is the data length code of received process request
 * @param[in] can is the data source pointer to received data
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_sync_rx_process_main(uint16_t dlc, byte_t const *can)
{
#ifdef CAN_MAIN_DIAGNOSE
	can_main_diagnose.pr++;
#endif // #ifdef CAN_MAIN_DIAGNOSE

	can = can; // suppress warning

	can_sync.pr_periode = can_port_ticks_2_msec(can_port_get_system_ticks() - can_sync.main_pr_time);
	can_sync.main_pr_time = can_port_get_system_ticks();

	can_sync.main_pr_cnt++;
	can_sync.main_pr_dlc = dlc;
	//	can_sync.main_pr_index = (uint16_t) can[0];
	can_sync.main_pr_flag = 0xFFFF;
	return (LI_CAN_SLV_ERR_OK);
}

/**
 * @brief checks the success of the synchronous data transmission from the master via main CAN-controller
 * @remarks if the main CAN-controller receives the synchronous RX-data from the master following steps are
 * executed.
 * - call of the main RX-conversion-code for the logical module\n
 * - if CAN object (message) has the correct DLC increment the main RX-counter of this object
 * @param table_pos is the position in the CAN configuration module table (maximum defined logical modules
 * are #LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES)
 * @param can_id is the 11bit CAN identifier of the CAN message
 * @param dlc is the data length code of the transmit object
 * @param[in] can is the data pointer of the used CAN message object
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_sync_rx_data_main(uint16_t table_pos, uint16_t can_id, uint16_t dlc, byte_t *can)
{
	li_can_slv_errorcode_t err;
	uint16_t obj;
#ifdef LI_CAN_SLV_DEBUG_SYNC_MAIN
	uint16_t i;
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_MAIN

#ifdef LI_CAN_SLV_SYNC_CHECK_MODULE_NR_AND_DLC
	li_can_slv_module_nr_t module_nr;
#endif // #ifdef LI_CAN_SLV_SYNC_CHECK_MODULE_NR_AND_DLC

	obj = can_id & CAN_SYNC_OBJ_MASK;

#ifdef LI_CAN_SLV_SYNC_CHECK_MODULE_NR_AND_DLC
#ifdef LI_CAN_SLV_MAIN_MON
	if (can_mainmon_type == CAN_MAINMON_TYPE_MAIN)
	{
		/* receive data only from master */
		module_nr = ((can_id - CAN_CONFIG_DATA_RX_MASK) >> 2) + 1;
	}
	else
	{
		/* receive data from master and main CPU */
		module_nr = ((can_id & ~(CAN_CONFIG_DATA_RX_MASK | CAN_CONFIG_DATA_TX_MASK)) >> 2) + 1;
	}
#else // #ifdef LI_CAN_SLV_MAIN_MON
	module_nr = ((can_id - CAN_CONFIG_DATA_RX_MASK) >> 2) + 1;
#endif // #ifdef LI_CAN_SLV_MAIN_MON

	if (module_nr != can_config_module_tab[table_pos].module_nr)
	{
		return (ERR_MSG_CAN_MAIN_RX_WRONG_MODULE_NR);
	}
	if (dlc != can_config_module_tab[table_pos].rx_dlc_sync[obj])
	{
		return (ERR_MSG_CAN_MAIN_RX_WRONG_DLC);
	}
#endif // #ifdef LI_CAN_SLV_SYNC_CHECK_MODULE_NR_AND_DLC

#ifdef LI_CAN_SLV_MAIN_MON
	if (can_mainmon_type == CAN_MAINMON_TYPE_MAIN)
	{
		/* receive data only from master */
		/* call main RX-conversion code with RX data pointer */
		err = can_config_module_tab[table_pos].rx_main_sync[obj](can, (byte_t *) can_config_module_tab[table_pos].rx[obj], dlc);
		can_sync.main_rx_cnt[table_pos][obj]++;
#ifdef LI_CAN_SLV_DEBUG_SYNC_RX
		//LI_CAN_SLV_DEBUG_PRINT("\n\nmain_rx_cnt: %d, table: %d, obj: %d", can_sync.main_rx_cnt[table_pos][obj], table_pos, obj);
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_RX
	}
	else
	{
		if ((can_id & CAN_CONFIG_DATA_RX_MASK) == CAN_CONFIG_DATA_RX_MASK)
		{
			/* receive data from master */
			/* call main RX-conversion code with RX data pointer */

			// copy atomic
			// CAN_PORT_DISABLE_IRQ();
			err = can_config_module_tab[table_pos].rx_main_sync[obj](can, (byte_t *) can_config_module_tab[table_pos].rx[obj], dlc);
			can_sync.main_rx_cnt[table_pos][obj]++;
			// CAN_PORT_ENABLE_IRQ();
		}
		else
		{
			/* receive data from main CPU */
			/* call TX main RX monitor conversion code with TX data pointer */

			// copy atomic
			// CAN_PORT_DISABLE_IRQ();
			err = can_config_module_tab[table_pos].tx_main_rx_mon_sync[obj](can, (byte_t *) can_config_module_tab[table_pos].tx[obj], dlc);
#ifdef LI_CAN_SLV_SYNC_MAIN_TX_CNT
			can_sync.main_tx_cnt[table_pos][obj]++;
#endif // #ifdef LI_CAN_SLV_SYNC_MAIN_TX_CNT
			// CAN_PORT_ENABLE_IRQ();
		}
	}
#else // #ifdef LI_CAN_SLV_MAIN_MON

#ifdef LI_CAN_SLV_DEBUG_SYNC_MAIN
	if (table_pos == 0)
	{
		LI_CAN_SLV_DEBUG_PRINT("cmain: ");
		for (i = 0; i < dlc; i++)
		{
			LI_CAN_SLV_DEBUG_PRINT("%02X ", can[i]);
		}
		LI_CAN_SLV_DEBUG_PRINT("\n");
	}
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_MAIN

	/* call main RX-conversion code */
	err = can_config_module_tab[table_pos].rx_main_sync[obj](can, (byte_t *) can_config_module_tab[table_pos].rx[obj], dlc);
	can_sync.main_rx_cnt[table_pos][obj]++;
#endif // #ifdef LI_CAN_SLV_MAIN_MON
	return (err);
}

/**
 * @brief synchronous data transmission to the master via main CAN-controller
 * @remarks The function has to be called by the process request the of master.\n
 * The routine includes following steps:
 * 	- capture of the application data of the logical module
 * 	- call of the main TX-conversion-code for the logical module
 * 	@param table_pos is the position in the CAN configuration module table (maximum defined logical modules
 * 	are #LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES)
 * 	@param obj is the transmit object number (maximum of 4 transmit objects for each logical module)
 * 	@param dlc is the data length code of the transmit object
 * 	@param[in] can is the data pointer of the used CAN message object
 * 	@return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_sync_tx_data_main(uint16_t table_pos, uint16_t obj, uint16_t dlc, byte_t *can)
{
	li_can_slv_errorcode_t err;
#ifndef LI_CAN_SLV_MON
	can_sync_data_t can_sync_data_main_tx_tmp;
#endif // #ifdef LI_CAN_SLV_MON
#ifdef LI_CAN_SLV_DEBUG_SYNC_MAIN
	uint16_t i;
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_MAIN

#ifdef LI_CAN_SLV_MON
	word_t *dptr1 = (word_t *) &can_sync_data_main_tx[table_pos].data[obj][0];
#else // #ifdef LI_CAN_SLV_MON
	word_t *dptr1 = (word_t *) &can_sync_data_main_tx_tmp.data[obj][0];
#endif // #ifdef LI_CAN_SLV_MON
	word_t *sptr1 = (word_t *) can_config_module_tab[table_pos].tx[obj];
	word_t *dptr2;
	word_t *sptr2;
	word_t *dptr3;
	word_t *sptr3;
	word_t *dptr4;
	word_t *sptr4;

	// capture application transmit data
	dptr2 = dptr1 + 1;
	sptr2 = sptr1 + 1;
	dptr3 = dptr1 + 2;
	sptr3 = sptr1 + 2;
	dptr4 = dptr1 + 3;
	sptr4 = sptr1 + 3;

	// copy atomic
	CAN_PORT_DISABLE_IRQ();
	*dptr1 = *sptr1;
	*dptr2 = *sptr2;
	*dptr3 = *sptr3;
	*dptr4 = *sptr4;
	CAN_PORT_ENABLE_IRQ();

#ifdef LI_CAN_SLV_MON
	err = can_config_module_tab[table_pos].tx_main_sync[obj](can, &can_sync_data_main_tx[table_pos].data[obj][0], dlc);
#else // #ifdef LI_CAN_SLV_MON
	err = can_config_module_tab[table_pos].tx_main_sync[obj](can, &can_sync_data_main_tx_tmp.data[obj][0], dlc);
#endif // #ifdef LI_CAN_SLV_MON

#ifdef LI_CAN_SLV_DEBUG_SYNC_MAIN
	if (table_pos == 0)
	{
		LI_CAN_SLV_DEBUG_PRINT("tx_main: ");
		for (i = 0; i < dlc; i++)
		{
			LI_CAN_SLV_DEBUG_PRINT("%02X ", can[i]);
		}
		LI_CAN_SLV_DEBUG_PRINT("\n");
	}
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_MAIN
	return (err);
}

/**
 * @brief increment the main TX-counter of the given object number (obj)
 * @param table_pos is the position in the CAN configuration module table (maximum defined logical modules
 * are #LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES)
 * @param obj is the transmit object number (maximum of 4 transmit objects for each logical module)
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_sync_tx_data_main_ok(uint16_t table_pos, uint16_t obj)
{
#ifdef LI_CAN_SLV_SYNC_MAIN_TX_CNT
	can_sync.main_tx_cnt[table_pos][obj]++;
#else // #ifdef LI_CAN_SLV_SYNC_MAIN_TX_CNT
	table_pos = table_pos;
	obj = obj;
#endif // #ifdef LI_CAN_SLV_SYNC_MAIN_TX_CNT
	return (LI_CAN_SLV_ERR_OK);
}

#ifdef LI_CAN_SLV_MON
/**
 * @brief checks process request from master via monitor CAN-controller
 * @remarks Incoming process request from master are checked
 * 			-	of correct DLC (DLC = 0 or DLC = 1 are accepted)
 * 			-	if DLC > 0 of correct process send number
 * @param dlc is the data length code of received process request
 * @param[in] can is the data source pointer to received data
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_sync_rx_process_mon(uint16_t dlc, byte_t const *can)
{
#ifdef CAN_MON_DIAGNOSE
	can_mon_diagnose.pr++;
#endif // #ifdef CAN_MON_DIAGNOSE

	can = can; // suppress warning

	can_sync.mon_pr_time = can_port_get_system_ticks();

	can_sync.mon_pr_cnt++;
	can_sync.mon_pr_dlc = dlc;
	//	can_sync.mon_pr_index = (uint16_t) can[0];
	can_sync.mon_pr_flag = 0xFFFF;
	return (LI_CAN_SLV_ERR_OK);
}

/**
 * @brief checks the success of the synchronous data transmission from the master via monitor CAN-controller
 * @remarks if the monitor CAN-controller receives the synchronous RX-data from the master following steps are
 * executed.
 * - call of the monitor RX-conversion-code for the logical module
 * - if CAN object (message) has the correct DLC increment the monitor RX-counter of this object
 * @param table_pos is the position in the CAN configuration module table (maximum defined logical modules
 * are #LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES)
 * @param can_id is the 11bit CAN identifier of the CAN message
 * @param dlc is the data length code of the transmit object
 * @param[in] can is the data pointer of the used CAN message object
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_sync_rx_data_mon(uint16_t table_pos, uint16_t can_id, uint16_t dlc, byte_t *can)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t obj;
#ifdef LI_CAN_SLV_DEBUG_SYNC_MON
	uint16_t i;
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_MON

#ifdef LI_CAN_SLV_SYNC_CHECK_MODULE_NR_AND_DLC
	li_can_slv_module_nr_t module_nr;
#endif // #ifdef LI_CAN_SLV_SYNC_CHECK_MODULE_NR_AND_DLC

#ifndef LI_CAN_SLV_SYNC_CHECK_MODULE_NR_AND_DLC
	can = can; // dummy assignment
#endif // #ifndef LI_CAN_SLV_SYNC_CHECK_MODULE_NR_AND_DLC
#ifndef LI_CAN_SLV_MON
	dlc = dlc; // dummy assignment
#endif // #ifndef LI_CAN_SLV_MON

	obj = can_id & CAN_SYNC_OBJ_MASK;

#ifdef LI_CAN_SLV_SYNC_CHECK_MODULE_NR_AND_DLC
#ifdef LI_CAN_SLV_MAIN_MON
	if (can_mainmon_type == CAN_MAINMON_TYPE_MAIN)
	{
		module_nr = ((can_id - CAN_CONFIG_DATA_RX_MASK) >> 2) + 1;
	}
	else
	{
		module_nr = ((can_id - CAN_CONFIG_DATA_TX_MASK) >> 2) + 1;
	}
#else // #ifdef LI_CAN_SLV_MAIN_MON
	module_nr = ((can_id - CAN_CONFIG_DATA_RX_MASK) >> 2) + 1;
#endif // #ifdef LI_CAN_SLV_MAIN_MON

	if (module_nr != can_config_module_tab[table_pos].module_nr)
	{
		return (ERR_MSG_CAN_MON_RX_WRONG_MODULE_NR);
	}
	if (dlc != can_config_module_tab[table_pos].rx_dlc_sync[obj])
	{
		return (ERR_MSG_CAN_MON_RX_WRONG_DLC);
	}
#endif // #ifdef LI_CAN_SLV_SYNC_CHECK_MODULE_NR_AND_DLC

	/* call monitor RX-conversion code */
#ifdef LI_CAN_SLV_MON

#ifdef LI_CAN_SLV_DEBUG_SYNC_MON
	if (table_pos == 0)
	{
		LI_CAN_SLV_DEBUG_PRINT("cmon: ");
		for (i = 0; i < dlc; i++)
		{
			LI_CAN_SLV_DEBUG_PRINT("%02X ", can[i]);
		}
		LI_CAN_SLV_DEBUG_PRINT("\n");
	}
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_MON

	err = can_config_module_tab[table_pos].rx_mon_sync[obj](can, &can_sync_data_mon_rx[table_pos].data[obj][0], dlc);
#endif // #ifdef LI_CAN_SLV_MON
	can_sync.mon_rx_cnt[table_pos][obj]++;
	return (err);
}

/**
 * @brief checks the success of the synchronous data transmission to the master via main CAN-controller
 * @remarks The function is using the monitor CAN-controller for that. If the main CAN-controller is sending
 * the synchronous TX-data to the master, also the monitor CAN-controller will be received this messages.
 * In this case the routine has to be called for each defined logical module in special for each object number.\n
 * The routine includes the following steps:
 * - call of the monitor TX-conversion-code for the logical module defined by the module number (module_nr) and
 * the object number (obj)
 * - compare converted data with the stored application data
 * - if the data is identically increment the monitor TX-counter of the given object number (obj)
 * @param table_pos table_pos is the position in the CAN configuration module table (maximum defined logical modules
 * are #LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES)
 * @param can_id is the 11bit CAN identifier of the CAN message
 * @param dlc is the data length code of the transmit object
 * @param can is the data pointer of the used CAN message object
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_sync_tx_data_mon(uint16_t table_pos, uint16_t can_id, uint16_t dlc, byte_t *can)
{
#ifdef LI_CAN_SLV_MON
	byte_t tmp_data[CAN_DLC_MAX];
#else // #ifdef LI_CAN_SLV_MON
	can = can; // dummy assignment
#endif // #ifdef LI_CAN_SLV_MON
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
#ifdef LI_CAN_SLV_DEBUG_SYNC_MON
	uint16_t i;
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_MON

#ifdef LI_CAN_SLV_SYNC_CHECK_MODULE_NR_AND_DLC
	li_can_slv_module_nr_t module_nr;
#endif // #ifdef LI_CAN_SLV_SYNC_CHECK_MODULE_NR_AND_DLC

	uint16_t obj;

#ifndef LI_CAN_SLV_SYNC_CHECK_MODULE_NR_AND_DLC
	can_id = can_id; // dummy assignment
	dlc = dlc; // dummy assignment
	table_pos = table_pos; // dummy assignment
#endif // #ifndef LI_CAN_SLV_SYNC_CHECK_MODULE_NR_AND_DLC

#ifdef LI_CAN_SLV_MON
	obj = can_id & CAN_SYNC_OBJ_MASK;
#endif // #ifdef LI_CAN_SLV_MON

#ifdef LI_CAN_SLV_SYNC_CHECK_MODULE_NR_AND_DLC
#ifdef LI_CAN_SLV_MAIN_MON
	if (can_mainmon_type == CAN_MAINMON_TYPE_MAIN)
	{
		module_nr = ((can_id - CAN_CONFIG_DATA_TX_MASK) >> 2) + 1;
	}
	else
	{
		module_nr = ((can_id - CAN_CONFIG_DATA_RX_MASK) >> 2) + 1;
	}
#else // #ifdef LI_CAN_SLV_MAIN_MON
	module_nr = ((can_id - CAN_CONFIG_DATA_TX_MASK) >> 2) + 1;
#endif // #ifdef LI_CAN_SLV_MAIN_MON

	if (module_nr != can_config_module_tab[table_pos].module_nr)
	{
		return (ERR_MSG_CAN_MON_TX_WRONG_MODULE_NR);
	}

	if (dlc != can_config_module_tab[table_pos].tx_dlc_sync[obj])
	{
		return (ERR_MSG_CAN_MON_TX_WRONG_DLC);
	}
#endif // #ifdef LI_CAN_SLV_SYNC_CHECK_MODULE_NR_AND_DLC

#ifdef LI_CAN_SLV_DEBUG_SYNC_MON
	if (table_pos == 0)
	{
		LI_CAN_SLV_DEBUG_PRINT("rx_mon(tx_main): ");
		for (i = 0; i < dlc; i++)
		{
			LI_CAN_SLV_DEBUG_PRINT("%02X ", can[i]);
		}
		LI_CAN_SLV_DEBUG_PRINT("\n");
	}
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_MON

#ifdef LI_CAN_SLV_MON
	// call monitor TX-conversion code with capture application data
	err = can_config_module_tab[table_pos].tx_mon_sync[obj](can, &tmp_data[0], dlc);
#endif // #ifdef LI_CAN_SLV_MON

#ifdef LI_CAN_SLV_MON
	/*----------------------------------------------------------------------*/
	/* compare transmit data of main CAN controller and received by         */
	/* monitor CAN controller                                               */
	/*----------------------------------------------------------------------*/
	if (!can_port_memory_cmp(&can_sync_data_main_tx[table_pos].data[obj][0], &tmp_data[0], dlc))
	{
		can_sync.mon_tx_cnt[table_pos][obj]++;
	}
	else
	{
		return (ERR_MSG_CAN_MAIN_MON_DATA_TX);
	}

#ifdef LI_CAN_SLV_DEBUG_SYNC_MON
	if (table_pos == 0)
	{
		LI_CAN_SLV_DEBUG_PRINT("cmp1: ");
		for (i = 0; i < dlc; i++)
		{
			LI_CAN_SLV_DEBUG_PRINT("%02X ", can_sync_data_main_tx[table_pos].data[obj][i]);
		}

		LI_CAN_SLV_DEBUG_PRINT("\n");

		LI_CAN_SLV_DEBUG_PRINT("cmp2: ");
		for (i = 0; i < dlc; i++)
		{
			LI_CAN_SLV_DEBUG_PRINT("%02X ", tmp_data[i]);
		}

		LI_CAN_SLV_DEBUG_PRINT("\n");
	}
#endif // #ifdef LI_CAN_SLV_DEBUG_SYNC_MON

#endif // #ifdef LI_CAN_SLV_MON
	return (err);
}
#endif // #ifdef LI_CAN_SLV_MON

/**
 * @brief clear the synchronous receive data
 * @param table_pos module position
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_sync_clr_rx_data_main_mon(uint16_t table_pos)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t obj;
	byte_t clear_data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	if (table_pos >= LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES)
	{
		return (ERR_MSG_CAN_CONFIG_MODULE_WRONG_TABLE_POS);
	}

	/*----------------------------------------------------------------------*/
	/* clear synchronous receive data by conversion code main and (mon)     */
	/*----------------------------------------------------------------------*/
	for (obj = 0; obj < can_config_module_tab[table_pos].rx_obj; obj++)
	{
		err = can_config_module_tab[table_pos].rx_main_sync[obj](clear_data, (byte_t *) can_config_module_tab[table_pos].rx[obj], can_config_module_tab[table_pos].rx_dlc_sync[obj]);
		if (err != LI_CAN_SLV_ERR_OK)
		{
			return (err);
		}
#ifdef LI_CAN_SLV_MAIN_MON
		/**@todo warning Not implemented... */
#endif // #ifdef LI_CAN_SLV_MAIN_MON

#ifdef LI_CAN_SLV_MON
		err = can_config_module_tab[table_pos].rx_mon_sync[obj](clear_data, &can_sync_data_mon_rx[table_pos].data[obj][0], can_config_module_tab[table_pos].rx_dlc_sync[obj]);
		if (err != LI_CAN_SLV_ERR_OK)
		{
			return (err);
		}
#endif // #ifdef LI_CAN_SLV_MON
	}

	return (err);
}

/**
 * @brief The function is called at every process request on the main CAN-controller to check the process image.
 * @remarks Following checks where done for every defined logical CAN module.
 * - Check the number of process requests:\n
 * The number of received process requests on the main and the monitor CAN-controller must be only one. In case of
 * any other received number of process requests the process image is not valid. If an process request is received
 * with DLC = 1 the RX data of the main and the monitor CAN-controller must be identical. In any other case the
 * process request (image) is not valid. The DLC (data length code) of the received process request must be identically
 * on the main and monitor CAN-controller .\n
 * - RX-data: For each defined logical module the received CAN-objects must be correct:
 * 	- exactly one time successful receiving on the main CAN-controller (main RX-counter = 1)
 * 	- exactly one time successful receiving on the monitor CAN-controller (monitor RX-counter = 1)
 * 	- The DLC of the received object must be identically to the defined DLC of the logical module. Also the DLC of
 * the received object via monitor CAN-controller must be exactly the same.
 * 	- The contents of the received data via main CAN-controller and the monitor CAN-controller must be identical in
 * consideration of the main RX-conversion-code and the monitor RX-conversion-code.
 * - TX-data: For each defined logical module the transmitted CAN-objects must be correct:
 * 	- exactly one time successful sending on the main CAN-controller (main TX-counter = 1, TxOK-flag)
 * 	- exactly one time successful received on the monitor CAN-controller (monitor TX-counter = 1)
 * 	- The DLC of the transmitted object must be identically to the defined DLC of the logical module. Also the DLC
 * of the received object via the monitor CAN-controller must be exactly the same.
 * 	- The contents of the transmitted data via main CAN-controller and then received by the monitor CAN-controller
 * (loop back) must be identical in consideration of the main TX-conversion-code and the monitor TX-conversion-code.\n
 */
void li_can_slv_sync_check_process_image(void)
{
	li_can_slv_errorcode_t err;
	uint16_t process_image_valid = 1;
	uint16_t i;

#ifdef LI_CAN_SLV_MON
	// only do checks when the PA was also received on the monitor CAN controller
	if (can_sync.main_pr_flag == 0xffff && can_sync.mon_pr_flag == 0xffff)
	{
#else
	if (can_sync.main_pr_flag == 0xffff)
	{
#endif // #ifdef LI_CAN_SLV_MON
#ifdef LI_CAN_SLV_DEBUG_CAN_SYNC_CHECK_PROCESS
		LI_CAN_SLV_DEBUG_PRINT("\ncan_sync_check_process_image");
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_SYNC_CHECK_PROCESS

		// check process DLC is exactly 1
		if (can_sync.main_pr_dlc > CAN_SYNC_VALID_DLC_MAX_PROCESS_REQUEST)
		{
			goto clear_exit;
		}
#ifdef LI_CAN_SLV_MON
		if (can_sync.mon_pr_dlc > CAN_SYNC_VALID_DLC_MAX_PROCESS_REQUEST)
		{
			goto clear_exit;
		}
#endif // #ifdef LI_CAN_SLV_MON

		// check the number of process requests
		if (can_sync.main_pr_cnt != CAN_SYNC_VALID_NR_OF_PROCESS_REQUEST)
		{
			can_sync.err.main_pr_cnt++;
#ifdef LI_CAN_SLV_DEBUG_CAN_SYNC_CHECK_PROCESS_ERROR
			LI_CAN_SLV_DEBUG_PRINT("\nERR_MSG_CAN_MAIN_NR_OF_PROCESS_REQUEST: %d", can_sync.main_pr_cnt);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_SYNC_CHECK_PROCESS_ERROR
			goto clear_exit;
		}
#ifdef LI_CAN_SLV_MON
		if (can_sync.mon_pr_cnt != CAN_SYNC_VALID_NR_OF_PROCESS_REQUEST)
		{
			can_sync.err.mon_pr_cnt++;
#ifdef LI_CAN_SLV_DEBUG_CAN_SYNC_CHECK_PROCESS_ERROR
			LI_CAN_SLV_DEBUG_PRINT("\nERR_MSG_CAN_MON_NR_OF_PROCESS_REQUEST: %d", can_sync.mon_pr_cnt);
#endif // #ifdef LI_CAN_SLV_DEBUG_CAN_SYNC_CHECK_PROCESS_ERROR
			goto clear_exit;
		}
#endif // #ifdef LI_CAN_SLV_MON

		// check the process image (rx/tx data of each used logical module
		for (i = 0; i < can_config_nr_of_modules; i++)
		{
			err = li_can_slv_sync_check_process_image_module(i);
			if (err != LI_CAN_SLV_ERR_OK)
			{
				can_sync.image_valid[i] = FALSE;

				// call the process call function if the image is not valid
				if (can_sync_process_image_not_valid_cbk_table[i] != NULL)
				{
					can_sync_process_image_not_valid_cbk_table[i]();
				}

				// one or more images are not valid
				process_image_valid = 0;
			}
			else
			{
				can_sync.image_valid[i] = TRUE;
				// call the process call function if the image is valid
				if (can_sync_process_image_valid_cbk_table[i] != NULL)
				{
					can_sync_process_image_valid_cbk_table[i]();
				}
			}
		}

		// whole process image correct with all logical modules
		if (process_image_valid == 1)
		{
			can_sync.pr_time_valid = can_port_get_system_ticks();
			if (can_sync_process_call != NULL)
			{
				can_sync_process_call();
			}
		}
		else
		{
			// image not valid
			goto clear_exit;
		}
		goto clear_exit;
	}

	// leave function here
	return;

clear_exit:
	// clear process data structure for next process image
	li_can_slv_sync_clear_process_image();
}

uint32_t li_can_slv_sync_get_process_valid_time(void)
{
	return (can_sync.pr_time_valid);
}

uint32_t li_can_slv_sync_get_process_configuration_time(void)
{
	/**
	 * @todo
	 */
	return 25;
}

uint32_t li_can_slv_sync_get_process_periode(void)
{
	if (can_sync.pr_periode > LI_CAN_SLV_SYNC_PROCESS_PERIODE_MAX)
	{
		can_sync.pr_periode = LI_CAN_SLV_SYNC_PROCESS_PERIODE_MAX;
	}

	return (can_sync.pr_periode);
}

uint32_t li_can_slv_sync_get_main_process_timestamp(void)
{
	return (can_sync.main_pr_time);
}

/**
 * @brief set the can sync process time valid function
 * @param type module type
 * @param pfnc function pointer to the process time valid function
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_sync_set_process_time_valid_fnc(char_t *type, uint32_t (**pfnc)(void))
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t table_pos;

	err = can_config_module_type_valid(type, &table_pos);
	if (err != LI_CAN_SLV_ERR_OK)
	{
		return (err);
	}

	switch (table_pos)
	{
#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 1
		case 0:
			*pfnc = &can_sync_get_pr_time_valid_module1;
			break;
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 1

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 2
		case 1:
			*pfnc = &can_sync_get_pr_time_valid_module2;
			break;
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 2

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 3
		case 2:
			*pfnc = &can_sync_get_pr_time_valid_module3;
			break;
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 3

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 4
		case 3:
			*pfnc = &can_sync_get_pr_time_valid_module4;
			break;
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 4

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 5
		case 4:
			*pfnc = &can_sync_get_pr_time_valid_module5;
			break;
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 5

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 6
		case 5:
			*pfnc = &can_sync_get_pr_time_valid_module6;
			break;
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 6

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 7
		case 6:
			*pfnc = &can_sync_get_pr_time_valid_module7;
			break;
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 7

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 8
		case 7:
			*pfnc = &can_sync_get_pr_time_valid_module8;
			break;
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 8
		default:
			break;

	}
	return (err);
}

/**
 * @brief set the process call function pointer
 * @param pfnc function pointer for the process call
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_sync_set_pr_call_fnc(void (*pfnc)(void))
{
	can_sync_process_call = pfnc;
	return (LI_CAN_SLV_ERR_OK);
}

/**
 * @brief set the process call function pointer for the module type, the function is called on every valid process image
 * @param type module type
 * @param module_number module number
 * @param pfnc function pointer for the process call
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_sync_set_process_image_valid_cbk(char_t *type, li_can_slv_module_nr_t module_number, void (*pfnc)(void))
{
	li_can_slv_errorcode_t err;
	uint16_t table_pos;

	err = can_config_module_valid(type, module_number, &table_pos);
	if (err != LI_CAN_SLV_ERR_OK)
	{
		return (err);
	}

	can_sync_process_image_valid_cbk_table[table_pos] = pfnc;
	return (err);
}

/**
 * @brief set the process call function pointer for the module type, the function is called every not valid process image
 * @param type module type
 * @param module_number module number
 * @param pfnc function pointer for the process call
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_sync_set_process_image_not_valid_cbk(char_t *type, li_can_slv_module_nr_t module_number, void (*pfnc)(void))
{
	li_can_slv_errorcode_t err;
	uint16_t table_pos;

	err = can_config_module_valid(type, module_number, &table_pos);
	if (err != LI_CAN_SLV_ERR_OK)
	{
		return (err);
	}

	can_sync_process_image_not_valid_cbk_table[table_pos] = pfnc;
	return (err);
}


/**
 * @brief set the process call function pointer, which is called on every time a process req is received, for the module type
 * @param type module type
 * @param module_number module number
 * @param pfnc function pointer for the process call
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_sync_set_process_request_cbk(char_t *type, li_can_slv_module_nr_t module_number, void (*pfnc)(void))
{
	li_can_slv_errorcode_t err;
	uint16_t table_pos;

	err = can_config_module_valid(type, module_number, &table_pos);
	if (err != LI_CAN_SLV_ERR_OK)
	{
		return (err);
	}

	can_sync_process_request_rx_cbk_table[table_pos] = pfnc;
	return (err);
}

void li_can_slv_sync_trigger_process_periode(void)
{
	uint32_t dtime;
	uint16_t periode_max_ticks;

	periode_max_ticks = can_port_msec_2_ticks(LI_CAN_SLV_SYNC_PROCESS_PERIODE_MAX);

	dtime = can_port_get_system_ticks() - can_sync.main_pr_time;
	if (dtime > periode_max_ticks)
	{
		dtime = periode_max_ticks;
	}

	dtime = can_port_ticks_2_msec(dtime);

	if (dtime > can_sync.pr_periode)
	{
		can_sync.pr_periode = dtime;
	}
}

/**
 * @brief can_sync_set_first_process_request_call_fnc
 * @return	#li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_sync_set_first_process_request_call_fnc(void (*pfnc)(void))
{
	can_sync_first_process_request_call = pfnc;
	return (LI_CAN_SLV_ERR_OK);
}

/**
 * \brief li_can_slv_sync_process_request_rx
 */
void li_can_slv_sync_process_request_rx(void)
{
	uint16_t i;

	if (can_sync_first_process_detect == 0)
	{
		can_sync_first_process_detect = 1;
		if (can_sync_first_process_request_call != NULL)
		{
			can_sync.pr_periode = LI_CAN_SLV_SYNC_PROCESS_PERIODE_MAX;
			can_sync_first_process_request_call();
		}
	}

	for (i = 0; i < can_config_nr_of_modules; i++)
	{
		if (can_sync_process_request_rx_cbk_table[i] != NULL)
		{
			can_sync_process_request_rx_cbk_table[i]();
		}
	}
}

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/
/**
 * @brief clears the data structure to check the synchronous process
 */
static void li_can_slv_sync_clear_process_image(void)
{
	uint16_t i, j;

	can_sync.main_pr_cnt = 0;
	can_sync.main_pr_flag = 0;

#ifdef LI_CAN_SLV_MON
	can_sync.mon_pr_cnt = 0;
	can_sync.mon_pr_flag = 0;
#endif // #ifdef LI_CAN_SLV_MON

	for (i = 0; i < LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES; i++)
	{
		for (j = 0; j < can_config_module_tab[i].rx_obj_sync; j++)
		{
			can_sync.main_rx_cnt[i][j] = 0;
#ifdef LI_CAN_SLV_MON
			can_sync.mon_rx_cnt[i][j] = 0;
#endif // #ifdef LI_CAN_SLV_MON
		}
		for (j = 0; j < can_config_module_tab[i].tx_obj_sync; j++)
		{
#ifdef LI_CAN_SLV_SYNC_MAIN_TX_CNT
			can_sync.main_tx_cnt[i][j] = 0;
#endif // #ifdef LI_CAN_SLV_SYNC_MAIN_TX_CNT
#ifdef LI_CAN_SLV_MON
			can_sync.mon_tx_cnt[i][j] = 0;
#endif // #ifdef LI_CAN_SLV_MON
		}
	}
}

/**
 * @brief checks the process image of the logical module at the current table position.
 * @param table_pos is the position in the CAN configuration module table (maximum  defined
 * logical modules are #LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES)
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t li_can_slv_sync_check_process_image_module(uint16_t table_pos)
{
	uint16_t i;

	// check the transmit data objects
	for (i = 0; i < can_config_module_tab[table_pos].tx_obj_sync; i++)
	{
#ifdef LI_CAN_SLV_SYNC_MAIN_TX_CNT
		/**
		 * Use LI_CAN_SLV_SYNC_MAIN_TX_CNT to enable the synchronous tx counter on the main node, but
		 * this is not possible on every target. On the STM32 bxCAN this is not possible because there
		 * is no guaranteed connection between the transmitted message and the tx interrupt.
		 */
		if (can_sync.main_tx_cnt[table_pos][i] != CAN_SYNC_VALID_NR_OF_TX_DATA)
		{
			can_sync.err.main_tx_cnt[table_pos]++;
			return (ERR_MSG_CAN_MAIN_NR_OF_TX_DATA);
		}
#endif // #ifdef LI_CAN_SLV_SYNC_MAIN_TX_CNT

#ifdef LI_CAN_SLV_MON
		if (can_sync.mon_tx_cnt[table_pos][i] != CAN_SYNC_VALID_NR_OF_TX_DATA)
		{
			can_sync.err.mon_tx_cnt[table_pos]++;
			return (ERR_MSG_CAN_MON_NR_OF_TX_DATA);
		}
#endif // #ifdef LI_CAN_SLV_MON
	}

	// check the receive data objects
	for (i = 0; i < can_config_module_tab[table_pos].rx_obj_sync; i++)
	{
		if (can_sync.main_rx_cnt[table_pos][i] != CAN_SYNC_VALID_NR_OF_RX_DATA)
		{
			can_sync.err.main_rx_cnt[table_pos]++;
			return (ERR_MSG_CAN_MAIN_NR_OF_RX_DATA);
		}

#ifdef LI_CAN_SLV_MON
		if (can_sync.mon_rx_cnt[table_pos][i] != CAN_SYNC_VALID_NR_OF_RX_DATA)
		{
			can_sync.err.mon_rx_cnt[table_pos]++;
			return (ERR_MSG_CAN_MON_NR_OF_RX_DATA);
		}

		// check the receive data, compare received data of main and monitor CAN controller
		if (can_port_memory_cmp(&can_sync_data_mon_rx[table_pos].data[i][0], (byte_t *)can_config_module_tab[table_pos].rx[i], can_config_module_tab[table_pos].rx_dlc_sync[i]) != 0)
		{
			can_sync.err.data[table_pos]++;
			return (ERR_MSG_CAN_MAIN_MON_DATA_RX);
		}
#endif // #ifdef LI_CAN_SLV_MON
	}

	can_sync.pr_time_valid_module[table_pos] = can_port_get_system_ticks();
	return (LI_CAN_SLV_ERR_OK);
}

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 1
/**
 * @brief can_sync_get_pr_time_valid_module1
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static uint32_t can_sync_get_pr_time_valid_module1(void)
{
	return (can_sync.pr_time_valid_module[0]);
}
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 1

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 2
/**
 * @brief can_sync_get_pr_time_valid_module2
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static uint32_t can_sync_get_pr_time_valid_module2(void)
{
	return (can_sync.pr_time_valid_module[1]);
}
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 2

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 3
/**
 * @brief can_sync_get_pr_time_valid_module3
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static uint32_t can_sync_get_pr_time_valid_module3(void)
{
	return (can_sync.pr_time_valid_module[2]);
}
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 3

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 4
/**
 * @brief can_sync_get_pr_time_valid_module4
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static uint32_t can_sync_get_pr_time_valid_module4(void)
{
	return (can_sync.pr_time_valid_module[3]);
}
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 4

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 5
/**
 * @brief can_sync_get_pr_time_valid_module5
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static uint32_t can_sync_get_pr_time_valid_module5(void)
{
	return (can_sync.pr_time_valid_module[4]);
}
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 5

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 6
/**
 * @brief can_sync_get_pr_time_valid_module6
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static uint32_t can_sync_get_pr_time_valid_module6(void)
{
	return (can_sync.pr_time_valid_module[5]);
}
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 6

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 7
/**
 * @brief can_sync_get_pr_time_valid_module7
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static uint32_t can_sync_get_pr_time_valid_module7(void)
{
	return (can_sync.pr_time_valid_module[6]);
}
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 7

#if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 8
/**
 * @brief can_sync_get_pr_time_valid_module8
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static uint32_t can_sync_get_pr_time_valid_module8(void)
{
	return (can_sync.pr_time_valid_module[7]);
}
#endif // #if LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES >= 8

/** @} */
