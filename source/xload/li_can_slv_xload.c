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
 * \brief This module implements the functionality used for download from the Liebherr CAN protocol.
 * \remarks
 * \arg It has to be checked if the received component can be programmed or if it is allowed to be programmed.
 * (The InnerCore (IC) can only program an OuterCore (OC))\n
 * \arg The destination (the resource) for the download is defined via the component name.\n
 * 	- intel hex files (detect the sector number corresponding to the name of the component)
 * 	- tuples (only one tuple per tuple file)
 * 	\arg it has to be guaranteed that only one download is possible at the same moment. A second download
 * 	has to be cancelled via the download termination message.
 * 	\arg the module number for the download is stored in the SysInfo area. (the module number is necessary for the
 * 	asynchronous acknowledge messages)
 * 	\arg check the address area for the corresponding HEX component<br>
 *
 * 	Following functions are implemented\n\n
 *
 * 	- <b>Messages from Master to Slave</b>
 * 	<table>
 * 		<tr> <td ><b>system message</b>           	<td ><b>Byte1 (ID of message)</b>  <td ><b>processed</b> </td></tr>
 * 		<tr> <td >download version request simple   <td >0x10                          <td >li_can_slv_dload_version_request_short </td></tr>
 * 		<tr> <td >download version request long 1   <td >0x11                          <td >li_can_slv_dload_version_request_long1 </td></tr>
 * 		<tr> <td >download version request long 2   <td >0x12                          <td >li_can_slv_dload_version_request_long2 </td></tr>
 * 		<tr> <td >download program start request 1  <td >0x13                          <td >li_can_slv_dload_start_request1 </td></tr>
 * 		<tr> <td >download program start request 2  <td >0x14                          <td >li_can_slv_dload_start_request2 </td></tr>
 * 		<tr> <td >download data block               <td >0x15                          <td >li_can_slv_dload_data </td></tr>
 * 		<tr> <td >download data block end           <td >0x16                          <td >li_can_slv_dload_data_block_end </td></tr>
 * 		<tr> <td >download program end              <td >0x17                          <td >li_can_slv_dload_end </td></tr>
 * 	</table><br>
 * 	- <b>Messages from Slave to Master</b>
 * 	<table>
 * 		<tr> <td ><b>system message</b>       <td ><b>Byte1 (ID of message)</b> <td ><b>processed</b> </td></tr>
 * 		<tr> <td >download version answer     <td >0x18                         <td >li_can_slv_dload_version_answer </td></tr>
 * 		<tr> <td >download start acknowledge  <td >0x19                         <td >li_can_slv_dload_start_ackn </td></tr>
 * 		<tr> <td >download block acknowledge  <td >0x1A                         <td >dload_block_ackn </td></tr>
 * 		<tr> <td >download end acknowledge    <td >0x1B                         <td >li_can_slv_dload_end_ackn </td></tr>
 * 		<tr> <td >download termination        <td >0x1C                         <td >li_can_slv_dload_termination </td></tr>
 * 	</table><br>
 *
 * 	- <b>Download version request (short, long1+long2):</b><br>
 * 	The data (index of request, component name) is extracted from the received messages. With this information the CRC and
 * 	the date of the component is calculated (or taken from the EEPROM) and sent to the master as version answer.
 * 	- <b>Download program start request (1+2):</b><br>
 * 	\arg <b>Download data:</b>\n
 * 	data is stored in data buffer
 * 	\arg <b>Download data block end:</b>\n
 * 	Data buffer is written to the area defined through the component name:
 * 	- EEPROM (tuple data and IO configuration)
 * 	- Flash (intel hex data)
 * 	\arg <b>Download program end:</b>\n
 * 	\pre	\arg size of download buffer: #LI_CAN_SLV_XLOAD_BLOCK_BUFFER_SIZE_DLOAD bytes (--> #LI_CAN_SLV_XLOAD_BLOCK_BUFFER_SIZE_DLOAD bytes is the maximum tuple size)
 * 	\arg a tuple file to download may only contain a single tuple.\n
 * 	\arg for download to the flash only intel hex files are provided.\n
 * 	\arg only 1 download instance is possible at the same time. (a parallel loading of various components is not possible.)
 */

/**
 * @file li_can_slv_xload.c
 * @brief li_can_slv dload functions
 * @addtogroup lcs_xload
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/xload/li_can_slv_xload.h>
#include <li_can_slv/xload/li_can_slv_xload_api.h>
#include <li_can_slv/error/io_can_errno.h>
#include <li_can_slv/async/io_can_async.h>
#include <li_can_slv/core/io_can_main.h>
#include <li_can_slv/core/io_can.h>
#include <li_can_slv/core/io_can_types.h>
#include "io_can_port.h"
#include <li_can_slv/config/io_can_config.h>

#include <li_can_slv/xload/li_can_slv_xload_buffer.h>
#include <li_can_slv/core/li_can_slv_core_defines.h>

#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_debug.h"
#endif // #ifdef LI_CAN_SLV_DEBUG

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#define ULOAD_STATE_END				(0)
#define ULOAD_STATE_FILL_BUFFER		(1)
#define ULOAD_STATE_SEND_MESSAGE	(2)

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
static li_can_slv_dload_start_handle_funcp_t dload_start_handle_funcp = NULL;
static li_can_slv_dload_version_request_handle_funcp_t dload_version_request_handle_funcp = NULL;
static li_can_slv_dload_start_request_handle_funcp_t dload_start_request_handle_funcp = NULL;
static li_can_slv_dload_data_block_end_handle_funcp_t dload_data_block_end_handle_funcp = NULL;
static li_can_slv_dload_end_handle_funcp_t dload_end_handle_funcp = NULL;

#ifdef LI_CAN_SLV_ULOAD
static li_can_slv_uload_info_request_handle_funcp_t uload_info_request_handle_funcp = NULL;
static li_can_slv_uload_start_request_handle_funcp_t uload_start_request_handle_funcp = NULL;
static li_can_slv_uload_block_ack_handle_funcp_t uload_block_ack_handle_funcp = NULL;
#endif // #ifdef LI_CAN_SLV_ULOAD

static li_can_slv_xload_progress_t xload_in_progress = LI_CAN_SLV_XLOAD_IDLE;

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/
static li_can_slv_errorcode_t dload_version_request_pre_handle(li_can_slv_module_nr_t module_nr);
static li_can_slv_errorcode_t dload_block_ackn(li_can_slv_module_nr_t module_nr);
#ifdef LI_CAN_SLV_ULOAD
static li_can_slv_errorcode_t uload_info_request_pre_handle(li_can_slv_module_nr_t module_nr);
#endif // #ifdef LI_CAN_SLV_ULOAD
/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
/**
 * @brief Function writes the number of maximum bytes per block and time delay (in ms) between the objects
 * @remarks the first byte of destination buffer is the identifier of the data object.
 * @param module_nr number of the module for the download
 * @param max_bytes_per_block maximum bytes to receive per block
 * @param t_delay_objs time delay between CAN objects
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_dload_start_ackn(li_can_slv_module_nr_t module_nr, uint16_t max_bytes_per_block, byte_t t_delay_objs)
{
	byte_t acknowledge_buffer[8];
	li_can_slv_errorcode_t rc;

	/* initialization of download counters */
	dload_buffer.nr_of_can_objs = 0;
	dload_buffer.bytes_cnt_of_block = 0;

