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
 * @file io_app_module_change.c
 * @addtogroup app
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "io_app_module_change.h"

#include "io_app_ma_w.h"

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#define MODULE_NR_MIN	(APP_MA_W_MODULE_NR_MIN-1)
#define MODULE_NR_MAX	(APP_MA_W_MODULE_NR_MAX+1)

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
#ifdef LI_CAN_SLV_USE_SYS_CHANGE_MODULE_NUMBER
/**
 * @brief check if the module nr is in range for the module type or handle any specific things during the change of the module type
 * 			maybe storing the "new" module nr in any persistent storage
 * @param *module_type type on which the change nr command was executed
 * @param new_module_nr requested module nr to where it should be changed
 * @param act_module_nr actual module nr
 * @return lsca_module_change_voter_state_t depending on the state the CAN stack will change the number or not
 */
lcsa_module_change_voter_state_t app_module_change_vote_module_nr_change_valid(char_t *module_type, lcsa_module_number_t new_module_nr, lcsa_module_number_t act_module_nr)
{
	module_type = module_type;

	if (new_module_nr == act_module_nr)
	{
		return (LCSA_MODULE_CHANGE_VALID_BUT_SET_SILENT);
	}
	else if ((new_module_nr < MODULE_NR_MAX) && (new_module_nr > MODULE_NR_MIN))
	{
		return (LCSA_MODULE_CHANGE_VALID);
	}
	else
	{
		return (LCSA_MODULE_CHANGE_FORBIDDEN);
	}
}
#endif // #ifdef LI_CAN_SLV_USE_SYS_CHANGE_MODULE_NUMBER

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/** @} */
