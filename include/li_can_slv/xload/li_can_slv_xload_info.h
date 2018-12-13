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
 * @file li_can_slv_xload_info.h
 * @brief header to li_can_slv_xload_info.h module
 * @addtogroup lcs_xload_info
 * @{
 */

#ifndef LI_CAN_SLV_XLOAD_INFO_H_
#define LI_CAN_SLV_XLOAD_INFO_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>
#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/error/li_can_slv_error_types.h>
#include <li_can_slv/xload/li_can_slv_xload_buffer_types.h>
#include <li_can_slv/config/io_can_config_types.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
#define LI_CAN_SLV_XLOAD_INFO_VALID			0x1000
#define LI_CAN_SLV_XLOAD_INFO_NOT_VALID		0x1001

/*!
 * \brief define the possible resets reset on a module
 */
typedef enum
{
	LI_CAN_SLV_XLOAD_INFO_RESET_POWERUP = 0, /*!< normal startup situation on module */
	LI_CAN_SLV_XLOAD_INFO_RESET_WATCHDOG = 1, /*!< reset caused by watchdog reset */
	LI_CAN_SLV_XLOAD_INFO_RESET_SOFTWARE = 2 /*!< reset caused by software reset */
} li_can_slv_xload_info_reset_reason_t;

/*!
 * \brief define the possible modes after a startup
 */
typedef enum
{
	LI_CAN_SLV_XLOAD_INFO_MODE_DOWNLOAD_REBOOT = 0, /*!< system info mode on reboot */
	LI_CAN_SLV_XLOAD_INFO_MODE_DOWNLOAD_INN = 1, /*!< system info mode go to download (Inner Core) */
	LI_CAN_SLV_XLOAD_INFO_MODE_DOWNLOAD_OUT = 2, /*!< system info mode go to download (Outer Core) */
	LI_CAN_SLV_XLOAD_INFO_MODE_DOWNLOAD_ACKN = 3, /*!< system info mode go to download end ack */
	LI_CAN_SLV_XLOAD_INFO_MODE_DOWNLOAD_RUNNING = 4, /*!< system info mode go to running */
	LI_CAN_SLV_XLOAD_INFO_MODE_DOWNLOAD_FATAL = 5 /*!< system info mode go to system fatal */
} li_can_slv_xload_info_mode_t;

/*!
 * \brief define error in sysinfo
 */
typedef struct
{
	uint32_t error_one; /*!< error one */
	uint32_t error_two; /*!< error two */
} li_can_slv_xload_info_error_t;

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/
#ifdef __GNUC__
#pragma pack(1)
#endif
typedef struct
{
	li_can_slv_xload_info_mode_t mode; /*!< mode */
#ifdef LI_CAN_SLV_XLOAD_INFO_BACKWARD_COMPATIBILITY
	li_can_slv_xload_info_reset_reason_t reset_reason; /*!< reset reason */
#endif // #ifdef LI_CAN_SLV_XLOAD_INFO_BACKWARD_COMPATIBILITY
	li_can_slv_xload_component_t dload; /*!< dload component */
	li_can_slv_config_bdr_t can_baudrate; /*!< can_baudrate 100, 250, 500 or 1000 kBaud */
#ifdef LI_CAN_SLV_XLOAD_INFO_BACKWARD_COMPATIBILITY
	li_can_slv_xload_info_error_t err; /*!< xload_info err */
#endif // #ifdef LI_CAN_SLV_XLOAD_INFO_BACKWARD_COMPATIBILITY
#ifdef LI_CAN_SLV_XLOAD_INFO_USER
	uint8_t user1;
	uint8_t user2;
#endif // #ifdef LI_CAN_SLV_XLOAD_INFO_USER
	/* far the content is CRC-protected */
	uint16_t crc; /*!< xload_info crc */
#if defined (LI_CAN_SLV_XLOAD_INFO_BACKWARD_COMPATIBILITY) && !defined (LI_CAN_SLV_XLOAD_INFO_USER)
	byte_t tmp1;
	byte_t tmp2;
#elif defined (LI_CAN_SLV_XLOAD_INFO_BACKWARD_COMPATIBILITY) && defined (LI_CAN_SLV_XLOAD_INFO_USER)
#error only use LI_CAN_SLV_XLOAD_INFO_BACKWARD_COMPATIBILITY or LI_CAN_SLV_XLOAD_INFO_USER
#endif // #if defined (LI_CAN_SLV_XLOAD_INFO_BACKWARD_COMPATIBILITY) && !defined (LI_CAN_SLV_XLOAD_INFO_USER)
} li_can_slv_xload_info_t;
#ifdef __GNUC__
#pragma pack()
#endif
/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/
#ifdef _KEIL
extern li_can_slv_xload_info_t XHUGE xload_info;
#else // #ifdef _KEIL
extern li_can_slv_xload_info_t xload_info;
#endif // #ifdef _KEIL

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t li_can_slv_xload_info_init(void);
void li_can_slv_xload_info_clean(void);

void li_can_slv_xload_info_check_crc(uint16_t *state);

void li_can_slv_xload_info_get(li_can_slv_xload_info_t **pxload_info);

void li_can_slv_xload_info_set_mode(li_can_slv_xload_info_mode_t mode);
void li_can_slv_xload_info_get_mode(li_can_slv_xload_info_mode_t *mode);

#ifdef LI_CAN_SLV_XLOAD_INFO_BACKWARD_COMPATIBILITY
void li_can_slv_xload_info_set_reset_reason(li_can_slv_xload_info_reset_reason_t reset_reason);
void li_can_slv_xload_info_get_reset_reason(li_can_slv_xload_info_reset_reason_t *reset_reason);
#endif // #ifdef LI_CAN_SLV_XLOAD_INFO_BACKWARD_COMPATIBILITY

void li_can_slv_xload_info_set_dload_component(const li_can_slv_xload_component_t *dload_component);
void li_can_slv_xload_info_get_dload_component(li_can_slv_xload_component_t *dload_component);

void li_can_slv_xload_info_get_can_baudrate(uint16_t *can_baudrate);
void li_can_slv_xload_info_set_can_baudrate(uint16_t can_baudrate);

#ifdef LI_CAN_SLV_XLOAD_INFO_USER
void li_can_slv_xload_info_set_user1(uint8_t user1_data);
void li_can_slv_xload_info_get_user1(uint8_t *user1_data);
void li_can_slv_xload_info_set_user2(uint8_t user2_data);
void li_can_slv_xload_info_get_user2(uint8_t *user2_data);
#endif // #ifdef LI_CAN_SLV_XLOAD_INFO_USER

#ifdef __cplusplus
} // closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef LI_CAN_SLV_XLOAD_INFO_H_
/** @} */