#ifdef LI_CAN_SLV_DLOAD_BUFFER_INTERNAL
	/* initialization of circular buffer */
	dload_buffer.read = 0;
	dload_buffer.write = 0;
	dload_buffer.nr_bytes = 0;
#endif // #ifdef LI_CAN_SLV_DLOAD_BUFFER_INTERNAL

	acknowledge_buffer[0] = CAN_ASYNC_DL_START_ACKN;
	acknowledge_buffer[1] = (byte_t)((max_bytes_per_block & 0xFF00u) >> 8);
	acknowledge_buffer[2] = (byte_t)(max_bytes_per_block & 0x00FFu);
	acknowledge_buffer[3] = (byte_t) t_delay_objs;

	can_port_memory_set(&acknowledge_buffer[4], 0x00, 4);

#ifdef LI_CAN_SLV_DEBUG_DLOAD_START
	LI_CAN_SLV_DEBUG_PRINT("\nlcsdl_start_ackn");
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_START

	rc = can_async_send_data_to_async_ctrl_tx_queue(module_nr, CAN_ASYNC_CTRL_TX_TYPE_SLAVE, acknowledge_buffer);
	return rc;
}

/**
 * @param handle handle function for dload start
 * @return ERR_MSG_CAN_DLOAD_ADD_HANDLE or LI_CAN_SLV_ERR_OK
 */
li_can_slv_errorcode_t li_can_slv_dload_set_start_handle(li_can_slv_dload_start_handle_funcp_t handle)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if (NULL == dload_start_handle_funcp)
	{
		dload_start_handle_funcp = handle;
	}
	else
	{
		err = ERR_MSG_CAN_DLOAD_ADD_HANDLE;
	}
	return err;
}

/**
 * @param handle handle function for download version request
 * @return ERR_MSG_CAN_DLOAD_ADD_HANDLE or LI_CAN_SLV_ERR_OK
 */
li_can_slv_errorcode_t li_can_slv_dload_set_version_request_handle(li_can_slv_dload_version_request_handle_funcp_t handle)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if (NULL == dload_version_request_handle_funcp)
	{
		dload_version_request_handle_funcp = handle;
	}
	else
	{
		err = ERR_MSG_CAN_DLOAD_ADD_HANDLE;
	}
	return err;
}

/**
 * @param handle handle function for download start request
 * @return ERR_MSG_CAN_DLOAD_ADD_HANDLE or LI_CAN_SLV_ERR_OK
 */
li_can_slv_errorcode_t li_can_slv_dload_set_start_request_handle(li_can_slv_dload_start_request_handle_funcp_t handle)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if (NULL == dload_start_request_handle_funcp)
	{
		dload_start_request_handle_funcp = handle;
	}
	else
	{
		err = ERR_MSG_CAN_DLOAD_ADD_HANDLE;
	}
	return err;
}

/**
 * @param handle handle function for data block end
 * @return ERR_MSG_CAN_DLOAD_ADD_HANDLE or LI_CAN_SLV_ERR_OK
 */
li_can_slv_errorcode_t li_can_slv_dload_set_data_block_end_handle(li_can_slv_dload_data_block_end_handle_funcp_t handle)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if (NULL == dload_data_block_end_handle_funcp)
	{
		dload_data_block_end_handle_funcp = handle;
	}
	else
	{
		err = ERR_MSG_CAN_DLOAD_ADD_HANDLE;
	}
	return err;
}

/**
 * @param handle handle function for download end
 * @return ERR_MSG_CAN_DLOAD_ADD_HANDLE or LI_CAN_SLV_ERR_OK
 */
li_can_slv_errorcode_t li_can_slv_dload_set_end_handle(li_can_slv_dload_end_handle_funcp_t handle)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if (NULL == dload_end_handle_funcp)
	{
		dload_end_handle_funcp = handle;
	}
	else
	{
		err = ERR_MSG_CAN_DLOAD_ADD_HANDLE;
	}
	return err;
}

#ifdef LI_CAN_SLV_ULOAD
/**
 * @param handle handle function for upload info request
 * @return ERR_MSG_CAN_DLOAD_ADD_HANDLE or LI_CAN_SLV_ERR_OK
 */
li_can_slv_errorcode_t li_can_slv_uload_set_info_request_handle(li_can_slv_uload_info_request_handle_funcp_t handle)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if (NULL == uload_info_request_handle_funcp)
	{
		uload_info_request_handle_funcp = handle;
	}
	else
	{
		err = ERR_MSG_CAN_ULOAD_ADD_HANDLE;
	}
	return err;
}

/**
 * @param handle handle function for upload start request
 * @return ERR_MSG_CAN_DLOAD_ADD_HANDLE or LI_CAN_SLV_ERR_OK
 */
li_can_slv_errorcode_t li_can_slv_uload_set_start_request_handle(li_can_slv_uload_start_request_handle_funcp_t handle)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if (NULL == uload_start_request_handle_funcp)
	{
		uload_start_request_handle_funcp = handle;
	}
	else
	{
		err = ERR_MSG_CAN_ULOAD_ADD_HANDLE;
	}
	return err;
}

/**
 * @param handle handle function for upload block acknowledge
 * @return ERR_MSG_CAN_DLOAD_ADD_HANDLE or LI_CAN_SLV_ERR_OK
 */
li_can_slv_errorcode_t li_can_slv_uload_set_uload_block_ack_handle(li_can_slv_uload_block_ack_handle_funcp_t handle)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if (NULL == uload_block_ack_handle_funcp)
	{
		uload_block_ack_handle_funcp = handle;
	}
	else
	{
		err = ERR_MSG_CAN_ULOAD_ADD_HANDLE;
	}
	return err;
}
#endif // #ifdef LI_CAN_SLV_ULOAD

