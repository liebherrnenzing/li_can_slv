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
 * @file li_can_slv_config_internal.h
 * @brief header to li_can_slv_config_internal.h module
 */

#ifndef LI_CAN_SLV_CONFIG_INTERNAL_H_
#define LI_CAN_SLV_CONFIG_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include "li_can_slv_config.h"
#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_config_debug.h"
#endif // #ifdef LI_CAN_SLV_DEBUG

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
/**
 * @defgroup li_can_slv_defines_a General Defines
 * @addtogroup li_can_slv_defines_a
 * @{
 */
#ifdef __DOXYGEN__
/**
 * This define should be used if you want to configure everything by yourself.
 */
#define LI_CAN_SLV_GENERIC
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_GENERIC
#define LI_CAN_SLV
#endif
#endif // #ifdef __DOXYGEN__

#ifdef __DOXYGEN__
/**
 * With this define the system alarms and system commands activated by #LI_CAN_SLV_USE_SYS and the
 * synchronous process handling activated by #LI_CAN_SLV_USE_SYNC are set.
 */
#define LI_CAN_SLV_BASIC
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_BASIC
#define LI_CAN_SLV
#define LI_CAN_SLV_USE_SYS
#define LI_CAN_SLV_USE_SYNC
#define OUTER_APP
#endif
#endif // #ifdef __DOXYGEN__

#ifdef __DOXYGEN__
/**
 * With this define every feature except the upload is activated. So this means
 * the following defines are set: #LI_CAN_SLV_USE_SYS, #LI_CAN_SLV_USE_SYNC,
 * #LI_CAN_SLV_USE_ASYNC, #LI_CAN_SLV_USE_ASYNC_DOWNLOAD and #LI_CAN_SLV_USE_ASYNC_TUNNEL.
 * If the upload is also needed add the define #LI_CAN_SLV_USE_ASYNC_UPLOAD
 */
#define LI_CAN_SLV_FULL
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_FULL
#define LI_CAN_SLV
#define LI_CAN_SLV_USE_SYS
#define LI_CAN_SLV_USE_SYNC
#define LI_CAN_SLV_USE_ASYNC
#define LI_CAN_SLV_USE_ASYNC_DOWNLOAD
#define LI_CAN_SLV_USE_ASYNC_TUNNEL
#define OUTER_APP
#endif
#endif // #ifdef __DOXYGEN__

/**
 * @todo add documentaion for LI_CAN_SLV_BOOT
 */

/**
 * @}
 */

/**
 * @defgroup li_can_slv_defines_b General Defines
 * @addtogroup li_can_slv_defines_b
 * @{
 */
#ifdef __DOXYGEN__
/**
 * Defines the maximum number of the usable logical modules. The current implementation is
 * able to handle 8 logical modules at a time but the real maximum depends on the current
 * CAN hardware and number of message objects used by the activated modules. Default value
 * if not defined is 1.
 */
#define LI_CAN_SLV_SET_MAX_NR_OF_LOGICAL_MODULES
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_SET_MAX_NR_OF_LOGICAL_MODULES
#define LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES (LI_CAN_SLV_SET_MAX_NR_OF_LOGICAL_MODULES)
#endif
#endif // #ifdef __DOXYGEN__
/**
 * @}
 */

/**
 * @defgroup li_can_slv_defines_b General Defines
 * @addtogroup li_can_slv_defines_b
 * @{
 */
#ifdef __DOXYGEN__
/**
 * Use this define to add a UKWN Module if no other module was added.
 */
#define LI_CAN_SLV_USE_UNKNOWN_MODULE
#define LI_CAN_SLV_UNKNOWN_MODULE
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_USE_UNKNOWN_MODULE
#define LI_CAN_SLV_UNKNOWN_MODULE
#endif
#endif // #ifdef __DOXYGEN__
/**
 * @}
 */

/**
 * @defgroup li_can_slv_defines_b General Defines
 * @addtogroup li_can_slv_defines_b
 * @{
 */
#ifdef __DOXYGEN__
/**
 \rst
  .. attention::
     Use this define to disable the xload_info shared memory but then so some of the
     internal features like sending download end acknowledge or change baud rate and
     others must be handled outside. Take a look at the lcsa_init() function for a
     better understanding.
 \endrst
 */
#define LI_CAN_SLV_DO_NOT_USE_XLOAD_INFO

#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_DO_NOT_USE_XLOAD_INFO
#define LI_CAN_SLV_NO_XLOAD_INFO
#endif
#endif // #ifdef __DOXYGEN__
/**
 * @}
 */

/**
 * @defgroup li_can_slv_defines_mainmon Defines for Main Monitor usage
 * @addtogroup li_can_slv_defines_mainmon
 * @{
 */
