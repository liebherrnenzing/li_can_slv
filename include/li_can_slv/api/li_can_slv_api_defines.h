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
 * @file li_can_slv_api_defines.h
 * @brief header to li_can_slv_api_defines.h module
 * @addtogroup lcs_api
 * @{
 */

#ifndef LI_CAN_SLV_API_DEFINES_H_
#define LI_CAN_SLV_API_DEFINES_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_defines.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
/**
 * @defgroup li_can_slv_api_defines_default_module_nr Default Module Number Defines
 * @addtogroup li_can_slv_api_defines_default_module_nr
 * @{
 */
#define LCSA_DEF_MODULE_NR1		(LI_CAN_SLV_CONFIG_DEF_MOD_NR1) /**< default module for can module 1 */
#define LCSA_DEF_MODULE_NR2		(LI_CAN_SLV_CONFIG_DEF_MOD_NR2) /**< default module for can module 2 */
#define LCSA_DEF_MODULE_NR3		(LI_CAN_SLV_CONFIG_DEF_MOD_NR3) /**< default module for can module 3 */
#define LCSA_DEF_MODULE_NR4		(LI_CAN_SLV_CONFIG_DEF_MOD_NR4) /**< default module for can module 4 */
#define LCSA_DEF_MODULE_NR5		(LI_CAN_SLV_CONFIG_DEF_MOD_NR5) /**< default module for can module 5 */
#define LCSA_DEF_MODULE_NR6		(LI_CAN_SLV_CONFIG_DEF_MOD_NR6) /**< default module for can module 6 */
#define LCSA_DEF_MODULE_NR7		(LI_CAN_SLV_CONFIG_DEF_MOD_NR7) /**< default module for can module 7 */
#define LCSA_DEF_MODULE_NR8		(LI_CAN_SLV_CONFIG_DEF_MOD_NR8) /**< default module for can module 8 */
/**
 * @}
 */

/**
 * @defgroup li_can_slv_api_defines_config Config Defines
 * @addtogroup li_can_slv_api_defines_config
 * @{
 */
#define LCSA_CONFIG_OBJ_USED_NOT	LI_CAN_SLV_CONFIG_OBJ_USED_NOT /**< message objects not used  */
#define LCSA_CONFIG_OBJ_USED_0		LI_CAN_SLV_CONFIG_OBJ_USED_NOT /**< number of message objects used 0 */
#define LCSA_CONFIG_OBJ_USED_1		LI_CAN_SLV_CONFIG_OBJ_USED_1 /**< number of message objects used 1 */
#define LCSA_CONFIG_OBJ_USED_2		LI_CAN_SLV_CONFIG_OBJ_USED_2 /**< number of message objects used 2 */
#define LCSA_CONFIG_OBJ_USED_3		LI_CAN_SLV_CONFIG_OBJ_USED_3 /**< number of message objects used 3 */
#define LCSA_CONFIG_OBJ_USED_4		LI_CAN_SLV_CONFIG_OBJ_USED_4 /**< number of message objects used 4 */

#define LCSA_CONFIG_DLC_USED_0	LI_CAN_SLV_CONFIG_DLC_USED_0 /**< used data length 0 */
#define LCSA_CONFIG_DLC_USED_1	LI_CAN_SLV_CONFIG_DLC_USED_1 /**< used data length 1 */
#define LCSA_CONFIG_DLC_USED_2	LI_CAN_SLV_CONFIG_DLC_USED_2 /**< used data length 2 */
#define LCSA_CONFIG_DLC_USED_3	LI_CAN_SLV_CONFIG_DLC_USED_3 /**< used data length 3 */
#define LCSA_CONFIG_DLC_USED_4	LI_CAN_SLV_CONFIG_DLC_USED_4 /**< used data length 4 */
#define LCSA_CONFIG_DLC_USED_5	LI_CAN_SLV_CONFIG_DLC_USED_5 /**< used data length 5 */
#define LCSA_CONFIG_DLC_USED_6	LI_CAN_SLV_CONFIG_DLC_USED_6 /**< used data length 6 */
#define LCSA_CONFIG_DLC_USED_7	LI_CAN_SLV_CONFIG_DLC_USED_7 /**< used data length 7 */
#define LCSA_CONFIG_DLC_USED_8	LI_CAN_SLV_CONFIG_DLC_USED_8 /**< used data length 8 */

#define LCSA_CONFIG_MODULE_TYPE_LENGTH	CAN_CONFIG_TYPE_STRING_LENGTH /**< max module type length */

/**
 * @}
 */

/**
 * @defgroup li_can_slv_api_defines_baudrate Baud Rate Defines
 * @addtogroup li_can_slv_api_defines_baudrate
 * @{
 */
#define LCSA_BAUD_RATE_DEFAULT	(LI_CAN_SLV_CONFIG_BAUD_RATE_DEFAULT) /**< default baud rate (250kBaud) */
#define LCSA_BAUD_RATE_125K		(LI_CAN_SLV_CONFIG_BAUD_RATE_125K) /*!< 125 kBaud */
#define LCSA_BAUD_RATE_250K		(LI_CAN_SLV_CONFIG_BAUD_RATE_250K) /*!< 250 kBaud */
#define LCSA_BAUD_RATE_500k		(LI_CAN_SLV_CONFIG_BAUD_RATE_500K) /*!< 500 kBaud */
#define LCSA_BAUD_RATE_1M		(LI_CAN_SLV_CONFIG_BAUD_RATE_1M) /*!< 1000 kBaud */
/**
 * @}
 */

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
} // closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef LI_CAN_SLV_API_DEFINES_H_

/** @} */
