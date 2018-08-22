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

/*!
 * \brief ERROR numbering
 * \remarks Provides consistent error numbering scheme across I/O module\n
 * projects for different software components
 */

/**
 * @file io_can_errno.c
 * @brief dummy module for error number definitions
 * @addtogroup lcs_error_core
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/error/io_can_errno.h>

/*--------------------------------------------------------------------------*/
/* global (exported) variables                                              */
/*--------------------------------------------------------------------------*/

#ifdef ERROR_SUBGROUP_TEXT
#if defined(OUTER) || defined(OUTER_APP)
/*!
 * \brief	Defines the text for possible subgroups
 */
const char_t *error_can_subgroup_text[16] =
{
	"undefined",	//0
	"system",		//1
	"application",	//2
	"boot",			//3
	"os",			//4
	"can",			//5
	"drivers",		//6
	"syscheck",		//7
	"utilities",	//8
	"recorder",		//9
	"sensor",		//A
	"undefined",	//B
	"undefined",	//C
	"undefined",	//D
	"undefined",	//E
	"undefined",	//F
};
#endif // #if defined(OUTER) || defined(OUTER_APP)
#endif // #ifdef ERROR_SUBGROUP_TEXT

/*--------------------------------------------------------------------------*/
/* global (not exported) variables                                          */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* public (exported) functions                                              */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* private (not exported) functions                                         */
/*--------------------------------------------------------------------------*/
/** @} */