#ifdef __DOXYGEN__
/**
 * Use this define to enable the LCS-Stack for Main/Monitor Systems.
 */
#define LI_CAN_SLV_USE_MAIN_MON
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_USE_MAIN_MON
#define LI_CAN_SLV_MAIN_MON
#endif
#endif // #ifdef __DOXYGEN__
/**
 * @}
 */

/**
 * @defgroup li_can_slv_defines_reconnect Defines for Reconnect(Autobaud)
 * @addtogroup li_can_slv_defines_reconnect
 * @{
 */
#ifdef __DOXYGEN__
/**
 * Use the reconnect utility on the CAN Main Node.
 */
#define LI_CAN_SLV_USE_RECONNECT
#define LI_CAN_SLV_USE_RECONNECT_ON_MAIN_NODE
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_USE_RECONNECT_ON_MAIN_NODE
#define LI_CAN_SLV_USE_RECONNECT
#define LI_CAN_SLV_RECONNECT
#define LI_CAN_SLV_RECONNECT_MAIN_NODE
#endif
#endif // #ifdef __DOXYGEN__

#ifdef __DOXYGEN__
/**
 * Use the reconnect callback on baudrate change.
 */
#define LI_CAN_SLV_USE_RECONNECT_CHANGE_BAUDRATE_CALLBACK
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_USE_RECONNECT_CHANGE_BAUDRATE_CALLBACK
#define LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK
#define LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
#endif
#endif // #ifdef __DOXYGEN__
/**
 * @}
 */

/**
 * @defgroup li_can_slv_defines_system Defines for the system protocol definitions
 * @addtogroup li_can_slv_defines_system
 * @{
 */
#ifdef __DOXYGEN__
/**
 * Use the system message protocol handling.
 */
#define LI_CAN_SLV_USE_SYS
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_USE_SYS
#define LI_CAN_SLV_SYS
#endif
#endif // #ifdef __DOXYGEN__

#ifdef __DOXYGEN__
/**
 * This define is used to enable the error message system with 16 Bit error codes.
 */
#define LI_CAN_SLV_USE_SYS_ERROR_16_BIT
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_USE_SYS_ERROR_16_BIT
#define LI_CAN_SLV_SYS_MODULE_ERROR
#define LI_CAN_SLV_SYS_MODULE_ERROR_16_BIT
//#define LI_CAN_SLV_SYS_MODULE_ERROR_QUEUE
//#define LI_CAN_SLV_SYS_MODULE_ERROR_QUEUE_SIZE
#endif
#endif // #ifdef __DOXYGEN__

#ifdef __DOXYGEN__
/**
 * Use the change module number feature.
 */
#define LI_CAN_SLV_USE_SYS_CHANGE_MODULE_NUMBER
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_USE_SYS_CHANGE_MODULE_NUMBER
#define LI_CAN_SLV_SYS_CHANGE_MODULE_NR
#endif
#endif // #ifdef __DOXYGEN__

#ifdef __DOXYGEN__
/**
 * Use the factory reset feature.
 */
#define LI_CAN_SLV_USE_SYS_FACTORY_RESET_CALLBACK
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_USE_SYS_FACTORY_RESET_CALLBACK
#define LI_CAN_SLV_SYS_FACTORY_RESET_CBK
#endif
#endif // #ifdef __DOXYGEN__

/**
 * @}
 */

/**
 * @defgroup li_can_slv_defines_synchrony Defines for the synchrony protocol definitions
 * @addtogroup li_can_slv_defines_synchrony
 * @{
 */
#ifdef __DOXYGEN__
/**
 * Use the synchronous protocol handling.
 */
#define LI_CAN_SLV_USE_SYNC
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_USE_SYNC
#define LI_CAN_SLV_SYNC
#endif
#endif // #ifdef __DOXYGEN__

#ifdef __DOXYGEN__
/**
 \rst
 Set the maximum process period time in milliseconds, the default value is set to 100 milliseconds.

 .. attention::
     More than 200 milliseconds is not recommended and will cause a build warning.

 \endrst
*/
#define LI_CAN_SLV_SET_SYNC_PROCESS_PERIODE_MAX
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_SET_SYNC_PROCESS_PERIODE_MAX
#define LI_CAN_SLV_SYNC_PROCESS_PERIODE_MAX	(LI_CAN_SLV_SET_SYNC_PROCESS_PERIODE_MAX)
#endif
#endif // #ifdef __DOXYGEN__
/**
 * @}
 */

/**
 * @defgroup li_can_slv_defines_asynchrony Defines for the asynchrony protocol definitions
 * @addtogroup li_can_slv_defines_asynchrony
 * @{
 */
#ifdef __DOXYGEN__
/**
 * Use the asynchrony protocol handling.
 */
