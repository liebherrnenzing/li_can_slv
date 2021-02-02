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
 * @file canpie_adapter/io_can_hw.c
 * @addtogroup lcs_arch_canpie_adapter
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "io_can_hw.h"
#include <li_can_slv/core/io_can_main_types.h>
#include <li_can_slv/error/io_can_errno.h>

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
li_can_slv_errorcode_t can_hw_init(void)
{
	return (LI_CAN_SLV_ERR_OK);
}

#ifdef LI_CAN_SLV_MAIN_MON
can_mainmon_type_t can_hw_get_mainmon_type(void)
{
	return (can_mainmon_type_t) can_port_get_mainmon_type();
}
#endif // #ifdef LI_CAN_SLV_MAIN_MON

#ifdef LI_CAN_SLV_SYS_OBJ2
#if defined(OUTER) || defined(OUTER_APP)
li_can_slv_errorcode_t can_hw_combine_msg_obj_to_two_stage_fifo(uint16_t msg_obj, uint16_t msg_obj2)
{
	/**
	 * @todo is this needed in the future
	 */
	msg_obj = msg_obj;
	msg_obj2 = msg_obj2;
	return (LI_CAN_SLV_ERR_OK);
}
#endif // #ifdef LI_CAN_SLV_SYS_OBJ2
#endif // #if defined(OUTER) || defined(OUTER_APP)

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/** @} */