/**
 * @brief function writes the version answer to destination buffer
 * @remarks the first byte of destination buffer is the identifier of the data object.
 * @param module_nr number of the module for the download
 * @param index index of request (bytes 2 of data)
 * @param crc
 * @param download_id
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_dload_version_answer(li_can_slv_module_nr_t module_nr, byte_t index, uint16_t crc, uint32_t download_id)
{
	uint8_t data[8];
	li_can_slv_errorcode_t rc;

#ifdef LI_CAN_SLV_BOOT
	li_can_slv_set_mode(LI_CAN_SLV_MODE_OPERATIONAL);
	(void)can_main_enable();
#endif

#ifdef LI_CAN_SLV_DEBUG_DLOAD_VERSION
	LI_CAN_SLV_DEBUG_PRINT("\nlcsdl_ver_ans crc:0x%04X and id:%lu", crc, download_id);
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_VERSION
	data[0] = CAN_ASYNC_DL_VERSION_ANSWER;
	data[1] = (uint8_t) index;
	data[2] = (uint8_t)(crc >> 8U);
	data[3] = (uint8_t)(crc);
	data[4] = (uint8_t)((download_id & 0xFF000000) >> 24);
	data[5] = (uint8_t)((download_id & 0x00FF0000) >> 16);
	data[6] = (uint8_t)((download_id & 0x0000FF00) >> 8);
	data[7] = (uint8_t)(download_id & 0x000000FF);
	rc = can_async_send_data_to_async_ctrl_tx_queue(module_nr, CAN_ASYNC_CTRL_TX_TYPE_SLAVE, data);
	return rc;
}

/**
 * @brief function receives the version request short data. The crc and download_id of the
 * given component is calculated and the private function li_can_slv_dload_version_answer is called
 * @remarks byte 1 of source data is the identifier of the object. the identifier is not checked in this function.
 * @param module_nr number of the module for the download
 * @param[in] src pointer to the source of the download data
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_dload_version_request_short(li_can_slv_module_nr_t module_nr, const byte_t *src)
{
	li_can_slv_errorcode_t rc = LI_CAN_SLV_ERR_OK;

	xload_component_version_request.index_of_request = src[1];
	xload_component_version_request.name[0] = (char_t)(0x20U + (src[2] >> 2U));
	xload_component_version_request.name[1] = (char_t)(0x20U + ((src[2] & 0x03U) << 4U) + ((src[3] & 0xF0) >> 4U));
	xload_component_version_request.name[2] = (char_t)(0x20U + ((src[3] & 0x0FU) << 2U) + ((src[4] & 0xC0) >> 6U));
	xload_component_version_request.name[3] = (char_t)(0x20U + (src[4] & 0x3FU));
	xload_component_version_request.name[4] = (char_t)(0x20U + ((src[5] & 0xFCU) >> 2U));
	xload_component_version_request.name[5] = (char_t)(0x20U + ((src[5] & 0x03U) << 4U) + ((src[6] & 0xF0) >> 4U));
	xload_component_version_request.name[6] = (char_t)(0x20U + ((src[6] & 0x0FU) << 2U) + ((src[7] & 0xC0) >> 6U));
	xload_component_version_request.name[7] = (char_t)(0x20U + (src[7] & 0x3FU));
	xload_component_version_request.name[8] = '\0';

#ifdef LI_CAN_SLV_DEBUG_DLOAD_VERSION
	LI_CAN_SLV_DEBUG_PRINT("\nlcsdl_ver_req short");
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_VERSION

	rc = dload_version_request_pre_handle(module_nr);
	return rc;
}

/**
 * @brief function receives the version request long1 data. The first 6 characters of the component name is stored
 * in the private data structure.
 * @remarks byte 1 of source data is the identifier of the object. the identifier is not checked in this function.
 * @param module_nr number of the module for the download
 * @param[in] src pointer to the source of the download data
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_dload_version_request_long1(li_can_slv_module_nr_t module_nr, const byte_t *src)
{
	uint16_t i;

	module_nr = module_nr; //dummy assignment
	xload_component_version_request.index_of_request = src[1];
	for (i = 0; i < 6; i++)
	{
		xload_component_version_request.name[i] = src[i + 2];
	}

#ifdef LI_CAN_SLV_DEBUG_DLOAD_VERSION
	LI_CAN_SLV_DEBUG_PRINT("\nlcsdl_ver_req long1");
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_VERSION

	return LI_CAN_SLV_ERR_OK;
}

/**
 * @brief function receives the version request long2 data. The last 2 characters of the component name are stored
 * in the private data structure. The crc and the download_id of the given	component is calculated and the private function
 * li_can_slv_dload_version_answer is called.
 * @remarks byte 1 of source data is the identifier of the object. the identifier is not checked in this function.
 * @param module_nr number of the module for the download
 * @param[in] src pointer to the source of the download data
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_dload_version_request_long2(li_can_slv_module_nr_t module_nr, const byte_t *src)
{
	li_can_slv_errorcode_t rc = LI_CAN_SLV_ERR_OK;

#if defined(OUTER) || defined(OUTER_APP)
	//  static li_can_slv_module_nr_t module_nr_bgnd; // needed for background task
#endif // #if defined(OUTER) || defined(OUTER_APP)

	xload_component_version_request.name[6] = src[2];
	xload_component_version_request.name[7] = src[3];
	xload_component_version_request.name[8] = '\0';

#ifdef LI_CAN_SLV_DEBUG_DLOAD_VERSION
	LI_CAN_SLV_DEBUG_PRINT("\nlcsdl_ver_req long2");
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_VERSION

	rc = dload_version_request_pre_handle(module_nr);
	return rc;
}

/**
 * @brief function receives the first 7 characters of the component name for download
 * @remarks byte 1 of source data is the identifier of the object. the identifier is not checked in this function.
 * @param module_nr number of the module for the download
 * @param[in] src pointer to the source of the download data
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_dload_start_request1(li_can_slv_module_nr_t module_nr, const byte_t *src)
{
	uint16_t i;

#ifdef LI_CAN_SLV_DEBUG_DLOAD_START
	LI_CAN_SLV_DEBUG_PRINT("\nlcsdl_start_req 1");
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_START

	module_nr = module_nr; //dummy assignment
	if (xload_in_progress == LI_CAN_SLV_XLOAD_IDLE)
	{
		for (i = 0; i < (CAN_DLC_MAX - 1); i++)
		{
			xload_component.name[i] = src[i + 1];
		}
	}

	return LI_CAN_SLV_ERR_OK;
}

/**
 * @brief function receives the last character of the component name, the component crc and the component download_id
 * and saves it in the corresponding structure members. The private function li_can_slv_dload_start_ackn is called in this
 * function. This function also prepares the flash for the download (erasing of corresponding sectors). This module
 * stores the component information in the system info area and sets the module via reset to download mode.
 * @remarks byte 1 of source data is the identifier of the object. the identifier is not checked in this function.
 * @param module_nr number of the module for the download
 * @param[in] src pointer to the source of the download data
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_dload_start_request2(li_can_slv_module_nr_t module_nr, const byte_t *src)
{
	li_can_slv_errorcode_t rc = LI_CAN_SLV_ERR_OK;
#if defined(OUTER) || defined(OUTER_APP)
	uint16_t table_pos;
#endif // #if defined(OUTER) || defined(OUTER_APP)
	uint16_t module_found;
	can_config_module_silent_t module_silent;

#ifdef LI_CAN_SLV_DEBUG_DLOAD_START
	LI_CAN_SLV_DEBUG_PRINT("\nlcsdl_start_req 2");
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_START

	if (xload_in_progress == LI_CAN_SLV_XLOAD_IDLE)
	{
		xload_in_progress = LI_CAN_SLV_XLOAD_IN_PROGRESS;
		xload_component.name[7] = src[1];
		xload_component.name[8] = '\0';
		xload_component.crc = src[2] << 8;
		xload_component.crc += src[3];

		xload_component.download_id = ((uint32_t) src[4] << 24);
		xload_component.download_id += ((uint32_t) src[5] << 16);
		xload_component.download_id += ((uint32_t) src[6] << 8);
		xload_component.download_id += (uint32_t) src[7];
		xload_component.module_nr = module_nr;

#ifdef LI_CAN_SLV_DEBUG_DLOAD_START_EXTENDED
		LI_CAN_SLV_DEBUG_PRINT("\nlcsdl_start_req 2");
		LI_CAN_SLV_DEBUG_PRINT(" name:%s with crc:%d and id:%ld", xload_component.name, xload_component.crc, xload_component.download_id);
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_START_EXTENDED

#ifdef LI_CAN_SLV_BOOT
		rc = can_config_module_nr_valid(module_nr, &module_silent, &module_found);
#else // #ifdef LI_CAN_SLV_BOOT
		rc = can_config_module_nr_valid(module_nr, &table_pos, &module_silent, &module_found);
#endif // #ifdef LI_CAN_SLV_BOOT

#ifdef LI_CAN_SLV_DEBUG_DLOAD_START_EXTENDED
		LI_CAN_SLV_DEBUG_PRINT("\ncan_config_module_nr_valid() err:%04x", rc);
#endif // #ifdef // #ifdef LI_CAN_SLV_DEBUG_DLOAD_START_EXTENDED
		if (rc == LI_CAN_SLV_ERR_OK)
		{
#ifndef LI_CAN_SLV_BOOT
			rc = can_config_get_module_type(table_pos, xload_component.module_type);
#endif // #ifdef LI_CAN_SLV_BOOT
		}
#ifdef LI_CAN_SLV_DEBUG_DLOAD_START_EXTENDED
		LI_CAN_SLV_DEBUG_PRINT("\ncan_config_get_module_type() err:%04x module_type:%04s", rc, dload_buffer.component.module_type);
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_START_EXTENDED

		if (rc != LI_CAN_SLV_ERR_OK)
		{
			return rc;
		}

		// call handle
		if (NULL != dload_start_request_handle_funcp)
		{
#ifdef LI_CAN_SLV_DEBUG_DLOAD_START
			LI_CAN_SLV_DEBUG_PRINT("\ndload_start_request_handle_funcp()");
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_START
			rc = dload_start_request_handle_funcp(&xload_component);
		}
		else
		{
			rc = ERR_MSG_CAN_DLOAD_UNKNOWN_COMPONENT;
		}
	}
	else
	{
		rc = ERR_MSG_CAN_DLOAD_IN_PROGRESS;
	}

	if (rc != LI_CAN_SLV_ERR_OK)
	{
		li_can_slv_dload_termination(module_nr, rc);
	}

	return rc;
}

/**
 * @brief function receives the data block and stores it in the data structure and increments counter for bytes
 * per block and the counter for CAN objects\n
 * @remarks byte 1 of source data is the identifier of the object. the identifier is not checked in this function.
 * @param module_nr number of the module for the download
 * @param[in] src pointer to the source of the download data
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_dload_data(li_can_slv_module_nr_t module_nr, const byte_t *src)
{
	uint16_t i;
	li_can_slv_errorcode_t rc = LI_CAN_SLV_ERR_OK;

#ifdef LI_CAN_SLV_DEBUG_DLOAD_DATA_EXTENDED
	LI_CAN_SLV_DEBUG_PRINT("\nlcsdl_data in o:%u b:%u", dload_buffer.nr_of_can_objs, dload_buffer.bytes_cnt_of_block);
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_DATA_EXTENDED

	for (i = 1; i < CAN_DLC_MAX; i++)
	{
#ifdef LI_CAN_SLV_DLOAD_BUFFER_INTERNAL
		rc = dload_add_byte_to_buffer(src[i]);
#else // #ifdef LI_CAN_SLV_DLOAD_BUFFER_INTERNAL
		rc = li_can_slv_port_dload_add_byte_to_buffer(src[i]);
#endif // #ifdef LI_CAN_SLV_DLOAD_BUFFER_INTERNAL
		if (rc != LI_CAN_SLV_ERR_OK)
		{
			// terminate download if an error occurred in the dload buffer internal or external management
			li_can_slv_dload_termination(module_nr, rc);
			return (rc);
		}
	}
	dload_buffer.nr_of_can_objs++;
	dload_buffer.bytes_cnt_of_block += (CAN_DLC_MAX - 1);

#ifdef LI_CAN_SLV_DEBUG_DLOAD_DATA_EXTENDED
	LI_CAN_SLV_DEBUG_PRINT("\nlcsdl_data out o:%u b:%u", dload_buffer.nr_of_can_objs, dload_buffer.bytes_cnt_of_block);
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_DATA_EXTENDED
	return rc;
}

/**
 * @brief Function receives the number of bytes per block and the number of CAN objects and compares the values with
 * the values stored in the data structure. Private function dload_prog_data is called. after programming the block
 * to the destination, the acknowledge is sent in the dload_block_ack function.
 * @remarks byte 1 of source data is the identifier of the object. the identifier is not checked in this function.
 * @param module_nr number of the module for the download
 * @param[in] src pointer to the source of the download data
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_dload_data_block_end(li_can_slv_module_nr_t module_nr, const byte_t *src)
{
	li_can_slv_errorcode_t rc = LI_CAN_SLV_ERR_OK;
	uint16_t nr_of_can_objs;
	uint16_t dload_bytes_of_block;

	dload_bytes_of_block = ((uint16_t) src[1] << 8) + src[2];
	nr_of_can_objs = ((uint16_t) src[3] << 8) + src[4];

	/*--------------------------------------------------------------------------*/
	/* check received data block information									*/
	/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_DEBUG_DLOAD_DATA_EXTENDED
	LI_CAN_SLV_DEBUG_PRINT("\nlcsdl_data_block_end");
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_DATA_EXTENDED
#ifdef LI_CAN_SLV_DEBUG_DLOAD_DATA_EXTENDED
	LI_CAN_SLV_DEBUG_PRINT("\nlcsdl_data_block_end nr_of_can_objs:%u nr_of_obj_cnt:%u", dload_buffer.nr_of_can_objs, dload_buffer.nr_of_can_objs);
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_DATA_EXTENDED

	if (nr_of_can_objs == dload_buffer.nr_of_can_objs)
	{
		// it is possible that the master sends the whole block of data. As an example if
		dload_buffer.nr_bytes_end = (dload_buffer.bytes_cnt_of_block - dload_bytes_of_block);

		if (dload_buffer.nr_bytes_end > 0)
		{
#ifdef LI_CAN_SLV_DEBUG_DLOAD_EXTENDED
			LI_CAN_SLV_DEBUG_PRINT("\nnr_bytes: %u", dload_buffer.nr_bytes);
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_EXTENDED

#ifdef LI_CAN_SLV_DLOAD_BUFFER_INTERNAL
			dload_remove_bytes(dload_buffer.nr_bytes_end); //remove trailing dummy bytes
#endif // #ifdef LI_CAN_SLV_DLOAD_BUFFER_INTERNAL
			dload_buffer.nr_bytes_end = 0;
		}

		// call handle
		if (NULL != dload_data_block_end_handle_funcp)
		{
#ifdef LI_CAN_SLV_DEBUG_DLOAD_EXTENDED
			LI_CAN_SLV_DEBUG_PRINT("\ndload_data_block_end_handle_funcp");
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_EXTENDED
			rc = dload_data_block_end_handle_funcp(&xload_component);
		}
		else
		{
			rc = ERR_MSG_CAN_DLOAD_UNKNOWN_COMPONENT;
		}

		if (rc == LI_CAN_SLV_ERR_OK)
		{
			dload_block_ackn(module_nr);
			dload_buffer.bytes_cnt_of_block = 0;
			dload_buffer.nr_of_can_objs = 0;
		}
		else
		{
			li_can_slv_dload_termination(module_nr, rc);
		}
	}
	else
	{
		rc = ERR_MSG_CAN_DLOAD_BYTE_COUNT_NOT_MATCH;
		li_can_slv_dload_termination(module_nr, rc);
	}

	return rc;
}

