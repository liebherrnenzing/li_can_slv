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
 * @file li_can_slv_config_defines.h
 * @brief header to li_can_slv_config_defines.h module
 * @addtogroup can_config
 * @{
 */

#ifndef LI_CAN_SLV_CONFIG_DEFINES_H_
#define LI_CAN_SLV_CONFIG_DEFINES_H_

#ifdef __cplusplus
extern "C" {
#endif /*  #ifdef __cplusplus */

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include "li_can_slv.h"

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
#define CAN_CONFIG_TYPE_STRING_LENGTH	5 /*!< type string length inclusive 0-character*/

#define LI_CAN_SLV_CONFIG_OBJ_USED_NOT	0 /**< */
#define LI_CAN_SLV_CONFIG_OBJ_USED_1	1 /**< */
#define LI_CAN_SLV_CONFIG_OBJ_USED_2	2 /**< */
#define LI_CAN_SLV_CONFIG_OBJ_USED_3	3 /**< */
#define LI_CAN_SLV_CONFIG_OBJ_USED_4	4 /**< */

#define LI_CAN_SLV_CONFIG_DLC_USED_0		0 /**< */
#define LI_CAN_SLV_CONFIG_DLC_USED_1		1 /**< */
#define LI_CAN_SLV_CONFIG_DLC_USED_2		2 /**< */
#define LI_CAN_SLV_CONFIG_DLC_USED_3		3 /**< */
#define LI_CAN_SLV_CONFIG_DLC_USED_4		4 /**< */
#define LI_CAN_SLV_CONFIG_DLC_USED_5		5 /**< */
#define LI_CAN_SLV_CONFIG_DLC_USED_6		6 /**< */
#define LI_CAN_SLV_CONFIG_DLC_USED_7		7 /**< */
#define LI_CAN_SLV_CONFIG_DLC_USED_8		8 /**< */

#define CAN_CONFIG_MODULE_TYPE_NOT_USED	"\0\0\0\0" /*!< module type string for not used table position */
#define CAN_CONFIG_MODULE_TYPE_UNKNOWN	"UKWN\0" /*!< module type string for unknown module type */
#define LI_CAN_SLV_CONFIG_DEF_MOD_NR1		125u /*!< default module number1 */
#define LI_CAN_SLV_CONFIG_DEF_MOD_NR2		126u /*!< default module number2 */
#define LI_CAN_SLV_CONFIG_DEF_MOD_NR3		127u /*!< default module number3 */
#define LI_CAN_SLV_CONFIG_DEF_MOD_NR4		127u /*!< default module number4 */
#define LI_CAN_SLV_CONFIG_DEF_MOD_NR5		127u /*!< default module number5 */
#define LI_CAN_SLV_CONFIG_DEF_MOD_NR6		127u /*!< default module number6 */
#define LI_CAN_SLV_CONFIG_DEF_MOD_NR7		127u /*!< default module number7 */
#define LI_CAN_SLV_CONFIG_DEF_MOD_NR8		127u /*!< default module number8 */

#define CAN_CONFIG_DIR_TX	(CAN_DIR_TX) /*!< CAN direction transmit */
#define CAN_CONFIG_DIR_RX	(CAN_DIR_RX) /*!< CAN direction receive */

#define CAN_OBJECT_IS_SYNC	1 /*!< Indicates that a given message object shall serve as synchronous data object */
#define CAN_OBJECT_NOT_SYNC	0 /*!< Indicates that a given message object is not a synchronous data object */

/* CAN baud rate definitions */
#define CAN_CONFIG_DEF_BDR	250u /*!< default baud rate */
#define LI_CAN_SLV_CONFIG_BAUD_RATE_DEFAULT	CAN_CONFIG_DEF_BDR /*!< default baud rate 250kBaud */
#define LI_CAN_SLV_CONFIG_BAUD_RATE_125K	125u /*!< 125 kBaud */
#define LI_CAN_SLV_CONFIG_BAUD_RATE_250K	250u /*!< 250 kBaud */
#define LI_CAN_SLV_CONFIG_BAUD_RATE_500K	500u /*!< 500 kBaud */
#define LI_CAN_SLV_CONFIG_BAUD_RATE_1M		1000u /*!< 1000 kBaud */

#define CAN_CONFIG_SIZE_OF_BDR_TAB 4u /*!< size of baud rate table */

#define CAN_CONFIG_NR_OF_MODULE_OBJS 4u /*!< number of module msg objects */

#define CAN_CONFIG_MIN_MODULE_NR	2u /*!< minimum valid module number */
#define CAN_CONFIG_MAX_MODULE_NR	124u /*!< maximum valid module number */

#define LI_CAN_SLV_CONFIG_TABLE_POS_MAIN_MODULE 0u

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
}/* closing brace for extern "C" */
#endif /* #ifdef __cplusplus */

#endif /* #ifndef LI_CAN_SLV_CONFIG_DEFINES_H_ */
/** @} */

