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
 * @file io_can_config.h
 * @brief header to IO CAN CONFIG module
 * @addtogroup can_config
 * @{
 */

#ifndef IO_CAN_CONFIG_H_
#define IO_CAN_CONFIG_H_

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>
#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/error/li_can_slv_error_types.h>
#include <li_can_slv/config/io_can_config_types.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/
#ifndef LI_CAN_SLV_BOOT
extern uint16_t can_config_nr_of_modules;
#endif // #ifndef LI_CAN_SLV_BOOT

#ifdef LI_CAN_SLV_BOOT
extern li_can_slv_config_module_t can_config_module_tab;
#endif // #ifndef LI_CAN_SLV_BOOT

#if (defined(OUTER) || defined(OUTER_APP))
extern li_can_slv_config_module_t can_config_module_tab[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES];
#endif // #if (defined(OUTER) || defined(OUTER_APP))

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_BOOT
#ifdef LI_CAN_SLV_TUPLE
li_can_slv_errorcode_t can_config_add_module(char_t *module_type, li_can_slv_module_nr_t module_nr, tuple_id_t tuple_id_module_nr);
#else // #ifdef LI_CAN_SLV_TUPLE
li_can_slv_errorcode_t can_config_add_module(char_t *module_type, li_can_slv_module_nr_t module_nr);
#endif // #ifdef LI_CAN_SLV_TUPLE

li_can_slv_module_nr_t can_config_get_module_nr(void);
#define can_config_get_module_nr_main()	can_config_get_module_nr()
#endif // #ifdef LI_CAN_SLV_BOOT

#if defined(LI_CAN_SLV_BOOT) && defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR)
li_can_slv_errorcode_t can_config_change_module_nr(li_can_slv_module_nr_t module_nr);
#endif // #if defined(LI_CAN_SLV_BOOT) && defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR

#if (defined(OUTER) || defined(OUTER_APP)) && defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR)
li_can_slv_errorcode_t can_config_change_module_nr(uint16_t table_pos, li_can_slv_module_nr_t module_nr);
#endif // #if (defined(OUTER) || defined(OUTER_APP)) && defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR)

#ifdef LI_CAN_SLV_BOOT
li_can_slv_errorcode_t can_config_get_module_silent_awake(can_config_module_silent_t *module_silent_awake);
li_can_slv_errorcode_t can_config_set_module_silent_awake(can_config_module_silent_t  module_silent_awake);
#else // #ifndef LI_CAN_SLV_BOOT
li_can_slv_errorcode_t can_config_get_module_silent_awake(uint16_t table_pos, can_config_module_silent_t *module_silent_awake);
li_can_slv_errorcode_t can_config_set_module_silent_awake(uint16_t table_pos, can_config_module_silent_t  module_silent_awake);
#endif // #ifdef LI_CAN_SLV_BOOT

#if defined(OUTER) || defined(OUTER_APP)
li_can_slv_errorcode_t can_config_init(void);
void can_config_init_module_nr_main(li_can_slv_module_nr_t module_nr);
li_can_slv_errorcode_t can_config_get_module_type(uint16_t table_pos, char_t *module_type);
li_can_slv_errorcode_t can_config_get_module_nr(uint16_t table_pos, li_can_slv_module_nr_t *module_nr);

/**
 * @brief get the main module number
 * @return #li_can_slv_module_nr_t
 */
li_can_slv_module_nr_t can_config_get_module_nr_main(void);
void can_config_get_nr_of_modules(uint16_t *nr_of_modules);

/**
 * @brief check if the chosen module is valid
 * @param type module type
 * @param table_pos position in configuration table
 * @return #LI_CAN_SLV_ERR_OK or #ERR_MSG_CAN_CONFIG_MODULE_TYPE_NOT_FOUND
 */
li_can_slv_errorcode_t can_config_module_type_valid(char_t *type, uint16_t *table_pos);
li_can_slv_errorcode_t can_config_module_valid(char_t *type, li_can_slv_module_nr_t module_nr, uint16_t *table_pos);
#endif // #if defined(OUTER) || defined(OUTER_APP)

#if defined(OUTER) || defined(APP) || defined(OUTER_APP)
li_can_slv_errorcode_t can_config_add_module(const li_can_slv_config_module_t *module, li_can_slv_module_nr_t module_nr, void *rx0, void *rx1, void *rx2, void *rx3, void *tx0, void *tx1, void *tx2, void *tx3);

#ifdef LI_CAN_SLV_UNKNOWN_MODULE
li_can_slv_errorcode_t can_config_add_ukwn_module(li_can_slv_module_type_t *module_type, li_can_slv_module_nr_t module_nr);
#endif // #ifdef LI_CAN_SLV_UNKNOWN_MODULE

li_can_slv_errorcode_t can_config_get_module(li_can_slv_config_module_t **module, char_t *type);

/**
 * @brief switch of synchrony process of defined module
 * @param[in] type is the module type
 * @return #li_can_slv_errorcode_t or #LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_set_critical(char_t *type);
li_can_slv_errorcode_t can_config_sync_on(uint16_t table_pos);
void can_config_sync_off(uint16_t table_pos);

#ifdef LI_CAN_SLV_MODULE_IO_CONFIG
li_can_slv_errorcode_t can_config_get_ioconfig(uint16_t table_pos, can_config_ioconfig_t **ioconfig);
li_can_slv_errorcode_t can_config_set_ioconfig(char_t *type, crc16_t crc, uint32_t date);
li_can_slv_errorcode_t can_config_call_ioconfig_set(char_t *type);
#endif // #ifdef LI_CAN_SLV_MODULE_IO_CONFIG

#if defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR) && !defined(LI_CAN_SLV_BOOT)
li_can_slv_errorcode_t can_config_get_module_nr_by_type(li_can_slv_module_nr_t *modnr, char_t *type);
#endif // #if defined(LI_CAN_SLV_SYS_CHANGE_MODULE_NR) && !defined(LI_CAN_SLV_BOOT)

li_can_slv_errorcode_t can_config_set_module_silent_awake_call_fnc(void (*pfnc)(uint16_t table_pos, char_t *type, li_can_slv_module_nr_t module_nr, can_config_module_silent_t module_silent_awake));
#endif // #if defined(OUTER) || defined(APP) || defined(OUTER_APP)

li_can_slv_errorcode_t can_config_set_baudrate(can_config_bdr_t baudrate);

li_can_slv_errorcode_t can_config_set_baudrate_table(void);
li_can_slv_errorcode_t can_config_set_baudrate_default(void);
li_can_slv_errorcode_t can_config_get_baudrate(can_config_bdr_t *baudrate);
uint8_t can_config_is_baudrate_valid(can_config_bdr_t baudrate);

void can_config_save_baudrate_startup(li_can_slv_config_bdr_t baud_rate);
li_can_slv_config_bdr_t can_config_get_baudrate_startup(void);

#ifdef LI_CAN_SLV_BOOT
li_can_slv_errorcode_t can_config_module_nr_valid(li_can_slv_module_nr_t module_nr, can_config_module_silent_t *module_silent, uint16_t *module_found);
#else // #ifdef LI_CAN_SLV_BOOT
li_can_slv_errorcode_t can_config_module_nr_valid(li_can_slv_module_nr_t module_nr, uint16_t *table_pos, can_config_module_silent_t *module_silent, uint16_t *module_found);
#endif // #ifdef LI_CAN_SLV_BOOT

#ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR
li_can_slv_errorcode_t li_can_slv_config_change_module_nr_and_identifiers(char_t *module_type, li_can_slv_module_nr_t new_module_nr, li_can_slv_module_nr_t actual_module_nr);
void li_can_slv_config_set_module_nr_change_voter(lcsa_module_change_voter_state_t (*pfnc)(char_t *module_type, li_can_slv_module_nr_t new_module_nr, li_can_slv_module_nr_t act_module_nr));
li_can_slv_errorcode_t li_can_slv_config_set_module_silent_awake_from_type_and_nr(char_t *module_type, li_can_slv_module_nr_t module_nr, can_config_module_silent_t desired_state);
#endif // #ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR

#endif // #ifndef IO_CAN_CONFIG_H_
/** @} */