/**
 * @brief Function receives end of download identifier. after programming the whole file to destination calculate the crc
 * of the component and compare the result with the given crc of the dload_start_request. if the values are identical the
 * file download_id is programmed and the acknowledge is sent, otherwise a program_termination is send. The acknowledge is sent
 * after resetting the module to RUNNING mode.
 * @remarks byte 1 of source data is the identifier of the object. the identifier is not checked in this function.
 * @param module_nr number of the module for the download
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_dload_end(li_can_slv_module_nr_t module_nr)
{
	li_can_slv_errorcode_t rc = LI_CAN_SLV_ERR_OK;
#ifndef LI_CAN_SLV_BOOT
	uint16_t table_pos;
#endif // #ifndef LI_CAN_SLV_BOOT
	uint16_t module_found;
	can_config_module_silent_t module_silent;
	li_can_slv_xload_end_handle_status_t end_handle_status = LI_CAN_SLV_XLOAD_STACK_END_HANDLING;

#ifdef LI_CAN_SLV_DEBUG_DLOAD
	LI_CAN_SLV_DEBUG_PRINT("\nlcsdl_end component name:%s", xload_component.name);
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD

#ifdef LI_CAN_SLV_BOOT
	rc = can_config_module_nr_valid(module_nr, &module_silent, &module_found);
#else // #ifdef LI_CAN_SLV_BOOT
	rc = can_config_module_nr_valid(module_nr, &table_pos, &module_silent, &module_found);
#endif // #ifdef LI_CAN_SLV_BOOT

	if (rc != LI_CAN_SLV_ERR_OK)
	{
		return rc;
	}

	// call handle
	if (NULL != dload_end_handle_funcp)
	{
#ifdef LI_CAN_SLV_DEBUG_DLOAD
		LI_CAN_SLV_DEBUG_PRINT("\ndload_end_handle_funcp");
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD
		rc = dload_end_handle_funcp(&xload_component, &end_handle_status);
	}
	else
	{
		rc = ERR_MSG_CAN_DLOAD_UNKNOWN_COMPONENT;
	}

	if (rc == LI_CAN_SLV_ERR_OK)
	{
#ifndef LI_CAN_SLV_BOOT
		if (LI_CAN_SLV_XLOAD_STACK_END_HANDLING == end_handle_status)
		{
			rc = li_can_slv_dload_end_ackn(module_nr);
		}
#endif // #ifndef LI_CAN_SLV_BOOT
	}
	else
	{
#ifdef LI_CAN_SLV_DEBUG_DLOAD_END
		LI_CAN_SLV_DEBUG_PRINT("\nli_can_slv_dload_end termination module_nr:%d, rc:%04x", module_nr, rc);
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_END
		li_can_slv_dload_termination(module_nr, rc);
	}
	return rc;
}

/**
 * @brief this function starts the download communication
 * @param dload_component
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_dload_start(li_can_slv_xload_component_t *dload_component)
{
	li_can_slv_errorcode_t rc = LI_CAN_SLV_ERR_OK;

#ifdef LI_CAN_SLV_DEBUG_DLOAD_START
	LI_CAN_SLV_DEBUG_PRINT("\nlcsdl_start");
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_START

	xload_in_progress = LI_CAN_SLV_XLOAD_IN_PROGRESS;

	if (rc != LI_CAN_SLV_ERR_OK)
	{
		return rc;
	}

	// call handle
	if (NULL != dload_start_handle_funcp)
	{
#ifdef LI_CAN_SLV_DEBUG_DLOAD_START_EXTENDED
		LI_CAN_SLV_DEBUG_PRINT("\nlcsdl_start_handle_funcp");
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_START_EXTENDED
		rc = dload_start_handle_funcp(dload_component);
	}
	else
	{
		rc = ERR_MSG_CAN_DLOAD_UNKNOWN_COMPONENT;
	}

	/*----------------------------------------------------------------------*/
	/* send download program start acknowledge								*/
	/*----------------------------------------------------------------------*/
	if (rc == LI_CAN_SLV_ERR_OK)
	{
		/* send download start acknowledge for hex files */
		rc = li_can_slv_dload_start_ackn(dload_component->module_nr, LI_CAN_SLV_XLOAD_MAX_BYTES_PER_BLOCK_HEX_DLOAD, IO_DLOAD_OBJ_TIME_DELAY_MS_HEX);
	}
	else
	{
		li_can_slv_dload_termination(dload_component->module_nr, rc);
	}
	return rc;
}

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief function writes the identifier of download end acknowledge to destination buffer. (without data)
 * @remarks the first byte of destination buffer is the identifier of the data object.
 * @param module_nr number of the module for the download
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_dload_end_ackn(li_can_slv_module_nr_t module_nr)
{
	byte_t acknowledge_buffer[8];
	li_can_slv_errorcode_t rc;

	/*----------------------------------------------------------------------*/
	/* clear component name for next correct download                       */
	/*----------------------------------------------------------------------*/
	can_port_memory_set(&xload_component.name[0], 0x00U, 8U);

	acknowledge_buffer[0] = CAN_ASYNC_DL_END_ACKN;
	can_port_memory_set(&acknowledge_buffer[1], 0x00, 7);
	rc = can_async_send_data_to_async_ctrl_tx_queue(module_nr, CAN_ASYNC_CTRL_TX_TYPE_SLAVE, acknowledge_buffer);
	xload_in_progress = LI_CAN_SLV_XLOAD_IDLE;
	return rc;
}
#endif	// #if defined(OUTER) || defined(OUTER_APP)

