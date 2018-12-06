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
 * @file li_can_slv_api.h
 * @addtogroup lcs_api
 * @{
 */

#ifndef LI_CAN_SLV_API_H_
#define LI_CAN_SLV_API_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/config/li_can_slv_config_internal.h>

#include <li_can_slv/api/li_can_slv_api_types.h>
#include <li_can_slv/api/li_can_slv_api_defines.h>
#include <li_can_slv/api/li_can_slv_api_error.h>

#include <li_can_slv/core/li_can_slv_core_api.h>

#include <li_can_slv/core/io_can.h>
#include <li_can_slv/sys/io_can_sys.h>
#include <li_can_slv/config/io_can_config.h>

#include <li_can_slv/error/io_can_error.h>

#ifdef LI_CAN_SLV_SYNC
#include <li_can_slv/sync/io_can_sync.h>
#endif // #ifdef LI_CAN_SLV_SYNC

#ifdef LI_CAN_SLV_ASYNC
#include <li_can_slv/async/io_can_async.h>
#endif // #ifdef LI_CAN_SLV_ASYNC

#if defined (LI_CAN_SLV_DLOAD) || defined(LI_CAN_SLV_ULOAD) || defined(LI_CAN_SLV_XLOAD)
#include <li_can_slv/xload/li_can_slv_xload.h>
#include <li_can_slv/xload/li_can_slv_xload_info.h>
#include <li_can_slv/xload/li_can_slv_xload_types.h>
#include <li_can_slv/xload/li_can_slv_xload_buffer_types.h>
#endif // #if defined (LI_CAN_SLV_DLOAD) || defined(LI_CAN_SLV_ULOAD) || defined(LI_CAN_SLV_XLOAD)

#ifdef LI_CAN_SLV_RECONNECT
#include <li_can_slv/core/io_can_reconnect.h>
#endif // #ifdef LI_CAN_SLV_RECONNECT

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
/**
 * @def lcsa_trigger_5msec
 * @brief This function must be called every 5 milliseconds.
 * @return LCSA_ERROR_OK
 */
#define lcsa_trigger_5msec()	li_can_slv_process()

/**
 * @brief With this function it is possible to set a module to a critical state. That
 * means if the module is added to li_can_slv master i will be in timeout after the call
 * off this function.
 \rst
  .. attention::
     After the module is in a critical state there is no way to bring the module back in a
     non critical state. The only possible solution is a reboot of the system.
 \endrst
 * @param module_type module type which should be set critical
 * @return #LCSA_ERROR_OK or #LCSA_ERROR_CONFIG_MODULE_TYPE_NOT_FOUND
 */
#define lcsa_set_module_critical(module_type)	can_config_set_critical(module_type)

/**
 * @def lcsa_is_module_valid(module_type, module_number, table_pos)
 * @param module_type module type of module
 * @param module_number module number of module
 * @param table_pos table position of the module
 * @return #LCSA_ERROR_OK or #LCSA_ERROR_CONFIG_MODULE_NOT_FOUND
 */
#define lcsa_is_module_valid(module_type, module_number, table_pos)	can_config_module_valid(module_type, module_number, table_pos)

/**
 * @def lcsa_get_main_module_number()
 * @return the module number of the main module type
 */
#define lcsa_get_main_module_number()	can_config_get_module_nr_main()

/**
 * @def lcsa_set_baudrate(baudrate)
 * @param baudrate is the  baud rate to set in kBaud
 * @return #LCSA_ERROR_OK or #ERR_MSG_CAN_CONFIG_SET_INVALID_BAUDRATE
 */
#define lcsa_set_baudrate(baudrate)	can_config_set_baudrate(baudrate)

#ifdef LI_CAN_SLV_SYNC
/**
 * @defgroup li_can_slv_api_sync Functions for synchronous protocol handling
 * @addtogroup li_can_slv_api_sync
 * @{
 */
/**
 * @brief lcsa_sync_trigger_process_periode
 */
#define lcsa_sync_trigger_process_periode()	li_can_slv_sync_trigger_process_periode()

/**
 * @brief li_can_slv_sync_get_process_periode
 * @return lcsa_errorcode_t or #LCSA_ERROR_OK if successful
 */
#define lcsa_sync_get_process_periode()	li_can_slv_sync_get_process_periode()

/**
 * Set function pointer to read the valid time for this module type
 * @def lcsa_sync_set_process_time_valid_fnc(module_type, pfnc)
 * @param module_type
 * @param pfnc function pointer to the valid function
 * @return lcsa_errorcode_t or #LCSA_ERROR_OK if successful
 */
#define lcsa_sync_set_process_time_valid_fnc(module_type, pfnc)	li_can_slv_sync_set_process_time_valid_fnc(module_type , pfnc)
/**
 * @}
 */

/**
 * @defgroup li_can_slv_api_sync_cbk Functions for synchronous protocol handling
 * @addtogroup li_can_slv_api_sync_cbk
 * @{
 */

/**
 * Set the callback function pointer for the function which should be called on the first status request.
 * @param pfnc function pointer to the callback
 * @return #LCSA_ERROR_OK if successful
 */
#define lcsa_set_first_status_request_cbk(pfnc) can_sys_set_first_status_request_call_fnc(pfnc)


/**
 * Set the callback function pointer for the function which should be called on every first received process request.
 * @param pfnc function pointer to the callback
 * @return #LCSA_ERROR_OK if successful
 */
#define lcsa_sync_set_first_process_request_cbk(pfnc) can_sync_set_first_process_request_call_fnc(pfnc)

/**
 * Set the callback function pointer for the function which should be called on every received process request.
 * @def lcsa_sync_set_process_request_cbk(module_type, pfnc)
 * @param module_type the module type for which the callback is set
 * @param module_number the module number for which the callback is set
 * @param pfnc function pointer to the callback
 * @return #LCSA_ERROR_OK if successful
 */
#define lcsa_sync_set_process_request_cbk(module_type, module_number, pfnc)	li_can_slv_sync_set_process_request_cbk(module_type, module_number, pfnc)

/**
 * Set the callback function pointer for the function which should be called on every valid process image.
 * @def lcsa_sync_set_process_image_valid_cbk(module_type, pfnc)
 * @param module_type the module type for which the callback is set
 * @param module_number the module number for which the callback is set
 * @param pfnc function pointer to the callback
 * @return #LCSA_ERROR_OK if successful
 *
\rst
.. tip::
	The callback will be called if the image for the current module is valid. Here is an example of a possible implementation:
	::
		// add the callback
		err = lcsa_sync_set_process_image_valid_cbk(module_type, module_number, &app_my_module_process_image_valid_cbk);
		if (err == LCSA_ERROR_OK)
		{
			return(err);
		}

		// callback implementation
		static void app_my_module_process_image_valid_cbk(void)
		{

		}
\endrst
 */
#define lcsa_sync_set_process_image_valid_cbk(module_type, module_number, pfnc)	li_can_slv_sync_set_process_image_valid_cbk(module_type, module_number, pfnc)

/**
 * Set the callback function pointer for the function which should be called on every not valid process image.
 * @def lcsa_sync_set_process_image_valid_cbk(module_type, pfnc)
 * @param module_type the module type for which the callback is set
 * @param module_number the module number for which the callback is set
 * @param pfnc function pointer to the callback
 * @return #LCSA_ERROR_OK if successful
 **/
#define lcsa_sync_set_process_image_not_valid_cbk(module_type, module_number, pfnc)	li_can_slv_sync_set_process_image_not_valid_cbk(module_type, module_number, pfnc)

/**
 * Set the callback function pointer for the function which should be called if the whole image was received valid.
 * @def lcsa_sync_set_whole_process_image_valid_cbk(pfnc)
 * @param pfnc function pointer to the callback
 * @return #LCSA_ERROR_OK if successful
 */
#define lcsa_sync_set_whole_process_image_valid_cbk(pfnc)	can_sync_set_pr_call_fnc(pfnc)
/**
 * @}
 */
#endif // #ifdef LI_CAN_SLV_SYNC

#ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR
/**
 * @defgroup lcsa_api_change_module_nr Change Module Number System Command
 * @addtogroup lcsa_api_change_module_nr
 * @{
 */
/**
 * @def lcsa_change_module_nr_and_identifiers(module_type, new_module_number, actual_module_number)
 * @param module_type CAN module type as char_t
 * @param new_module_number the new module number where the identifier is calculated from
 * @param actual_module_number the actual module number of the module to identify the exact module if two of the same modules are on the line
 * @return #LCSA_ERROR_OK or too many messages to specify
 */
#define lcsa_change_module_nr_and_identifiers(module_type, new_module_number, actual_module_number)	li_can_slv_config_change_module_nr_and_identifiers(module_type, new_module_number, actual_module_number)

/**
 * @def lsca_set_module_nr_change_cbk_voter(pfnc)
 * @param pfnc function pointer to the voter which has to return an state of type @ref lcsa_module_change_voter_state_t and will get the parameters of the module_type, the desired new number
 * and the actual module number
\rst
.. tip::
	The callback as function pointer will be called on each change module number command and can be used to store the new module number information in an persistent storage for example.
	::
		lcsa_module_change_voter_state_t vote_module_change_valid(char_t *module_type, lcsa_module_number_t new_module_nr, lcsa_module_number_t act_module_nr)
		{
			if((new_module_nr < MODULE_NR_MAX) && (new_module_nr > MODULE_NR_MIN))
			{
				// ...store new module number in persistent storage for next start up
				// ...or do any special handling
				return(LI_CAN_SLV_MODULE_CHANGE_VALID);
			}
			else
			{
				return(LI_CAN_SLV_MODULE_CHANGE_FORBIDDEN);
			}
		}
\endrst
 */
#define lsca_set_module_nr_change_cbk_voter(pfnc) li_can_slv_config_set_module_nr_change_voter(pfnc)

/**
 * @def lcsa_set_module_silent_awake_from_type_and_nr(module_type, module_number, state)
 * @param module_type CAN module type as char_t
 * @param module_number module number of the module to identify the exact module if two of the same modules are on the line
 * @param state in type of lcsa_module_response_sate_t
 * @return #LCSA_ERROR_OK or #ERR_MSG_CAN_CONFIG_MODULE_NOT_FOUND or #ERR_MSG_CAN_CONFIG_INVALID_SILENT_AWAKE_STATE
 */
#define lcsa_set_module_silent_awake_from_type_and_nr(module_type, module_number, state) li_can_slv_config_set_module_silent_awake_from_type_and_nr(module_type, module_number, state)
/**
 * @}
 */
#endif // #ifdef LI_CAN_SLV_SYS_CHANGE_MODULE_NR

#ifdef LI_CAN_SLV_USE_SYS_FACTORY_RESET_CALLBACK
/**
 * Set the factory reset callback function pointer.
 * @param pfnc function pointer to the callback
 */
#define lcsa_set_factory_reset_cbk(pfnc)	li_can_slv_sys_set_factory_reset_cbk(pfnc)
#endif // #ifdef LI_CAN_SLV_USE_SYS_FACTORY_RESET_CALLBACK

#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
/**
 * @defgroup li_can_slv_api_sync_error Functions for module/sensor error handling
 * @addtogroup li_can_slv_api_sync_error
 * @{
 */
/**
 * @param msg_code message code of the error
 * @param priority the priority used for this message. Usage of #LCSA_ERROR_MSG_PRIO_INFO, #LCSA_ERROR_MSG_PRIO_WARNING or #LCSA_ERROR_MSG_PRIO_ERROR is possible.
 * @param group group definition of the message for example use #LCSA_ERROR_GROUP_CAN for group CAN
 * @param subgroup subgroup definition of the message for example use #LCSA_ERROR_SUBGROUP_CAN_APPLICATION for subgroup CAN application
 * @param module_nr logical CAN module number that caused the error
 * @return #LCSA_ERROR_OK if successful
 */
#define lcsa_send_module_error(msg_code, priority, group, subgroup, module_nr)	error_syserr_send_full(msg_code, 0, priority, group, subgroup, module_nr)

/**
 * @def lcsa_send_module_error_with_add_info(msg_code, add_info, priority, group, subgroup, module_nr)
 * @param msg_code message code of the error
 * @param add_info additional signed info byte
 * @param priority the priority used for this message. Usage of #LCSA_ERROR_MSG_PRIO_INFO, #LCSA_ERROR_MSG_PRIO_WARNING or #LCSA_ERROR_MSG_PRIO_ERROR is possible.
 * @param group group definition of the message for example use #LCSA_ERROR_GROUP_CAN for group CAN
 * @param subgroup subgroup definition of the message for example use #LCSA_ERROR_SUBGROUP_CAN_APPLICATION for subgroup CAN application
 * @param module_nr logical CAN module number that caused the error.
 * @return lcsa_errorcode_t or #LCSA_ERROR_OK if successful
 */
#define lcsa_send_module_error_with_add_info(msg_code, add_info, priority, group, subgroup, module_nr)	error_syserr_send_full(msg_code, add_info, priority, group, subgroup, module_nr) /**< send module error with info */

/**
 * @def lcsa_send_sensor_error(msg_code, priority,  module_nr)
 * @param msg_code message code of the error
 * @param priority the priority used for this message. Usage of #LCSA_ERROR_MSG_PRIO_INFO, #LCSA_ERROR_MSG_PRIO_WARNING or #LCSA_ERROR_MSG_PRIO_ERROR is possible.
 * @param module_nr logical CAN module number that caused the error.
 * @return lcsa_errorcode_t or #LCSA_ERROR_OK if successful
 */
#define lcsa_send_sensor_error(msg_code, priority,  module_nr)	error_syserr_send_full(msg_code, 0, priority, MSG_EXTRACT_GROUP(MSG_GROUP_CAN), MSG_EXTRACT_GROUP_SUBGROUP(MSG_SUBGROUP_CAN_SENSOR), module_nr) /**< send sensor error */

/**
 * @}
 */
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR

#ifdef LI_CAN_SLV_DLOAD
/**
 * @defgroup li_can_slv_api_dload Download Functions
 * @addtogroup li_can_slv_api_dload
 * @{
 */

/**
 * @param hdl handle function for download start
 * @return #LCSA_ERROR_DLOAD_ADD_HANDLE or #LCSA_ERROR_OK
 */
#define lcsa_dload_set_start_handle(hdl)	li_can_slv_dload_set_start_handle(hdl)

/**
 * @param hdl handle function for download version request
 * @return #LCSA_ERROR_DLOAD_ADD_HANDLE or #LCSA_ERROR_OK
 */
#define lcsa_dload_set_version_request_handle(hdl)	li_can_slv_dload_set_version_request_handle(hdl);

/**
 * @param hdl handle function for download start request
 * @return #LCSA_ERROR_DLOAD_ADD_HANDLE or #LCSA_ERROR_OK
 */
#define lcsa_dload_set_start_request_handle(hdl)	li_can_slv_dload_set_start_request_handle(hdl);

/**
 * @param hdl handle function for data block end
 * @return #LCSA_ERROR_DLOAD_ADD_HANDLE or #LCSA_ERROR_OK
 */
#define lcsa_dload_set_data_block_end_handle(hdl)	li_can_slv_dload_set_data_block_end_handle(hdl);

/**
 * @param hdl handle function for download end
 * @return #LCSA_ERROR_DLOAD_ADD_HANDLE or #LCSA_ERROR_OK
 */
#define lcsa_dload_set_end_handle(hdl)	li_can_slv_dload_set_end_handle(hdl);

/**
 * @}
 */
#endif // #ifdef LI_CAN_SLV_DLOAD

#ifdef LI_CAN_SLV_ULOAD
/**
 * @defgroup li_can_slv_api_uload Upload Functions
 * @addtogroup li_can_slv_api_uload
 * @{
 */

/**
 * @param hdl handle function for upload info request
 * @return #LCSA_ERROR_XLOAD_ADD_HANDLE or #LCSA_ERROR_OK
 */
#define lcsa_uload_set_info_request_handle(hdl)	li_can_slv_uload_set_info_request_handle(hdl);

/**
 * @param hdl handle function for upload start request
 * @return #LCSA_ERROR_XLOAD_ADD_HANDLE or #LCSA_ERROR_OK
 */
#define lcsa_uload_set_start_request_handle(hdl)	li_can_slv_uload_set_start_request_handle(hdl);

/**
 * @param hdl handle function for upload block acknowledge
 * @return #LCSA_ERROR_XLOAD_ADD_HANDLE or #LCSA_ERROR_OK
 */
#define lcsa_uload_set_uload_block_ack_handle(hdl)	li_can_slv_uload_set_uload_block_ack_handle(hdl);

/**
 * @}
 */
#endif // #ifdef LI_CAN_SLV_ULOAD

#ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK
/**
 * @defgroup li_can_slv_api_reconnect_online Reconnect Online Callback Functions
 * @addtogroup li_can_slv_api_reconnect_online
 * @{
 */
/**
 * @brief This function sets the reconnect online change baud rate start callback
 * @param pfnc function to handle the reconnect online change baud rate start callback
 */
#define lcsa_reconnect_set_online_change_baudrate_start_cbk(pfnc)	li_can_slv_reconnect_set_online_change_baudrate_start_cbk(pfnc)

/**
 * @brief This function sets the reconnect online change baud rate stop callback
\rst
.. tip::
	This function can be used to save the baud rate as an example:
	::
	  static void reconnect_online_change_baudrate_stop(lcsa_bdr_t baudrate)
	  {
	    // save the baud rate here or do what ever you want to do...
	  }

	Now this callback can be set that way:
	::
	  lcsa_reconnect_set_online_change_baudrate_stop_cbk(reconnect_online_change_baudrate_stop);
\endrst
 * @param pfnc function to handle the reconnect online change baud rate stop callback
 */
#define lcsa_reconnect_set_online_change_baudrate_stop_cbk(pfnc)	li_can_slv_reconnect_set_online_change_baudrate_stop_cbk(pfnc)
/**
 * @}
 */
#endif // #ifdef LI_CAN_SLV_RECONNECT_ONLINE_CHANGE_BAUDRATE_CBK

#ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK
/**
 * @defgroup li_can_slv_api_reconnect_startup Reconnect Startup Callback Functions
 * @addtogroup li_can_slv_api_reconnect_startup
 * @{
 */
/**
 * @brief This function sets the reconnect startup change baud rate start callback
 * @param pfnc function to handle the reconnect startup change baud rate start callback
 */
#define lcsa_reconnect_set_startup_change_baudrate_start_cbk(pfnc)	li_can_slv_reconnect_set_startup_change_baudrate_start_cbk(pfnc)

/**
 * @brief This function sets the reconnect startup change baud rate stop callback
 \rst
 .. attention::
     Do not use this function to save the baud rate.
 \endrst
 * @param pfnc function to handle the reconnect startup change baud rate stop callback
 */
#define lcsa_reconnect_set_startup_change_baudrate_stop_cbk(pfnc)	li_can_slv_reconnect_set_startup_change_baudrate_stop_cbk(pfnc)
/**
 * @}
 */
#endif // #ifdef LI_CAN_SLV_RECONNECT_STARTUP_CHANGE_BAUDRATE_CBK

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/
/**
 * @brief This function is used to initialize the stack.
 * @param baudrate Use one of the available baud rate defines: @link li_can_slv_api_defines_baudrate @endlink
 */
#ifdef LI_CAN_SLV_DLOAD
/**
 * @return #LCSA_ERROR_OK or #LCSA_ERROR_XLOAD_INFO_BAD_CRC if #LI_CAN_SLV_USE_ASYNC_DOWNLOAD is used.
 */
#else // #ifdef LI_CAN_SLV_DLOAD

/**
 * @return #LCSA_ERROR_OK
 */
#endif // #ifdef LI_CAN_SLV_DLOAD
extern lcsa_errorcode_t lcsa_init(lcsa_bdr_t baudrate);

/**
 *
 \rst
  .. attention::
     The function should be called at a point where the system is setup correctly and ready for normal operation.
     Because after that call the master is allowed to start the normal operation.
 \endrst
 * @brief Use this function to start the general handling of the slave stack.
 * @return #LCSA_ERROR_OK
 */
extern lcsa_errorcode_t lcsa_start(void);

#ifndef LI_CAN_SLV_BOOT
/**
 * @brief add module to can configuration
 * @remark In case of multiple usage of the module number the default
 * module number is set and a according error is returned.
 * @param module is the pointer to application module type configuration
 * @param module_nr is the module number of the configured module
 * @param rx0 data pointer to application data of CAN receive object 0
 * @param rx1 data pointer to application data of CAN receive object 1
 * @param rx2 data pointer to application data of CAN receive object 2
 * @param rx3 data pointer to application data of CAN receive object 3
 * @param tx0 data pointer to application data of CAN transmit object 0
 * @param tx1 data pointer to application data of CAN transmit object 1
 * @param tx2 data pointer to application data of CAN transmit object 2
 * @param tx3 data pointer to application data of CAN transmit object 3
 * @return #LCSA_ERROR_OK if successful other send the error over CAN
 */
extern lcsa_errorcode_t lcsa_add_module(const lcsa_module_config_t *module, lcsa_module_number_t module_nr, void *rx0, void *rx1, void *rx2, void *rx3, void *tx0, void *tx1, void *tx2, void *tx3);
#endif // #ifndef LI_CAN_SLV_BOOT

#ifdef LI_CAN_SLV_UNKNOWN_MODULE
#define lcsa_add_ukwn_module(module_type, module_nr)	can_config_add_ukwn_module(module_type, module_nr);
#endif // #ifdef LI_CAN_SLV_UNKNOWN_MODULE

/**
 * @brief get current configured CAN baud rate
 * @return the current used baud rate
 */
extern lcsa_bdr_t lcsa_get_baudrate(void);

/**
 * @brief This function can be used to check if the baudrate is valid
 * @param bdr baudrate to check
 */
#define lcsa_is_baudrate_valid(bdr)	can_config_is_baudrate_valid(bdr)

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef LI_CAN_SLV_API_H_

/** @} */
