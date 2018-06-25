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

#if defined( __DOXYGEN__)
#define USING
#define INTERRUPT(name, vector) static void name(void)
#define NEAR
#define FAR
#define FASTRAM
#define sdata
#define SDATA
#define huge
#define HUGE
#define XHUGE
#define INLINE
#define sbit
#endif /* #if defined( __DOXYGEN__) */

/**
 * @file io_can_document.h
 * @brief header containing module wide documentation stuff
 */

/**
 * \defgroup lcs_api API Layer
 */

/*!
 * \defgroup lcs_core core
 */

/*!
 * \defgroup lcs_types types
 */

/**
 * @defgroup lcs_core_main_types main
 * @ingroup lcs_types
 */

/**
 * @defgroup lcs_core_mon_types mon
 * @ingroup lcs_types
 */

/**
 * @defgroup lcs_config_types config
 * @ingroup lcs_types
 */

/**
 * @defgroup lcs_xload_types xload
 * @ingroup lcs_types
 */

/**
 * @defgroup lcs_error_types error
 * @ingroup lcs_types
 */

/*!
 * \defgroup can_driver_main main
 * \ingroup lcs_core
 */

/*!
 * \defgroup can_driver_mon monitor
 * \ingroup lcs_core
 */

/*!
 * \defgroup can_config configuration
 */

/**
 * \defgroup can_sys system
 */

/**
 * @defgroup can_sync synchronous
 */

/*!
 * \defgroup can_async asynchronous
 */

/**
 * @defgroup can_utils utilities
 */

/**
 * \defgroup can_reconnect reconnect
 * \ingroup can_utils
 */

/**
 * \defgroup can_quality quality
 * \ingroup can_utils
 */

/**
 * \defgroup can_analyzer analyzer
 * \ingroup can_utils
 */

/**
 * \defgroup can_assert assert
 * \ingroup can_utils
 */

/**
 * \defgroup can_memory	memory
 * \ingroup can_utils
 */

/**
 * \defgroup lcs_xload xload
 */

/**
 * \defgroup lcs_xload_buffer xload_buffer
 * \ingroup lcs_xload
 */

/**
 * \defgroup lcs_xload_info xload_info
 * \ingroup lcs_xload
 */

/**
 * \defgroup lcs_error error
 */

/**
 * \defgroup lcs_error_core error system
 * \ingroup lcs_error
 */

/**
 * \defgroup message_code error msg code
 * \ingroup lcs_error
 */

/**
 * \defgroup errno error numbers old
 * \ingroup lcs_error
 */

/**
 * \defgroup lcs_arch HW Layer
 */

/**
 * \defgroup lcs_arch_canpie_adapter canpie adapter
 * \ingroup lcs_arch
 */

/**
 * \defgroup lcs_mock stubs/mocks
 */

#ifndef IO_CAN_DOCUMENT_H_
#define IO_CAN_DOCUMENT_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/

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

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef IO_CAN_DOCUMENT_H_