/**
 * @brief function writes the identifier of download termination and a error code to destination buffer.
 * @remarks	the first byte of destination buffer is the identifier of the data object.
 * @param module_nr number of the module for the download
 * @param error reason of termination
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_dload_termination(li_can_slv_module_nr_t module_nr, uint32_t error)
{
	byte_t acknowledge_buffer[8];
	li_can_slv_errorcode_t rc;
#ifdef LI_CAN_SLV_DEBUG_DLOAD
	LI_CAN_SLV_DEBUG_PRINT("\ndload_termination() err:%08lx", error);
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD

	/*----------------------------------------------------------------------*/
	/* clear component name for next correct download                       */
	/*----------------------------------------------------------------------*/
	can_port_memory_set(&xload_component.name[0], 0x00U, 8U);

	error = MSG_SG_EXPAND(error) | MSG_GROUP_CAN;
	acknowledge_buffer[0] = CAN_ASYNC_DL_TERMINATION;
	acknowledge_buffer[1] = (byte_t)((error & 0xFF000000) >> 24);
	acknowledge_buffer[2] = (byte_t)((error & 0x00FF0000) >> 16);
	acknowledge_buffer[3] = (byte_t)((error & 0x0000FF00) >> 8);
	acknowledge_buffer[4] = (byte_t)(error & 0x000000FF);
	acknowledge_buffer[5] = 0x00;
	acknowledge_buffer[6] = 0x00;
	acknowledge_buffer[7] = (byte_t)((MSG_GROUP_CAN >> 24) | (MSG_SG_EXTRACT(error)));
	rc = can_async_send_data_to_async_ctrl_tx_queue(module_nr, CAN_ASYNC_CTRL_TX_TYPE_SLAVE, acknowledge_buffer);
	xload_in_progress = LI_CAN_SLV_XLOAD_IDLE; // download terminated
	return rc;
}

