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
 * @file io_app_convc.h
 * @addtogroup app_convc
 * @{
 */

#ifndef IO_APP_CONVC_H_
#define IO_APP_CONVC_H_

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
#if (!defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)) && (!defined(APP_PLATFORM_BYTE_ORDER_BIG_ENDIAN))
#define APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN
#warning no platform byte order is defined... so APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN is used
#endif // #if (!defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN) || !defined(APP_PLATFORM_BYTE_ORDER_BIG_ENDIAN))

/* see bitrate.h or ecrypt-portable.h on www */
#define APP_CONV_U16C(v) (v##U)
#define APP_CONV_U16V(v) ((uint16_t)(v) & APP_CONV_U16C(0xFFFF))

#ifndef APP_CONV_ROTL16
#define APP_CONV_ROTL16(v, n) (APP_CONV_U16V((v) << (n)) | ((v) >> (16 - (n))))
#endif // #ifndef APP_CONV_ROTL16

#define APP_CONV_U32C(v) (v##U)
#define APP_CONV_U32V(v) ((uint32_t)(v) & APP_CONV_U32C(0xFFFFFFFF))

#ifndef APP_CONV_ROTL32
#define APP_CONV_ROTL32(v, n) (APP_CONV_U32V((v) << (n)) | ((v) >> (32 - (n))))
#endif // #ifdef APP_CONV_ROTL32

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
/* conversion codes transmit */
lcsa_errorcode_t app_convc_tx_bit(byte_t *can, void *app, uint16_t dlc);
lcsa_errorcode_t app_convc_tx_nibble(byte_t *can, void *app, uint16_t dlc);
lcsa_errorcode_t app_convc_tx_uint16(byte_t *can, void *app, uint16_t dlc);
lcsa_errorcode_t app_convc_tx_uint32(byte_t *can, void *app, uint16_t dlc);
#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
lcsa_errorcode_t app_convc_main_tx_uint12_msg_obj_1(byte_t *can, void *app, uint16_t dlc);
lcsa_errorcode_t app_convc_main_tx_uint12_msg_obj_2(byte_t *can, void *app, uint16_t dlc);
lcsa_errorcode_t app_convc_main_tx_uint12_msg_obj_3(byte_t *can, void *app, uint16_t dlc);
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)

/* conversion codes receive of main CAN-controller */
lcsa_errorcode_t app_convc_main_rx_bit(byte_t *can, void *app, uint16_t dlc);
lcsa_errorcode_t app_convc_main_rx_nibble(byte_t *can, void *app, uint16_t dlc);
lcsa_errorcode_t app_convc_main_rx_uint16(byte_t *can, void *app, uint16_t dlc);
lcsa_errorcode_t app_convc_main_rx_uint32(byte_t *can, void *app, uint16_t dlc);
#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
lcsa_errorcode_t app_convc_main_rx_uint12_msg_obj_1(byte_t *can, void *app, uint16_t dlc);
lcsa_errorcode_t app_convc_main_rx_uint12_msg_obj_2(byte_t *can, void *app, uint16_t dlc);
lcsa_errorcode_t app_convc_main_rx_uint12_msg_obj_3(byte_t *can, void *app, uint16_t dlc);
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)

/* conversion codes receive of monitor CAN-controller */
lcsa_errorcode_t app_convc_mon_rx_bit(byte_t *can, void *app, uint16_t dlc);
lcsa_errorcode_t app_convc_mon_rx_nibble(byte_t *can, void *app, uint16_t dlc);
lcsa_errorcode_t app_convc_mon_rx_uint16(byte_t *can, void *app, uint16_t dlc);
lcsa_errorcode_t app_convc_mon_rx_uint32(byte_t *can, void *app, uint16_t dlc);

/* application conversion codes for data reductions */
#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
lcsa_errorcode_t app_convc_sint32_to_sint20(byte_t *can, void *app);
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)

#ifdef __cplusplus
} // closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_APP_CONVC_H_

/** @} */
