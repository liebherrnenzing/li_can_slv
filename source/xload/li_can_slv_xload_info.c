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
 * @file li_can_slv_xload_info.c
 * @brief
 * @addtogroup lcs_xload_info
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "li_can_slv_xload_info.h"
#include "io_can_port.h"
#include "io_can_errno.h"

#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_debug.h"
#endif // #ifdef LI_CAN_SLV_DEBUG

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/
#ifdef _KEIL
#pragma renameclass(xdata=xsysinfo)
#pragma NOINIT
li_can_slv_xload_info_t XHUGE xload_info;
#pragma INIT
#endif // #ifdef _KEIL

#ifdef __TASKING__
#pragma noclear
static li_can_slv_xload_info_t xload_info;
#pragma clear
#endif // #ifdef __TASKING__

#ifdef __DCC__
extern li_can_slv_xload_info_t xload_info;
#endif // #ifdef __DCC__

#ifdef __GNUC__
li_can_slv_xload_info_t xload_info __attribute__((section(".xload_info")));
#endif // #ifdef __GNUC__

/*
#ifdef __GNUC__
#pragma section ".nvram" aw 4
li_can_slv_xload_info_t xload_info;
#pragma section
#endif // #ifdef __GNUC__
*/

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/
static void li_can_slv_xload_info_update_crc(void);

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
extern uint16_t li_can_slv_xload_info_crc16_init(void);
extern uint16_t li_can_slv_xload_info_crc16_update(uint16_t crc, const void *data, size_t data_len);

/**
 * @brief initialization of the xload_info memory
 * @return ERR_MSG_CAN_XLOAD_INFO_BAD_CRC or LI_CAN_SLV_ERR_OK
 */
li_can_slv_errorcode_t li_can_slv_xload_info_init(void)
{
	uint16_t xload_info_state;
#ifdef LI_CAN_SLV_DEBUG_XLOAD_INFO
	uint16_t tmp;
#endif // #ifdef LI_CAN_SLV_DEBUG_XLOAD_INFO

	li_can_slv_xload_info_check_crc(&xload_info_state);
	if (xload_info_state == LI_CAN_SLV_XLOAD_INFO_NOT_VALID)
	{
#ifdef LI_CAN_SLV_DEBUG_XLOAD_INFO
		LI_CAN_SLV_DEBUG_PRINT("\n\n => not valid");
		LI_CAN_SLV_DEBUG_PRINT("xload_info clean...");
#endif // #ifdef LI_CAN_SLV_DEBUG_XLOAD_INFO
		li_can_slv_xload_info_clean();
		return ERR_MSG_CAN_XLOAD_INFO_BAD_CRC;
	}
	else
	{
#ifdef LI_CAN_SLV_DEBUG_XLOAD_INFO
		LI_CAN_SLV_DEBUG_PRINT("\n\n => valid");
		LI_CAN_SLV_DEBUG_PRINT("\nxload_info.mode: %d", xload_info.mode);
		LI_CAN_SLV_DEBUG_PRINT("\nxload_info.dload.module_nr = %d", xload_info.dload.module_nr);
		LI_CAN_SLV_DEBUG_PRINT("\nxload_info.dload.name = ");
		for (tmp = 0; tmp < DLOAD_COMPONENT_NAME_LENGTH; ++tmp)
		{
			LI_CAN_SLV_DEBUG_PRINT("%c", xload_info.dload.name[tmp]);
		}
		LI_CAN_SLV_DEBUG_PRINT("\nxload_info.dload.crc = 0x%x", xload_info.dload.crc);
		LI_CAN_SLV_DEBUG_PRINT("\nxload_info.dload.download_id = %d", xload_info.dload.download_id);
		LI_CAN_SLV_DEBUG_PRINT("\nxload_info.dload.index_of_request = %d", xload_info.dload.index_of_request);
		LI_CAN_SLV_DEBUG_PRINT("\nxload_info.dload.module_type = ");
		for (tmp = 0; tmp < CAN_CONFIG_TYPE_STRING_LENGTH; ++tmp)
		{
			LI_CAN_SLV_DEBUG_PRINT("%c", xload_info.dload.module_type[tmp]);
		}
		LI_CAN_SLV_DEBUG_PRINT("\nxload_info.can_baudrate = %d", xload_info.can_baudrate);
		LI_CAN_SLV_DEBUG_PRINT("\nxload_info.crc = 0x%x", xload_info.crc);

#ifdef LI_CAN_SLV_XLOAD_INFO_USER
		LI_CAN_SLV_DEBUG_PRINT("\nxload_info.user1 = %d", xload_info.user1);
		LI_CAN_SLV_DEBUG_PRINT("\nxload_info.user2 = %d", xload_info.user2);
#endif // #ifdef LI_CAN_SLV_XLOAD_INFO_USER

#endif // #ifdef LI_CAN_SLV_DEBUG_XLOAD_INFO
	}
	return LI_CAN_SLV_ERR_OK;
}