#ifdef LI_CAN_SLV_ULOAD
/*--------------------------------------------------------------------------*/
/* Upload																	*/
/*--------------------------------------------------------------------------*/
/**
 * @brief function receives the upload info request short data. The size of the given component
 * is calculated and the private function li_can_slv_uload_info_answer is called.
 * @remarks byte 1 of source data is the identifier of the object. the ID is not checked in this function.
 * @param module_nr number of the module for the download
 * @param[in] src pointer to the source of the download data
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_uload_info_request_short(li_can_slv_module_nr_t module_nr, const byte_t *src)
{
	li_can_slv_errorcode_t rc = LI_CAN_SLV_ERR_OK;

	xload_component.index_of_request = src[1];
	xload_component.name[0] = (char_t)(0x20U + (src[2] >> 2U));
	xload_component.name[1] = (char_t)(0x20U + ((src[2] & 0x03U) << 4U) + ((src[3] & 0xF0) >> 4U));
	xload_component.name[2] = (char_t)(0x20U + ((src[3] & 0x0FU) << 2U) + ((src[4] & 0xC0) >> 6U));
	xload_component.name[3] = (char_t)(0x20U + (src[4] & 0x3FU));
	xload_component.name[4] = (char_t)(0x20U + ((src[5] & 0xFCU) >> 2U));
	xload_component.name[5] = (char_t)(0x20U + ((src[5] & 0x03U) << 4U) + ((src[6] & 0xF0) >> 4U));
	xload_component.name[6] = (char_t)(0x20U + ((src[6] & 0x0FU) << 2U) + ((src[7] & 0xC0) >> 6U));
	xload_component.name[7] = (char_t)(0x20U + (src[7] & 0x3FU));
	xload_component.name[8] = '\0';

#ifdef LI_CAN_SLV_DEBUG_DLOAD_VERSION
	LI_CAN_SLV_DEBUG_PRINT("\nlcsdl_ver_req short");
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_VERSION

	rc = uload_info_request_pre_handle(module_nr);
	return rc;
}

/**
 * @brief function writes the info answer to destination buffer
 * @remarks the first byte of destination buffer is the identifier of the data object.
 * @param module_nr number of the module for the download
 * @param index index of request (bytes 2 of data)
 * @param size_of_component
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_uload_info_answer(li_can_slv_module_nr_t module_nr, byte_t index, uint32_t size_of_component)
{
	uint8_t data[8];
	li_can_slv_errorcode_t rc;

#ifdef LI_CAN_SLV_BOOT
	li_can_slv_set_mode(LI_CAN_SLV_MODE_OPERATIONAL);
	(void)can_main_enable();
#endif

#ifdef LI_CAN_SLV_DEBUG_DLOAD_VERSION
	LI_CAN_SLV_DEBUG_PRINT("\nlcsdl_ver_ans crc:0x%04X and id:%lu", crc, download_id);
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD_VERSION
	data[0] = CAN_ASYNC_UL_INFO_ANSWER;
	data[1] = (uint8_t) index;
	data[2] = (uint8_t)((size_of_component & 0xFF000000) >> 24);
	data[3] = (uint8_t)((size_of_component & 0x00FF0000) >> 16);
	data[4] = (uint8_t)((size_of_component & 0x0000FF00) >> 8);
	data[5] = (uint8_t)(size_of_component & 0x000000FF);
	data[6] = 0x00U;
	data[7] = 0x00U;
	rc = can_async_send_data_to_async_ctrl_tx_queue(module_nr, CAN_ASYNC_CTRL_TX_TYPE_SLAVE, data);
	return rc;
}

/**
 * @brief function receives the first 7 characters of the component name to upload and saves
 * it in the data structure member name_of_component (uload_handling_t)
 * @remarks byte 1 of source data is the identifier of the object. the identifier is not
 * checked in this function.
 * @param module_nr number of the module for the upload
 * @param src data source
 * @return #li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_uload_start_request1(li_can_slv_module_nr_t module_nr, const byte_t *src)
{
	uint16_t i;

	dload_buffer.nr_of_can_objs = 0;
	dload_buffer.bytes_cnt_of_block = 0;
	xload_component.module_nr = module_nr;
	if (xload_in_progress == LI_CAN_SLV_XLOAD_IDLE)
	{
		for (i = 0; i < (CAN_DLC_MAX - 1); i++)
		{
			xload_component.name[i] = src[i + 1];
		}
	}
	return LI_CAN_SLV_ERR_OK;
}

/**
 * @brief function receives the last character of the component name, the maximum bytes
 * per block to send and a time delay between the CAN objects and saves it in the
 * corresponding structure members. The private function uload_data is called to manage the upload.
 * @remarks byte 1 of source data is the identifier of the object. the identifier is not
 * checked in this function.
 * @param module_nr number of the module for the upload
 * @param src data source
 * @return #li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_uload_start_request2(li_can_slv_module_nr_t module_nr, const byte_t *src)
{
	li_can_slv_errorcode_t rc = LI_CAN_SLV_ERR_OK;

	if (xload_in_progress == LI_CAN_SLV_XLOAD_IDLE)
	{
		xload_in_progress = LI_CAN_SLV_XLOAD_IN_PROGRESS; // dl in progress
		xload_component.module_nr = module_nr;
		xload_component.name[7] = src[1];
		xload_component.name[8] = '\0';
		dload_buffer.max_bytes_per_block = src[2] << 8;
		dload_buffer.max_bytes_per_block += src[3];
		dload_buffer.delay_between_objs_msec = src[4];

		// call handle
		if (NULL != uload_start_request_handle_funcp)
		{
#ifdef LI_CAN_SLV_DEBUG_DLOAD
			LI_CAN_SLV_DEBUG_PRINT("\nuload_start_request_handle_funcp");
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD
			rc = uload_start_request_handle_funcp(&xload_component);
		}
		else
		{
			rc = ERR_MSG_CAN_DLOAD_UNKNOWN_COMPONENT;
		}
	}
	else
	{
		rc = ERR_MSG_CAN_DLOAD_IN_PROGRESS;
	}

	if (rc != LI_CAN_SLV_ERR_OK)
	{
		li_can_slv_dload_termination(module_nr, rc);
	}
	return rc;
}

/**
 * @param data
 * @param data_length
 * @return #li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_uload_data_from_buffer(uint8_t *data, uint16_t data_length)
{
	li_can_slv_errorcode_t rc = LI_CAN_SLV_ERR_OK;
	uint16_t i;
	uint16_t state;
	uint32_t systick_old;
	byte_t buffer[8];

	dload_buffer.nr_of_can_objs = 0;

	state = ULOAD_STATE_FILL_BUFFER;

	systick_old = li_can_slv_port_get_system_ticks();

	while (state != ULOAD_STATE_END)
	{
		switch (state)
		{
			case ULOAD_STATE_FILL_BUFFER:
				for (i = 1; i < CAN_DLC_MAX; i++)
				{
					if (dload_buffer.bytes_cnt_of_block < data_length)
					{
						/* fill data bytes */
						buffer[i] = data[dload_buffer.bytes_cnt_of_block];
						dload_buffer.bytes_cnt_of_block++;
					}
					else
					{
						/* fill dummy bytes for last message */
						buffer[i] = 0x00;
					}
				}
				state = ULOAD_STATE_SEND_MESSAGE;
				break;

			case ULOAD_STATE_SEND_MESSAGE:
				if (li_can_slv_port_get_system_ticks() > (systick_old + dload_buffer.delay_between_objs_msec))
				{

					buffer[0] = CAN_ASYNC_DL_DATA;

					/* send data as asynchronous control message */
					rc = can_async_send_data_to_async_ctrl_tx_queue(xload_component.module_nr, CAN_ASYNC_CTRL_TX_TYPE_SLAVE, buffer);

					if (rc == ERR_MSG_CAN_ASYNC_CTRL_TX_QUEUE_OVERFLOW)
					{
						/* overflow detected -> clear error and try one more */
						rc = LI_CAN_SLV_ERR_OK;
					}
					else
					{
						/* in case of any error return */
						if (rc != LI_CAN_SLV_ERR_OK)
						{
							return (rc);
						}
						else
						{
							/* success of can object transmission */
							dload_buffer.nr_of_can_objs++;

							/* check end of transmission */
							if (dload_buffer.bytes_cnt_of_block < data_length)
							{
								/* more data valid */
								state = ULOAD_STATE_FILL_BUFFER;
							}
							else
							{
								/* end of transmission reached */

								if (rc == LI_CAN_SLV_ERR_OK)
								{
									rc = li_can_slv_uload_data_block_end(xload_component.module_nr);
								}

								state = ULOAD_STATE_END;
							}
						}
					}
					systick_old = li_can_slv_port_get_system_ticks();
				}
				break;

			default:
				break;
		}
	}
	return rc;
}

