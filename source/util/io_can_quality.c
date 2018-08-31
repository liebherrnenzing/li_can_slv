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

/*! \remarks Includes a statistically measurement of the CAN bus quality.\n
 - <b>initialization</b>\n
 With initialization all counters (LEC, message, byte) are set to be zero.

 - <b>LEC (last error code) counters</b>\n
 A CAN bus measurement is implemented for the main CAN-controller which represented\n
 a statistical value of the CAN-quality (0 to 1000 per mill).	Therefore different\n
 LEC (last error code) counters are implemented:\n
 \arg	stuff error counter
 \arg	form error counter
 \arg	acknowledge error counter
 \arg	bit 1 error counter
 \arg	bit 0 error counter
 \arg	crc error counter\n\n

 - <b>message and byte counters</b>\n
 With every received CAN message a message counter is incremented. Also the DLC of the\n
 CAN message is calculate and increments the byte counter by its value.

 - <b>quality</b>\n
 With the knowledge of LEC counters and the successfully transmitted messages a\n
 statistical CAN-quality could be calculated (0 to 1000 per mill). The quality value is\n
 an indices for the CAN-quality since the machine was starting.\n

 - <b>current quality</b>\n
 The current quality (0 to 1000 per mill) gives the quality of the CAN line at the moment.\n
 To calculate the current quality the LECs and the successfully transmitted messages are counted\n
 in a defined time (quality window). The current quality represents the CAN quality of the CAN\n
 line in the time slot.

 - <b>CAN error warning</b>\n
 A reconnect routine is starting. See therefore io_can_reconnect.

 - <b>CAN bus off</b>\n
 A reconnect (recovery sequence) routine is starting. See therefore io_can_reconnect.
 */

/**
 * @file io_can_quality.c
 * @brief
 * @addtogroup can_quality
 * @{
 */
/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/util/io_can_quality.h>
#include <li_can_slv/error/io_can_errno.h>

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#define CAN_MAIN_NR_OF_LEC	6 /**< */

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/

/**
 * @brief can_quality structure
 * @struct can_quality_t
 */
typedef struct
{
	uint32_t msg_cnt; /**< */
	uint32_t byte_cnt; /**< */
	uint32_t cnt_lec[CAN_MAIN_NR_OF_LEC]; /**< */
	/* 0...stuff       */
	/* 1...form        */
	/* 2...acknowledge */
	/* 3...bit1        */
	/* 4...bit0        */
	/* 5...crc         */
	uint16_t current_quality; /**< */
	uint16_t quality; /**< */
} can_quality_t;

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/
//static can_quality_t can_quality; /**< */

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
li_can_slv_errorcode_t can_quality_init(void)
{
	return (ERR_MSG_CAN_NOT_IMPLEMENTED);
}

li_can_slv_errorcode_t can_quality_process(uint16_t obj_nr, uint16_t interrupt_id, uint16_t can_status)
{
	obj_nr = obj_nr;
	interrupt_id = interrupt_id;
	can_status = can_status;

	return (ERR_MSG_CAN_NOT_IMPLEMENTED);
}

li_can_slv_errorcode_t can_quality_current_get(uint16_t *current_quality)
{
	current_quality = current_quality;
	return (ERR_MSG_CAN_NOT_IMPLEMENTED);
}

li_can_slv_errorcode_t can_quality_get(uint16_t *quality)
{
	quality = quality;
	return (ERR_MSG_CAN_NOT_IMPLEMENTED);
}

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/
/** @} */