#define LI_CAN_SLV_USE_ASYNC
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_USE_ASYNC
#define LI_CAN_SLV_ASYNC
//#define LI_CAN_SLV_ASYNC_TEST_MESSAGE
#endif
#endif // #ifdef __DOXYGEN__

#ifdef __DOXYGEN__
/**
 * Use the asynchrony protocol download handling.
 */
#define LI_CAN_SLV_USE_ASYNC_DOWNLOAD
#define LI_CAN_SLV_USE_ASYNC_DOWNLOAD_BUFFER_EXTERNAL
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_USE_ASYNC_DOWNLOAD
#define LI_CAN_SLV_DLOAD
#ifndef LI_CAN_SLV_USE_ASYNC_DOWNLOAD_BUFFER_EXTERNAL
#define LI_CAN_SLV_DLOAD_BUFFER_INTERNAL
#endif
#endif
#endif // #ifdef __DOXYGEN__

#ifdef __DOXYGEN__
/**
 * Use the asynchrony protocol upload handling.
 */
#define LI_CAN_SLV_USE_ASYNC_UPLOAD

#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_USE_ASYNC_UPLOAD
#define LI_CAN_SLV_ULOAD
#endif
#endif // #ifdef __DOXYGEN__

#ifdef __DOXYGEN__
/**
 * Use the asynchrony protocol tunnel handling.
 */
#define LI_CAN_SLV_USE_ASYNC_TUNNEL
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_USE_ASYNC_TUNNEL
#define LI_CAN_SLV_ASYNC_TUNNEL
#endif
#endif // #ifdef __DOXYGEN__
/**
 * @}
 */

/**
 * @defgroup li_can_slv_defines_arch Defines to select the current architecture layer
 * @addtogroup li_can_slv_defines_arch
 * @{
 */
#ifdef __DOXYGEN__
/**
 * Set byte order to #LI_CAN_SLV_ARCH_SET_BYTE_ORDER_LITTLE_ENDIAN or #LI_CAN_SLV_ARCH_SET_BYTE_ORDER_BIG_ENDIAN.
 \rst
 .. attention::
     The default setting is LI_CAN_SLV_ARCH_SET_BYTE_ORDER_LITTLE_ENDIAN.
 \endrst
*/
#define LI_CAN_SLV_ARCH_SET_BYTE_ORDER_LITTLE_ENDIAN
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_ARCH_SET_BYTE_ORDER_LITTLE_ENDIAN
#define LI_CAN_SLV_BYTE_ORDER_LITTLE_ENDIAN
#endif
#endif // #ifdef __DOXYGEN__

#ifdef __DOXYGEN__
/**
 * Set byte order see also #LI_CAN_SLV_ARCH_SET_BYTE_ORDER_LITTLE_ENDIAN.
 */
#define LI_CAN_SLV_ARCH_SET_BYTE_ORDER_BIG_ENDIAN
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_ARCH_SET_BYTE_ORDER_BIG_ENDIAN
#define LI_CAN_SLV_BYTE_ORDER_BIG_ENDIAN
#endif
#endif // #ifdef __DOXYGEN__

#ifdef __DOXYGEN__
/**
 * Use the the CANPIE channel 1 HW interface for the can main node.
 */
#define LI_CAN_SLV_ARCH_USE_CANPIE_CH1_FOR_MAIN_NODE
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_ARCH_USE_CANPIE_CH1_FOR_MAIN_NODE
#include "canpie.h"
#if CP_VERSION_MAJOR <= 2
#define LI_CAN_SLV_MAIN_ARCH	CP_CHANNEL_1
#else
#define LI_CAN_SLV_MAIN_ARCH	eCP_CHANNEL_1
#endif
#endif // #ifdef LI_CAN_SLV_ARCH_USE_CANPIE_CH1_FOR_MAIN_NODE
#endif // #ifdef __DOXYGEN__

#ifdef __DOXYGEN__
/**
 * Use the the CANPIE channel 2 HW interface for the can monn node.
 */
#define LI_CAN_SLV_ARCH_USE_CANPIE_CH2_FOR_MAIN_NODE
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_ARCH_USE_CANPIE_CH2_FOR_MAIN_NODE
#include "canpie.h"
#if CP_VERSION_MAJOR <= 2
#define LI_CAN_SLV_MON_ARCH	CP_CHANNEL_2
#else
#define LI_CAN_SLV_MON_ARCH	eCP_CHANNEL_2
#endif
#endif // #ifdef LI_CAN_SLV_ARCH_USE_CANPIE_CH2_FOR_MAIN_NODE
#endif // #ifdef __DOXYGEN__

#ifdef __DOXYGEN__
/**
 * Use the the CANPIE channel 2 HW interface for the can monn node.
 */