/**
 * @brief function reads the identifier of block acknowledge from source buffer. (without data)
 * @remarks the first byte of source buffer is the identifier of the data object. The private
 * function uload_data is called to continue the upload.
 * @param module_nr number of the module for the upload
 * @param src data source buffer
 * @return #li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_uload_block_ackn(li_can_slv_module_nr_t module_nr, const byte_t *src)
{
	li_can_slv_errorcode_t rc = LI_CAN_SLV_ERR_OK;
	src = src; //dummy assignment
	module_nr = module_nr; //dummy assignment

	// call handle
	if (NULL != uload_block_ack_handle_funcp)
	{
#ifdef LI_CAN_SLV_DEBUG_DLOAD
		LI_CAN_SLV_DEBUG_PRINT("\nuload_block_ack_handle_funcp");
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD
		rc = uload_block_ack_handle_funcp(&xload_component);
	}
	else
	{
		rc = ERR_MSG_CAN_DLOAD_UNKNOWN_COMPONENT;
	}
	return rc;
}

/**
 * @brief function reads the identifier of download end acknowledge from source buffer. (without data)
 * @remarks the first byte of source buffer is the identifier of the data object.
 * @param module_nr number of the module for the upload
 * @param src data source buffer
 * @return #li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_uload_end_ackn(li_can_slv_module_nr_t module_nr, const byte_t *src)
{
	module_nr = module_nr; //dummy assignment
	src = src; //dummy assignment
	xload_in_progress = LI_CAN_SLV_XLOAD_IDLE;
	return LI_CAN_SLV_ERR_OK;
}

/**
 * @brief function receives the identifier of download termination and a errorcode from source buffer.
 * @remarks the first byte of source buffer is the identifier of the data object.
 * @param module_nr number of the module for the upload
 * @param src source buffer
 * @param error reason of termination
 * @return #li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_uload_termination(li_can_slv_module_nr_t module_nr, const byte_t *src, uint32_t *error)
{
	module_nr = module_nr; //dummy assignment
	src = src; //dummy assignment
	error = error; //dummy assignment
	dload_buffer.block_status = DLOAD_END;
	xload_in_progress = LI_CAN_SLV_XLOAD_IDLE;
	return LI_CAN_SLV_ERR_OK;
}
#endif // #ifdef LI_CAN_SLV_ULOAD

#ifdef LI_CAN_SLV_ULOAD
/**
 * @brief Function send end of up/download identifier to the async TX Queue.
 * @remarks byte 1 of source data is the identifier of the object.
 * @param module_nr number of the module for the upload
 * @return #li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_uload_end(li_can_slv_module_nr_t module_nr)
{
	byte_t buffer[8];
	li_can_slv_errorcode_t rc;
	uint16_t icnt;

	dload_buffer.block_status = DLOAD_END;
	buffer[0] = CAN_ASYNC_DL_END;
	for (icnt = 1; icnt <= 7; icnt++)
	{
		buffer[icnt] = 0x00;
	}

	rc = can_async_send_data_to_async_ctrl_tx_queue(module_nr, CAN_ASYNC_CTRL_TX_TYPE_SLAVE, buffer);
	return rc;
}
#endif // #ifdef LI_CAN_SLV_ULOAD

#ifdef LI_CAN_SLV_ULOAD
/**
 * @param module_nr
 * @return #li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t li_can_slv_uload_data_block_end(li_can_slv_module_nr_t module_nr)
{
	byte_t buffer[8];
	li_can_slv_errorcode_t rc;

	buffer[0] = CAN_ASYNC_DL_DATA_BLOCK_END;
	buffer[1] = (byte_t)((dload_buffer.bytes_cnt_of_block & 0xFF00) >> 8);
	buffer[2] = (byte_t)(dload_buffer.bytes_cnt_of_block & 0x00FF);
	buffer[3] = (byte_t)((dload_buffer.nr_of_can_objs & 0xFF00) >> 8);
	buffer[4] = (byte_t)(dload_buffer.nr_of_can_objs & 0x00FF);

	can_port_memory_set(&buffer[5], 0x00, 3);

	do
	{
		rc = can_async_send_data_to_async_ctrl_tx_queue(module_nr, CAN_ASYNC_CTRL_TX_TYPE_SLAVE, buffer);
	}
	while (rc != LI_CAN_SLV_ERR_OK);

	return rc;
}
#endif // #ifdef LI_CAN_SLV_ULOAD

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/
/**
 * @brief function writes the version answer to destination buffer
 * @remarks the first byte of destination buffer is the identifier of the data object.
 * @param module_nr number of the module for the download
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t dload_version_request_pre_handle(li_can_slv_module_nr_t module_nr)
{
	li_can_slv_errorcode_t rc = LI_CAN_SLV_ERR_OK;
#ifndef LI_CAN_SLV_BOOT
	uint16_t table_pos;
#endif // #ifndef LI_CAN_SLV_BOOT
	uint16_t module_found;
	can_config_module_silent_t module_silent;
	li_can_slv_xload_end_handle_status_t end_handle_status = LI_CAN_SLV_XLOAD_STACK_END_HANDLING;

#ifdef LI_CAN_SLV_BOOT
	rc = can_config_module_nr_valid(module_nr, &module_silent, &module_found);
#else // #ifdef LI_CAN_SLV_BOOT
	rc = can_config_module_nr_valid(module_nr, &table_pos, &module_silent, &module_found);
#endif // #ifdef LI_CAN_SLV_BOOT

	if (rc != LI_CAN_SLV_ERR_OK)
	{
		return rc;
	}

	// call download handle
	if (NULL != dload_version_request_handle_funcp)
	{
#ifdef LI_CAN_SLV_DEBUG_DLOAD
		LI_CAN_SLV_DEBUG_PRINT("\ndload_version_request_handle:%s", xload_component.name);
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD
		xload_component_version_request.module_nr = module_nr;
		rc = dload_version_request_handle_funcp(&xload_component_version_request, &end_handle_status);
	}
	else
	{
		rc = ERR_MSG_CAN_DLOAD_UNKNOWN_COMPONENT;
	}

	/*----------------------------------------------------------------------*/
	/* clear component name for next correct download                       */
	/*----------------------------------------------------------------------*/
	can_port_memory_set(&xload_component_version_request.name[0], 0x00U, 8U);

	if (rc == LI_CAN_SLV_ERR_OK)
	{
		if (LI_CAN_SLV_XLOAD_STACK_END_HANDLING == end_handle_status)
		{
			rc = li_can_slv_dload_version_answer(module_nr, xload_component_version_request.index_of_request, xload_component_version_request.crc, xload_component_version_request.download_id);
		}
	}
	else
	{
#ifdef LI_CAN_SLV_DEBUG_DLOAD
		LI_CAN_SLV_DEBUG_PRINT("\ndload_version_request_handle, dload_termination for:%s", xload_component_version_request.name);
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD
		li_can_slv_dload_termination(module_nr, rc);
	}
	return (rc);
}

