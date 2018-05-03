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
 * @file li_can_slv_core_defines.h
 * @brief header to li_can_slv_core_defines.h module
 * @addtogroup lcs_core
 * @{
 */

#ifndef LI_CAN_SLV_CORE_DEFINES_H_
#define LI_CAN_SLV_CORE_DEFINES_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include "li_can_slv.h"

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
/**
 * \note Highest allowed CAN identifier acc. to CAN spec is 2031, but for async ctrl \n
 * identifier of module numbers 125 - 128 the higher IDs are required
 */
#define CAN_IDENTIFIER_MAX	0x7FF /*!< maximum value of CAN identifier */
#define CAN_DLC_MIN			0u /*!< minimum value of CAN DLC */
#define CAN_DLC_MAX			8u /*!< maximum value of CAN DLC */

#define CAN_DIR_TX			1u /*!< CAN direction transmit */
#define CAN_DIR_RX			0u /*!< CAN direction receive */

#define CAN_MSG_OBJ0		0u /*!< CAN message object 0 number */
#define CAN_MSG_OBJ1		1u /*!< CAN message object 1 number */
#define CAN_MSG_OBJ2		2u /*!< CAN message object 2 number */
#define CAN_MSG_OBJ3		3u /*!< CAN message object 3 number */
#define CAN_MSG_OBJ4		4u /*!< CAN message object 4 number */
#define CAN_MSG_OBJ5		5u /*!< CAN message object 5 number */
#define CAN_MSG_OBJ6		6u /*!< CAN message object 6 number */
#define CAN_MSG_OBJ7		7u /*!< CAN message object 7 number */
#define CAN_MSG_OBJ8		8u /*!< CAN message object 8 number */
#define CAN_MSG_OBJ9		9u /*!< CAN message object 9 number */
#define CAN_MSG_OBJ10		10u /*!< CAN message object 10 number */
#define CAN_MSG_OBJ11		11u /*!< CAN message object 11 number */
#define CAN_MSG_OBJ12		12u /*!< CAN message object 12 number */
#define CAN_MSG_OBJ13		13u /*!< CAN message object 13 number */
#define CAN_MSG_OBJ14		14u /*!< CAN message object 14 number */
#define CAN_MSG_OBJ15		15u /*!< CAN message object 15 number */
#define CAN_MSG_OBJ16		16u /*!< CAN message object 16 number */
#define CAN_MSG_OBJ17		17u /*!< CAN message object 17 number */
#define CAN_MSG_OBJ18		18u /*!< CAN message object 18 number */
#define CAN_MSG_OBJ19		19u /*!< CAN message object 19 number */
#define CAN_MSG_OBJ20		20u /*!< CAN message object 20 number */
#define CAN_MSG_OBJ21		21u /*!< CAN message object 21 number */
#define CAN_MSG_OBJ22		22u /*!< CAN message object 22 number */
#define CAN_MSG_OBJ23		23u /*!< CAN message object 23 number */
#define CAN_MSG_OBJ24		24u /*!< CAN message object 24 number */
#define CAN_MSG_OBJ25		25u /*!< CAN message object 25 number */
#define CAN_MSG_OBJ26		26u /*!< CAN message object 26 number */
#define CAN_MSG_OBJ27		27u /*!< CAN message object 27 number */
#define CAN_MSG_OBJ28		28u /*!< CAN message object 28 number */
#define CAN_MSG_OBJ29		29u /*!< CAN message object 29 number */
#define CAN_MSG_OBJ30		30u /*!< CAN message object 30 number */
#define CAN_MSG_OBJ31		31u /*!< CAN message object 31 number */
#define CAN_MSG_OBJ32		32u /*!< CAN message object 32 number */
#define CAN_MSG_OBJ33		33u /*!< CAN message object 33 number */
#define CAN_MSG_OBJ34		34u /*!< CAN message object 34 number */
#define CAN_MSG_OBJ35		35u /*!< CAN message object 35 number */
#define CAN_MSG_OBJ36		36u /*!< CAN message object 36 number */
#define CAN_MSG_OBJ37		37u /*!< CAN message object 37 number */
#define CAN_MSG_OBJ38		38u /*!< CAN message object 38 number */
#define CAN_MSG_OBJ39		39u /*!< CAN message object 39 number */
#define CAN_MSG_OBJ40		40u /*!< CAN message object 40 number */
#define CAN_MSG_OBJ41		41u /*!< CAN message object 41 number */
#define CAN_MSG_OBJ42		42u /*!< CAN message object 42 number */
#define CAN_MSG_OBJ43		43u /*!< CAN message object 43 number */
#define CAN_MSG_OBJ44		44u /*!< CAN message object 44 number */
#define CAN_MSG_OBJ45		45u /*!< CAN message object 45 number */
#define CAN_MSG_OBJ46		46u /*!< CAN message object 46 number */
#define CAN_MSG_OBJ47		47u /*!< CAN message object 47 number */
#define CAN_MSG_OBJ48		48u /*!< CAN message object 48 number */
#define CAN_MSG_OBJ49		49u /*!< CAN message object 49 number */
#define CAN_MSG_OBJ50		50u /*!< CAN message object 50 number */
#define CAN_MSG_OBJ51		51u /*!< CAN message object 51 number */
#define CAN_MSG_OBJ52		52u /*!< CAN message object 52 number */
#define CAN_MSG_OBJ53		53u /*!< CAN message object 53 number */
#define CAN_MSG_OBJ54		54u /*!< CAN message object 54 number */
#define CAN_MSG_OBJ55		55u /*!< CAN message object 55 number */
#define CAN_MSG_OBJ56		56u /*!< CAN message object 56 number */
#define CAN_MSG_OBJ57		57u /*!< CAN message object 57 number */
#define CAN_MSG_OBJ58		58u /*!< CAN message object 58 number */
#define CAN_MSG_OBJ59		59u /*!< CAN message object 59 number */
#define CAN_MSG_OBJ60		60u /*!< CAN message object 60 number */
#define CAN_MSG_OBJ61		61u /*!< CAN message object 61 number */
#define CAN_MSG_OBJ62		62u /*!< CAN message object 62 number */
#define CAN_MSG_OBJ63		63u /*!< CAN message object 63 number */

/**
 * @todo move to reconnect maybe
 */
#define CAN_INTID_NO_INTERRUPT_PENDING	0 /*!< interrupt pending */
#define CAN_INTID_LEC_IE_TXOK_RXOK		1 /*!< lec or txok or rxok */
//#define CAN_INTID_REQUEST				-1 /*!< request */
#define CAN_INTID_MSG					2 /*!< successful message */

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
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef LI_CAN_SLV_CORE_DEFINES_H_

/** @} */