/**
 * @brief Initialize the xload (download/upload) info variables.
 */
void li_can_slv_xload_info_clean(void)
{
	xload_info.mode = LI_CAN_SLV_XLOAD_INFO_MODE_DOWNLOAD_RUNNING;
	xload_info.dload.module_nr = 125;
	li_can_slv_port_string_n_cpy((char_t *) xload_info.dload.name, (const char_t *) "\0\0\0\0\0\0\0\0\0", DLOAD_COMPONENT_NAME_LENGTH);
	xload_info.dload.crc = 0;
	xload_info.dload.download_id = 0;
	xload_info.dload.index_of_request = 0;
#ifdef LI_CAN_SLV_XLOAD_INFO_USER
	xload_info.user1 = 0;
	xload_info.user2 = 0;
#endif // #ifdef LI_CAN_SLV_XLOAD_INFO_USER
	li_can_slv_port_string_n_cpy((char_t *) xload_info.dload.module_type, (const char_t *) "\0\0\0\0\0", CAN_CONFIG_TYPE_STRING_LENGTH);
	li_can_slv_xload_info_update_crc();
}

/**
 * @brief checks the CRC of the xload_info data structure
 * @param[out] state current state of the xload info
 */
void li_can_slv_xload_info_check_crc(uint16_t *state)
{
	uint16_t crc;

	crc = li_can_slv_xload_info_crc16_init();
#ifdef LI_CAN_SLV_XLOAD_INFO_BACKWARD_COMPATIBILITY
	crc = li_can_slv_xload_info_crc16_update(crc, (uint8_t *) &xload_info, sizeof(li_can_slv_xload_info_t) - sizeof(uint16_t) - 2 * sizeof(uint8_t));
#else // #ifdef LI_CAN_SLV_XLOAD_INFO_USER
	// remove crc from crc calculation
	crc = li_can_slv_xload_info_crc16_update(crc, (uint8_t *) &xload_info, sizeof(li_can_slv_xload_info_t) - sizeof(uint16_t));
#endif // #ifdef LI_CAN_SLV_XLOAD_INFO_USER

	if (crc == xload_info.crc)
	{
		*state = LI_CAN_SLV_XLOAD_INFO_VALID;
	}
	else
	{
		*state = LI_CAN_SLV_XLOAD_INFO_NOT_VALID;
	}
}

/**
 * @remark Use this function to set the system info mode.
 * @param mode set the mode @see li_can_slv_xload_info_mode_t
 */
void li_can_slv_xload_info_set_mode(li_can_slv_xload_info_mode_t mode)
{
	xload_info.mode = mode;
	li_can_slv_xload_info_update_crc();
}

/**
 * @remark Use this function to read the xload info mode.
 * @param[out] mode pointer to the xload_info mode value
 */
void li_can_slv_xload_info_get_mode(li_can_slv_xload_info_mode_t *mode)
{
	*mode = xload_info.mode;
}

#ifdef LI_CAN_SLV_XLOAD_INFO_BACKWARD_COMPATIBILITY
/**
 * @remark Use this function to set the xload reset reason.
 * @param reset_reason set the reset_reason @see li_can_slv_xload_info_reset_reason_t
 */
void li_can_slv_xload_info_set_reset_reason(li_can_slv_xload_info_reset_reason_t reset_reason)
{
	xload_info.reset_reason = reset_reason;
	li_can_slv_xload_info_update_crc();
}