/**
 * @brief function writes the identifier of block acknowledge to destination buffer. (without data)
 * @remarks the first byte of destination buffer is the identifier of the data object.
 * @param module_nr number of the module for the download
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t dload_block_ackn(li_can_slv_module_nr_t module_nr)
{
	byte_t acknowledge_buffer[8];
	li_can_slv_errorcode_t rc;

	acknowledge_buffer[0] = CAN_ASYNC_DL_BLOCK_ACKN;

	can_port_memory_set(&acknowledge_buffer[1], 0x00, 7);

	rc = can_async_send_data_to_async_ctrl_tx_queue(module_nr, CAN_ASYNC_CTRL_TX_TYPE_SLAVE, acknowledge_buffer);
	return rc;
}

#ifdef LI_CAN_SLV_ULOAD
/**
 * @brief function writes the size information to the destination buffer
 * @remarks the first byte of destination buffer is the identifier of the data object.
 * @param module_nr number of the module for the download
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static li_can_slv_errorcode_t uload_info_request_pre_handle(li_can_slv_module_nr_t module_nr)
{
	li_can_slv_errorcode_t rc = LI_CAN_SLV_ERR_OK;
#ifndef LI_CAN_SLV_BOOT
	uint16_t table_pos;
#endif // #ifndef LI_CAN_SLV_BOOT
	uint16_t module_found;
	can_config_module_silent_t module_silent;
	li_can_slv_xload_end_handle_status_t end_handle_status = LI_CAN_SLV_XLOAD_STACK_END_HANDLING;
	uint32_t size_of_component = 0;

#ifdef LI_CAN_SLV_BOOT
	rc = can_config_module_nr_valid(module_nr, &module_silent, &module_found);
#else // #ifdef LI_CAN_SLV_BOOT
	rc = can_config_module_nr_valid(module_nr, &table_pos, &module_silent, &module_found);
#endif // #ifdef LI_CAN_SLV_BOOT

	if (rc != LI_CAN_SLV_ERR_OK)
	{
		return rc;
	}
	// call uload handle
	if (NULL != uload_info_request_handle_funcp)
	{
#ifdef LI_CAN_SLV_DEBUG_DLOAD
		LI_CAN_SLV_DEBUG_PRINT("\nuload_info_request_handle:%s", dload_buffer.component.name);
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD
		xload_component.module_nr = module_nr;
		rc = uload_info_request_handle_funcp(&xload_component, &size_of_component, &end_handle_status);
	}
	else
	{
		rc = ERR_MSG_CAN_DLOAD_UNKNOWN_COMPONENT;
	}

	/*----------------------------------------------------------------------*/
	/* clear component name for next correct upload                         */
	/*----------------------------------------------------------------------*/
	can_port_memory_set(&xload_component.name[0], 0x00U, 8U);

	if (rc == LI_CAN_SLV_ERR_OK)
	{
		if (LI_CAN_SLV_XLOAD_STACK_END_HANDLING == end_handle_status)
		{
			rc = li_can_slv_uload_info_answer(module_nr, xload_component.index_of_request, size_of_component);
		}
	}
	else
	{
#ifdef LI_CAN_SLV_DEBUG_DLOAD
		LI_CAN_SLV_DEBUG_PRINT("\ndload_version_request_handle, dload_termination for:%s", dload_buffer.component.name);
#endif // #ifdef LI_CAN_SLV_DEBUG_DLOAD
		li_can_slv_dload_termination(module_nr, rc);
	}
	return (rc);
}
#endif // #ifdef LI_CAN_SLV_ULOAD

/**
 * @return True if download or upload is active, False otherwise
 */
uint8_t li_can_slv_xload_active(void)
{
	if (xload_in_progress == LI_CAN_SLV_XLOAD_IN_PROGRESS)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/** @} */