#define LI_CAN_SLV_ARCH_USE_CANPIE_CH2_FOR_MON_NODE
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_ARCH_USE_CANPIE_CH2_FOR_MON_NODE
#include "canpie.h"
#if CP_VERSION_MAJOR <= 2
#define LI_CAN_SLV_MON_ARCH	CP_CHANNEL_2
#else
#define LI_CAN_SLV_MON_ARCH	eCP_CHANNEL_2
#endif
#endif // #ifdef LI_CAN_SLV_ARCH_USE_CANPIE_CH2_FOR_MON_NODE
#endif // #ifdef __DOXYGEN__

#ifdef __DOXYGEN__
/**
 * Set the maximum of usable message objects
 */
#define LI_CAN_SLV_ARCH_MAIN_NODE_MAX_NOF_MSG_OBJ
#define LI_CAN_SLV_ARCH_MON_NODE_MAX_NOF_MSG_OBJ
#else // #ifdef __DOXYGEN__
#ifndef LI_CAN_SLV_ARCH_MAIN_NODE_MAX_NOF_MSG_OBJ
#define LI_CAN_SLV_MAIN_NODE_MAX_NOF_MSG_OBJ (32)
#warning the maximum nubmer of usable message objects for the main CAN node ist set to 32.
#else // #ifndef LI_CAN_SLV_ARCH_MAIN_NODE_MAX_NOF_MSG_OBJ
#define LI_CAN_SLV_MAIN_NODE_MAX_NOF_MSG_OBJ (LI_CAN_SLV_ARCH_MAIN_NODE_MAX_NOF_MSG_OBJ)
#endif // #ifndef LI_CAN_SLV_ARCH_MAIN_NODE_MAX_NOF_MSG_OBJ

#ifdef LI_CAN_SLV_MON
#ifndef LI_CAN_SLV_ARCH_MON_NODE_MAX_NOF_MSG_OBJ
#define LI_CAN_SLV_MON_NODE_MAX_NOF_MSG_OBJ (32)
#warning the maximum nubmer of usable message objects for the monitor CAN node ist set to 32.
#else // #ifndef LI_CAN_SLV_ARCH_MON_NODE_MAX_NOF_MSG_OBJ
#define LI_CAN_SLV_MON_NODE_MAX_NOF_MSG_OBJ (LI_CAN_SLV_ARCH_MON_NODE_MAX_NOF_MSG_OBJ)
#endif // #ifndef LI_CAN_SLV_ARCH_MON_NODE_MAX_NOF_MSG_OBJ
#endif // #ifdef LI_CAN_SLV_MON
#endif // #ifdef __DOXYGEN__
/**
 * @}
 */

/**
 * @todo remove TUPLE and IO_CONFIG
 */
#ifdef __DOXYGEN__
/**
 * @ingroup li_can_slv_defines_deprecated
 * Set LI_CAN_SLV_MODULE_IO_CONFIG
 */
#define LI_CAN_SLV_USE_MODULE_IO_CONFIG
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_USE_MODULE_IO_CONFIG
#define LI_CAN_SLV_MODULE_IO_CONFIG
#endif
#endif // #ifdef __DOXYGEN__

#ifdef __DOXYGEN__
/**
 * @ingroup li_can_slv_defines_deprecated
 * Set LI_CAN_SLV_TUPLE
 */
#define LI_CAN_SLV_USE_TUPLE
#else // #ifdef __DOXYGEN__
#ifdef LI_CAN_SLV_USE_TUPLE
#define LI_CAN_SLV_TUPLE
#endif
#endif // #ifdef __DOXYGEN__

/**
 * @todo Is there a better way to document the defines if doxygen is running.
 */
#ifdef __DOXYGEN__
#define OUTER_APP
#define LI_CAN_SLV
#define LI_CAN_SLV_MAIN_MON

#define LI_CAN_SLV_RECONNECT
#define LI_CAN_SLV_RECONNECT_MAIN_NODE
#define LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK
#define LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
#define LI_CAN_SLV_SYS
#define LI_CAN_SLV_SYS_MODULE_ERROR
#define LI_CAN_SLV_SYS_MODULE_ERROR_16_BIT
#define LI_CAN_SLV_SYS_CHANGE_MODULE_NR

#define LI_CAN_SLV_SYNC
#define LI_CAN_SLV_ASYNC
#define LI_CAN_SLV_DLOAD
#define LI_CAN_SLV_ASYNC_TUNNEL

#define LI_CAN_SLV_BYTE_ORDER_LITTLE_ENDIAN
#define LI_CAN_SLV_BYTE_ORDER_BIG_ENDIAN
#define LI_CAN_SLV_MAIN_ARCH	CP_CHANNEL_1
#endif // #ifdef __DOXYGEN__

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

#endif // #ifndef LI_CAN_SLV_CONFIG_INTERNAL_H_