/**
 * @remark Use this function to read the xload reset reason.
 * @param[out] reset_reason pointer to the xload_info reset reason
 */
void li_can_slv_xload_info_get_reset_reason(li_can_slv_xload_info_reset_reason_t *reset_reason)
{
	*reset_reason = xload_info.reset_reason;
}
#endif // #ifdef LI_CAN_SLV_XLOAD_INFO_BACKWARD_COMPATIBILITY

/**
 * @brief this function returns a pointer to the current xload_info structure
 * @param pxload_info pointer to the current xload_info structure
 */
void li_can_slv_xload_info_get(li_can_slv_xload_info_t **pxload_info)
{
	*pxload_info = &xload_info;
}

/**
 * @brief set dload_component used in the xload_info
 * @param[in] dload_component pointer to dload_component of the xload_info structure
 */
void li_can_slv_xload_info_set_dload_component(const li_can_slv_xload_component_t *dload_component)
{
	li_can_slv_port_memory_cpy((li_can_slv_xload_component_t *) &xload_info.dload, dload_component, sizeof(li_can_slv_xload_component_t));
	li_can_slv_xload_info_update_crc();
}

/**
 * @brief get the dload_component from the xload_info
 * @remark Use this function to read the system info download structure.
 * @param[out] dload_component pointer to the li_can_slv_xload_component_t structure
 */
void li_can_slv_xload_info_get_dload_component(li_can_slv_xload_component_t *dload_component)
{
	li_can_slv_port_memory_cpy(dload_component, (li_can_slv_xload_component_t *) &xload_info.dload, sizeof(li_can_slv_xload_component_t));
}

/**
 * @param can_baudrate set the current baudrate
 */
void li_can_slv_xload_info_set_can_baudrate(uint16_t can_baudrate)
{
	xload_info.can_baudrate = can_baudrate;
	li_can_slv_xload_info_update_crc();
}

/**
 * @param can_baudrate get the current baudrate
 */
void li_can_slv_xload_info_get_can_baudrate(uint16_t *can_baudrate)
{
	*can_baudrate = xload_info.can_baudrate;
}

#ifdef LI_CAN_SLV_XLOAD_INFO_USER
/**
 * @param user1_data set the user1 data
 */
void li_can_slv_xload_info_set_user1(byte_t user1_data)
{
	xload_info.user1 = user1_data;
	li_can_slv_xload_info_update_crc();
}

/**
 * @param user1_data get the user1 data
 */
void li_can_slv_xload_info_get_user1(byte_t *user1_data)
{
	*user1_data = xload_info.user1;
}

/**
 * @param user1_data set the user1 data
 */
void li_can_slv_xload_info_set_user2(byte_t user2_data)
{
	xload_info.user2 = user2_data;
	li_can_slv_xload_info_update_crc();
}

/**
 * @param user2_data get the user2 data
 */
void li_can_slv_xload_info_get_user2(byte_t *user2_data)
{
	*user2_data = xload_info.user2;
}
#endif // #ifdef LI_CAN_SLV_XLOAD_INFO_USER

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/
/**
 * @brief this function updates the CRC of the xload_info structure
 */
static void li_can_slv_xload_info_update_crc(void)
{
	xload_info.crc = li_can_slv_xload_info_crc16_init();
#ifdef LI_CAN_SLV_XLOAD_INFO_BACKWARD_COMPATIBILITY
	xload_info.crc = li_can_slv_xload_info_crc16_update(xload_info.crc, (uint8_t *) &xload_info, sizeof(li_can_slv_xload_info_t) - sizeof(uint16_t) - 2 * sizeof(uint8_t));
#else // #ifdef LI_CAN_SLV_XLOAD_INFO_USER
	// remove crc from crc calculation
	xload_info.crc = li_can_slv_xload_info_crc16_update(xload_info.crc, (uint8_t *) &xload_info, sizeof(li_can_slv_xload_info_t) - sizeof(uint16_t));
#endif // #ifdef LI_CAN_SLV_XLOAD_INFO_USER
}
/** @} */

