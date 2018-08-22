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
 * @file io_can_quality.h
 * @brief header to IO CAN QUALITY module
 * @addtogroup can_quality
 * @{
 */

#ifndef IO_CAN_QUALITY_H
#define IO_CAN_QUALITY_H

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/core/li_can_slv.h>

#include <li_can_slv/core/io_can_types.h>
#include <li_can_slv/error/li_can_slv_error_types.h>

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
/*!
 * \brief initializes all internal variables (e.g.: set last error counters (LEC-counters) to zero)\n
 * \return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_quality_init(void);

/*!
 * \brief CAN-diagnose on main CAN-controller
 * \param obj_nr object number
 * \param interrupt_id CAN interrupt identification
 * \param can_status status of the CAN-controller
 * \return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_quality_process(uint16_t obj_nr, uint16_t interrupt_id, uint16_t can_status);

/*!
 * \brief get current quality
 * \param[out] current_quality current CAN bus quality
 * \return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_quality_current_get(uint16_t *current_quality);

/*!
 * \brief get quality
 * \param[out] quality of CAN bus since start up
 * \return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_quality_get(uint16_t *quality);

#endif // #ifndef IO_CAN_QUALITY_H
/** @} */

